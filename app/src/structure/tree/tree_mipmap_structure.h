#ifndef DYPC_TREE_MIPMAP_STRUCTURE_H_
#define DYPC_TREE_MIPMAP_STRUCTURE_H_

#include "tree_structure.h"
#include "../../downsampling.h"

namespace dypc {

static constexpr std::size_t tree_mipmap_structure_levels = 8;

template<class Splitter, std::size_t Levels = tree_mipmap_structure_levels>
class tree_mipmap_structure : public tree_structure<Splitter, Levels> {
private:
	const float mipmap_factor_;
	const downsampling_mode_t downsampling_mode_;

public:
	tree_mipmap_structure(std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, model& mod);

	float get_mipmap_factor() const { return mipmap_factor_; }
	downsampling_mode_t get_downsampling_mode() const { return downsampling_mode_; }
};



template<class Splitter, std::size_t Levels>
tree_mipmap_structure<Splitter, Levels>::tree_mipmap_structure(std::size_t leaf_capacity, float mmfac, downsampling_mode_t dmode, model& mod) :
tree_structure<Splitter, Levels>(leaf_capacity, mod), mipmap_factor_(mmfac), downsampling_mode_(dmode) {	
	using super = tree_structure<Splitter, Levels>;
	
	const auto& points = super::all_points_[0];
	
	float prob = 1.0 / mipmap_factor_;
	float area = mod.x_range() * mod.y_range() * mod.z_range();
	
	for(int lvl = 1; lvl < Levels; ++lvl) progress("Downsampling points, level " + std::to_string(lvl) + "...", 500, 5, [&]() {
		std::vector<point> downsampled;
		
		if(downsampling_mode_ == random_downsampling_mode) random_downsampling(points, prob, downsampled);
		else if(downsampling_mode_ == uniform_downsampling_mode) uniform_downsampling(points, prob, area, downsampled);

		auto& level_points = super::all_points_[lvl];
		level_points.reserve(downsampled.size());
		std::size_t c = 0;
		for(const point& pt : downsampled) {			
			super::root_.add_point(pt, super::root_cuboid_, leaf_capacity, lvl);
			set_progress(500 * (++c) / downsampled.size());
		}
		super::root_.move_out_points(level_points, lvl);

		prob /= mipmap_factor_;
	});
}

}

#endif
