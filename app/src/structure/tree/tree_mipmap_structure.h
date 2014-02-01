#ifndef DYPC_TREE_MIPMAP_STRUCTURE_H_
#define DYPC_TREE_MIPMAP_STRUCTURE_H_

#include "tree_structure.h"
#include "../../downsampling.h"

namespace dypc {

static constexpr std::size_t tree_mipmap_structure_levels = 8;

template<class Splitter, std::size_t Levels = tree_mipmap_structure_levels>
class tree_mipmap_structure : public tree_structure<Splitter, Levels> {
private:
	using super = tree_structure<Splitter, Levels>;

	const float mipmap_factor_;
	const downsampling_mode_t downsampling_mode_;
	const std::size_t downsampling_maximal_number_of_points_;
	
	void downsample_points_in_node_(float ratio, const typename super::node&, const cuboid&, std::vector<point>&, unsigned depth) const;

public:
	tree_mipmap_structure(std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod);

	float get_mipmap_factor() const { return mipmap_factor_; }
	downsampling_mode_t get_downsampling_mode() const { return downsampling_mode_; }
	std::size_t get_downsampling_maximal_number_of_points() const { return downsampling_maximal_number_of_points_; }
};



template<class Splitter, std::size_t Levels>
void tree_mipmap_structure<Splitter, Levels>::downsample_points_in_node_(float ratio, const typename super::node& nd, const cuboid& cub, std::vector<point>& output, unsigned depth) const {
	if(nd.is_leaf() || nd.number_of_points() <= downsampling_maximal_number_of_points_) {
		float area = cub.area();
		if(downsampling_mode_ == random_downsampling_mode) random_downsampling(nd.points_begin(), nd.points_end(), ratio, output);
		else if(downsampling_mode_ == uniform_downsampling_mode) uniform_downsampling(nd.points_begin(), nd.points_end(), ratio, area, output);
		
	} else {
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) {
			auto child_cuboid = Splitter::node_child_cuboid(i, cub, nd.get_points_information(), depth);
			downsample_points_in_node_(ratio, nd.child(i), child_cuboid, output, depth + 1);
		}
	}
}



template<class Splitter, std::size_t Levels>
tree_mipmap_structure<Splitter, Levels>::tree_mipmap_structure(std::size_t leaf_capacity, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod) :
tree_structure<Splitter, Levels>(leaf_capacity, mod), mipmap_factor_(mmfac), downsampling_mode_(dmode), downsampling_maximal_number_of_points_(dmax) {	
	const auto& points = super::all_points_[0];
	
	float ratio = 1.0 / mipmap_factor_;	
	for(int lvl = 1; lvl < Levels; ++lvl) {
		std::vector<point> downsampled;
		
		downsample_points_in_node_(ratio, super::root_, super::root_cuboid_, downsampled, 0);

		progress("Adding downsampled points, level " + std::to_string(lvl) + "...", downsampled.size(), [&]() {
			auto& level_points = super::all_points_[lvl];
			level_points.reserve(downsampled.size());
			std::size_t c = 0;
			for(const point& pt : downsampled) {			
				super::root_.add_point(pt, super::root_cuboid_, 0, leaf_capacity, lvl);
				increment_progress();
			}
			super::root_.move_out_points(level_points, lvl);
		});

		ratio /= mipmap_factor_;
	}
	
}

}

#endif
