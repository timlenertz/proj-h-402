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
	
	float distance = cuboid_distance_(req.position, cub, minimal_distance, maximal_distance);
	
	std::ptrdiff_t lvl = 0;
	if(distance >= downsampling_start_distance_) lvl = 1 + (distance - downsampling_start_distance_) / downsampling_step_distance_;
	if(lvl >= levels) lvl = levels - 1;
	
	return lvl;
}


float tree_structure_loader::cuboid_distance_(glm::vec3 position, const cuboid& cub, float min_dist, float max_dist) const {
	switch(downsampling_node_distance) {
		case minimal_node_distance: return min_dist;
		case maximal_node_distance: return max_dist;
		case mean_node_distance: return (max_dist + min_dist)/2.0;
		case center_node_distance: return glm::distance(position, cub.center());
	};
	return 0.0;
}


float tree_structure_loader::cuboid_distance_(glm::vec3 position, const cuboid& cub) const {
	float minimal_distance = cub.minimal_distance(position);
	float maximal_distance = cub.maximal_distance(position);
	return cuboid_distance_(position, cub, minimal_distance, maximal_distance);
}


::wxWindow* tree_structure_loader::create_panel(::wxWindow* parent) {
	tree_structure_panel* panel = new tree_structure_panel(parent);
	panel->set_loader(*this);
	return panel;
}


}
