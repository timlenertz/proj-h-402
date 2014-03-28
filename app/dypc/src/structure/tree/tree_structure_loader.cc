#include "tree_structure_loader.h"
#include <cmath>
#include <algorithm>

#include <iostream>

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
	
	if(	! is_leaf && maximal_distance - minimal_distance > additional_split_distance_difference_) return action_split;
	
	float distance = cuboid_distance_(req.position, cub, minimal_distance, maximal_distance, downsampling_node_distance_);
	return choose_downsampling_level(levels, distance, downsampling_setting_);
}


float tree_structure_loader::cuboid_distance_(glm::vec3 position, const cuboid& cub, float min_dist, float max_dist, cuboid_distance_mode type) const {
	switch(type) {
		case cuboid_distance_mode::minimal: return min_dist;
		case cuboid_distance_mode::maximal: return max_dist;
		case cuboid_distance_mode::mean: return (max_dist + min_dist)/2.0;
		case cuboid_distance_mode::center: return glm::distance(position, cub.center());
	};
	return 0.0;
}


float tree_structure_loader::cuboid_distance_(glm::vec3 position, const cuboid& cub, cuboid_distance_mode type) const {
	float minimal_distance = cub.minimal_distance(position);
	float maximal_distance = cub.maximal_distance(position);
	return cuboid_distance_(position, cub, minimal_distance, maximal_distance, type);
}


float tree_structure_loader::cuboid_distance_(glm::vec3 position, const cuboid& cub) const {
	float minimal_distance = cub.minimal_distance(position);
	float maximal_distance = cub.maximal_distance(position);
	return cuboid_distance_(position, cub, minimal_distance, maximal_distance, downsampling_node_distance_);
}


double tree_structure_loader::get_setting(const std::string& setting) const {
	if(setting == "downsampling_setting") return downsampling_setting_;
	else if(setting == "minimal_number_of_points_for_split") return minimal_number_of_points_for_split_;
	else if(setting == "downsampling_node_distance") return (double)downsampling_node_distance_;
	else if(setting == "additional_split_distance_difference") return additional_split_distance_difference_;
	else throw std::invalid_argument("Invalid loader setting");
}

void tree_structure_loader::set_setting(const std::string& setting, double value) {
	if(setting == "downsampling_setting") downsampling_setting_ = value;
	else if(setting == "minimal_number_of_points_for_split") minimal_number_of_points_for_split_ = value;
	else if(setting == "downsampling_node_distance") downsampling_node_distance_ = (cuboid_distance_mode)value;
	else if(setting == "additional_split_distance_difference") additional_split_distance_difference_ = value;
	else throw std::invalid_argument("Invalid loader setting");
}

}
