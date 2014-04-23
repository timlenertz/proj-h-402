#ifndef DYPC_TREE_STRUCTURE_H_
#define DYPC_TREE_STRUCTURE_H_

#include "../mipmap_structure.h"
#include "../../enums.h"
#include "../../geometry/cuboid.h"
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

/**
 * Tree structure where the model is recursively subdivided into cuboid regions.
 * @tparam Splitter Tree structure splitter which defines how space is subdivided into cuboids.
 * @tparam Levels Mipmap levels of downsampling to generate.
 * @tparam PointsContainer Container used to store arrays of points.
 */
template<class Splitter, std::size_t Levels, class PointsContainer = std::deque<point>>
class tree_structure : public mipmap_structure {
private:
	using super = mipmap_structure;

public:
	using splitter = Splitter;
	using node = tree_structure_node<Splitter, Levels, PointsContainer>;
	static constexpr std::size_t levels = Levels;
	static constexpr std::size_t number_of_node_children = Splitter::number_of_node_children;
	
protected:
	struct no_load_t {};
	static constexpr no_load_t no_load = no_load_t();

	const std::size_t leaf_capacity_;

	node root_;
	cuboid root_cuboid_;
	PointsContainer all_points_[Levels];
	
	tree_structure(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, no_load_t) : mipmap_structure(Levels, dmin, damount, dmode, mod), leaf_capacity_(leaf_cap) { }
	
	void unload_();
	void load_(const cuboid& cub, unsigned depth = 0);

public:
	tree_structure(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, bool load_all_downsampled = true);
	
	std::size_t number_of_points(std::ptrdiff_t lvl = 0) const { assert(lvl >= 0 && lvl < levels); return all_points_[lvl].size(); }
	std::size_t number_of_nodes() const { return root_.number_of_nodes_in_branch(); }
	std::size_t size() const;
	
	void load_downsampled_points(std::ptrdiff_t lvl, unsigned depth, uniform_downsampling_previous_results_t& previous_results);
	void unload_downsampled_points(std::ptrdiff_t lvl);
	
	void load_downsampled_points(std::ptrdiff_t lvl, unsigned depth = 0) {
		uniform_downsampling_previous_results_t previous_results;
		load_downsampled_points(lvl, depth, previous_results);
	}
	
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
void tree_structure<Splitter, Levels, PointsContainer>::unload_() {
	root_ = node();
	for(auto& pts : all_points_) pts.clear();
}


template<class Splitter, std::size_t Levels, class PointsContainer>
tree_structure<Splitter, Levels, PointsContainer>::tree_structure(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, bool load_all_downsampled) :
mipmap_structure(Levels, dmin, damount, dmode, mod), leaf_capacity_(leaf_cap) {
	root_cuboid_ = Splitter::adjust_root_cuboid(mod.enclosing_cuboid());
	load_(root_cuboid_);
	if(load_all_downsampled) for(std::ptrdiff_t lvl = 1; lvl < Levels; ++lvl) load_downsampled_points(lvl);
}


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure<Splitter, Levels, PointsContainer>::load_downsampled_points(std::ptrdiff_t lvl, unsigned depth, uniform_downsampling_previous_results_t& previous_results) {
	assert(lvl >= 1 && lvl < Levels);

	PointsContainer downsampled;
	const auto& original_points = all_points_[0];
	super::template downsample_points_<typename PointsContainer::const_iterator, PointsContainer>(original_points.begin(), original_points.end(), lvl, root_cuboid_.area(), downsampled, previous_results);
	
	auto& level_points = all_points_[lvl];
	std::size_t c = 0;
	progress_foreach(downsampled, "Adding downsampled points, level " + std::to_string(lvl) + "...", [&](const point& pt) {
		root_.add_higher_level_point(lvl, pt, root_cuboid_, depth, leaf_capacity_);
	});
	root_.move_out_points(level_points, lvl);
	root_.finalize_move_out(level_points, lvl);
}

template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure<Splitter, Levels, PointsContainer>::unload_downsampled_points(std::ptrdiff_t lvl) {
	assert(lvl >= 1 && lvl < Levels);
	all_points_[lvl].clear();
}


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure<Splitter, Levels, PointsContainer>::load_(const cuboid& cub, unsigned depth) {	
	unload_();
	root_cuboid_ = Splitter::adjust_root_cuboid(cub);
	
	PointsContainer all_points_unordered;
	
	progress_foreach(super::model_, "Collecting points from model...", [&](const point& pt) {
		if(cub.in_range(pt)) all_points_unordered.push_back(pt);
	});
	std::cout << all_points_unordered.size() << std::endl;

	progress(all_points_unordered.size(), "Adding points with info to tree...", [&](progress_handle& pr) {
		root_.add_points_with_information(all_points_unordered, root_cuboid_, depth, leaf_capacity_, pr);
	});

	root_.move_out_points(all_points_[0]);
	root_.finalize_move_out(all_points_[0]);
}



template<class Splitter, std::size_t Levels, class PointsContainer>
std::size_t tree_structure<Splitter, Levels, PointsContainer>::size() const {
	std::size_t sz = sizeof(tree_structure) + root_.size();
	for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) sz += all_points_[lvl].size() * sizeof(point);
	return sz;
}

}

#endif
