#ifndef DYPC_TREE_STRUCTURE_LOADER_H_
#define DYPC_TREE_STRUCTURE_LOADER_H_

#include "tree_structure.h"
#include "../structure_loader.h"

namespace dypc {
	
class tree_structure_loader : public structure_loader {
public:
	enum downsampling_node_distance_t { minimal_node_distance = 0, maximal_node_distance, mean_node_distance, center_node_distance };
	
private:
	float downsampling_start_distance_ = 20;
	float downsampling_step_distance_ = 10;
	std::size_t minimal_number_of_points_for_split_ = 1000;
	downsampling_node_distance_t downsampling_node_distance_ = center_node_distance;
	float additional_split_distance_difference_ = 25;

	float cuboid_distance_(glm::vec3 position, const cuboid& cub, float min_dist, float max_dist, downsampling_node_distance_t type) const;

protected:
	static constexpr std::ptrdiff_t action_skip = -2;
	static constexpr std::ptrdiff_t action_split = -1;
	
	std::ptrdiff_t action_for_node_(const cuboid&, std::size_t number_of_points, bool is_leaf, const loader::request_t&, std::size_t levels = 1) const;
	float cuboid_distance_(glm::vec3 position, const cuboid& cub, downsampling_node_distance_t type) const;
	float cuboid_distance_(glm::vec3 position, const cuboid& cub) const;
	
	//adapt_result_t adapt_settings_(std::size_t last_extracted, std::size_t capacity) override;
	
public:
	void set_downsampling_start_distance(float d) { downsampling_start_distance_ = d; }
	void set_downsampling_step_distance(float d) { downsampling_step_distance_ = d; }
	void set_minimal_number_of_points_for_split(std::size_t n) { minimal_number_of_points_for_split_ = n; }
	void set_downsampling_node_distance(downsampling_node_distance_t d) { downsampling_node_distance_ = d; }
	void set_additional_split_distance_difference(float d) { additional_split_distance_difference_ = d; }
	
	//::wxWindow* create_panel(::wxWindow* parent) override;
};
	
}

#endif
