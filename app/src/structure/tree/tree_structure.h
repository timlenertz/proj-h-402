#ifndef DYPC_TREE_STRUCTURE_H_
#define DYPC_TREE_STRUCTURE_H_

#include "../structure.h"
#include "../../cuboid.h"
#include "../../point.h"
#include "../../progress.h"
#include "../../model/model.h"
#include "../../downsampling.h"
#include "tree_structure_node.h"
#include <cassert>
#include <map>

#include <deque>
#include <vector>

namespace dypc {

template<class Splitter, std::size_t Levels, class PointsContainer = std::vector<point>>
class tree_structure : public structure {
public:
	using splitter = Splitter;
	using node = tree_structure_node<Splitter, Levels, PointsContainer>;
	static constexpr std::size_t levels = Levels;
	static constexpr std::size_t number_of_node_children = Splitter::number_of_node_children;
	
private:
	const std::size_t leaf_capacity_;
	const float mipmap_factor_;
	const downsampling_mode_t downsampling_mode_;
	const std::size_t downsampling_maximal_number_of_points_;

	std::map<const node*, uniform_downsampling_previous_results_t> node_uniform_downsampling_previous_results_;

	node root_;
	cuboid root_cuboid_;
	PointsContainer all_points_[Levels];
	
	void downsample_points_in_node_(float ratio, const node&, const cuboid&, PointsContainer&, unsigned depth);


public:
	tree_structure(std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod);
	
	std::size_t number_of_points(std::ptrdiff_t lvl = 0) const { assert(lvl >= 0 && lvl < levels); return all_points_[lvl].size(); }
	std::size_t number_of_nodes() const { return root_.number_of_nodes_in_branch(); }
	std::size_t size() const;
	
	const node& root_node() const { return root_; }
	const cuboid& root_cuboid() const { return root_cuboid_; }
	
	const PointsContainer& points_at_level(unsigned lvl = 0) const {
		assert(lvl < Levels); return all_points_[lvl];
	}

	std::ptrdiff_t node_points_offset(const node& nd, unsigned lvl = 0) const {
		return nd.points_begin(lvl) - all_points_[lvl].begin();
	}
};


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure<Splitter, Levels, PointsContainer>::downsample_points_in_node_(float ratio, const node& nd, const cuboid& cub, PointsContainer& output, unsigned depth) {
	if(nd.is_leaf() || nd.number_of_points() <= downsampling_maximal_number_of_points_) {
		float area = cub.area();
		if(downsampling_mode_ == random_downsampling_mode) random_downsampling(nd.points_begin(), nd.points_end(), ratio, output);
		else if(downsampling_mode_ == uniform_downsampling_mode) uniform_downsampling(nd.points_begin(), nd.points_end(), ratio, area, output, node_uniform_downsampling_previous_results_[&nd]);
		
	} else {
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) {
			auto child_cuboid = Splitter::node_child_cuboid(i, cub, nd.get_points_information(), depth);
			downsample_points_in_node_(ratio, nd.child(i), child_cuboid, output, depth + 1);
		}
	}
}




template<class Splitter, std::size_t Levels, class PointsContainer>
tree_structure<Splitter, Levels, PointsContainer>::tree_structure(std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod) :
leaf_capacity_(leaf_cap), mipmap_factor_(mmfac), downsampling_mode_(dmode), downsampling_maximal_number_of_points_(dmax), root_cuboid_(Splitter::root_cuboid(mod)) {
	PointsContainer all_points;
	progress("Collecting points from model...", mod.number_of_points(), [&]() {
		for(const auto& pt : mod) { all_points.push_back(pt); increment_progress(); }
	});
	progress("Adding points with info to tree...", mod.number_of_points(), [&]() {
		root_.add_points_with_information(all_points, root_cuboid_, 0, leaf_capacity_);
	});

	root_.move_out_points(all_points_[0]);
	root_.finalize_move_out(all_points_[0]);
	
	
	const auto& points = all_points_[0];
	float ratio = 1.0 / mipmap_factor_;	
	for(int lvl = 1; lvl < Levels; ++lvl) {
		PointsContainer downsampled;
		
		downsample_points_in_node_(ratio, root_, root_cuboid_, downsampled, 0);

		progress("Adding downsampled points, level " + std::to_string(lvl) + "...", downsampled.size(), [&]() {
			auto& level_points = all_points_[lvl];
			std::size_t c = 0;
			for(const point& pt : downsampled) {			
				root_.add_higher_level_point(lvl, pt, root_cuboid_, 0, leaf_capacity_);
				increment_progress();
			}
			root_.move_out_points(level_points, lvl);
			root_.finalize_move_out(level_points, lvl);
		});

		ratio /= mipmap_factor_;
	}
	
	node_uniform_downsampling_previous_results_.clear();
}


template<class Splitter, std::size_t Levels, class PointsContainer>
std::size_t tree_structure<Splitter, Levels, PointsContainer>::size() const {
	std::size_t sz = sizeof(tree_structure) + root_.size();
	for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) sz += all_points_[lvl].size() * sizeof(point);
	return sz;
}

}

#endif
