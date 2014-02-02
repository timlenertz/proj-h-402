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
		
	float minimal_distance = cub.minimal_distance(req.position);
	float maximal_distance = cub.maximal_distance(req.position);
	
	if(	! is_leaf &&
		maximal_distance > downsampling_start_distance_ &&
		maximal_distance - minimal_distance > additional_split_distance_difference_
	) return action_split;
	
	float distance;
	switch(downsampling_node_distance) {
		case minimal_node_distance: distance = minimal_distance; break;
		case maximal_node_distance: distance = maximal_distance; break;
		case mean_node_distance: distance = (maximal_distance + minimal_distance)/2.0; break;
		case center_node_distance: distance = glm::distance(req.position, cub.center()); break;
	};
	
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
