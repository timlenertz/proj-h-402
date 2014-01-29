#include "tree_structure_loader.h"
#include "../../ui/tree_structure_panel.h"
#include <cmath>
#include <algorithm>

namespace dypc {

std::ptrdiff_t tree_structure_loader::action_for_node_(const cuboid& cub, std::size_t number_of_points, bool is_leaf, const loader::request_t& req, std::size_t levels) const {
	auto intersection = req.view_frustum.contains_cuboid(cub);
	
	if(intersection == frustum::outside_frustum) {
		return action_skip;
	} else if(intersection == frustum::partially_inside_frustum && !is_leaf && number_of_points >= minimal_number_of_points_for_split_) {
		return action_split;
	} else if(levels <= 1) {
		return 0;
	}
		
	float distance = cub.maximal_distance(req.position);
	std::ptrdiff_t lvl = 0;
	if(distance >= downsampling_start_distance_) lvl = 1 + (distance - downsampling_start_distance_) / downsampling_step_distance_;
	if(lvl >= levels) lvl = levels - 1;
	
	return lvl;
}


::wxWindow* tree_structure_loader::create_panel(::wxWindow* parent) {
	tree_structure_panel* panel = new tree_structure_panel(parent);
	panel->set_loader(*this);
	return panel;
}


}
