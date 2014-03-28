#ifndef DYPC_TREE_STRUCTURE_LOADER_H_
#define DYPC_TREE_STRUCTURE_LOADER_H_

#include "tree_structure.h"
#include "tree_structure_source.h"
#include "../structure_loader.h"
#include "../../enums.h"
#include <memory>
#include <utility>

namespace dypc {
	
class tree_structure_loader : public structure_loader {	
private:
	float downsampling_setting_ = 20;
	std::size_t minimal_number_of_points_for_split_ = 1000;
	cuboid_distance_mode downsampling_node_distance_ = cuboid_distance_mode::center;
	float additional_split_distance_difference_ = 25;

	float cuboid_distance_(glm::vec3 position, const cuboid& cub, float min_dist, float max_dist, cuboid_distance_mode type) const;

protected:
	static constexpr std::ptrdiff_t action_skip = -2;
	static constexpr std::ptrdiff_t action_split = -1;
	
	std::unique_ptr<const tree_structure_source> source_;
	
	std::ptrdiff_t action_for_node_(const cuboid&, std::size_t number_of_points, bool is_leaf, const loader::request_t&, std::size_t levels = 1) const;
	float cuboid_distance_(glm::vec3 position, const cuboid& cub, cuboid_distance_mode type) const;
	float cuboid_distance_(glm::vec3 position, const cuboid& cub) const;
		
public:
	void set_downsampling_setting(float d) { downsampling_setting_ = d; }
	void set_minimal_number_of_points_for_split(std::size_t n) { minimal_number_of_points_for_split_ = n; }
	void set_downsampling_node_distance(cuboid_distance_mode d) { downsampling_node_distance_ = d; }
	void set_additional_split_distance_difference(float d) { additional_split_distance_difference_ = d; }
	
	double get_setting(const std::string&) const override;
	void set_setting(const std::string&, double) override;
	
	void take_source(const tree_structure_source* src) { source_.reset(src); }
	void delete_source() { source_.release(); }
	
	loader_type get_loader_type() const override { return loader_type::tree; }
	
	std::size_t memory_size() const override { return source_->memory_size(); }
	std::size_t rom_size() const override { return source_->rom_size(); }
	std::size_t number_of_points() const override { return source_->number_of_points(); }
};
	
}

#endif
