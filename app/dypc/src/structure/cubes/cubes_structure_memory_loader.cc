#include "cubes_structure_memory_loader.h"
#include <vector>

namespace dypc {

std::size_t cubes_structure_memory_loader::extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
	std::size_t frustum_cubes = 0, downsampled_cubes = 0, secondary_pass_cubes = 0;
	std::size_t remaining = capacity;
	std::size_t total = 0;

	std::vector<const cubes_structure::cube*> secondary_pass;

	point_buffer_t buf = points;
	for(const auto& p : structure_.cubes()) {
		const cubes_structure::cube& c = p.second;
		cuboid cube = cube_from_index_(p.first, structure_.get_side_length());
		
		if(frustum_culling_ && !req.view_frustum.contains_cuboid(cube)) continue;
		++frustum_cubes;

		float min_weight = 0;
		float distance = std::abs(glm::distance(req.position, cube.center()));
		if(distance >= secondary_pass_distance_) {
			secondary_pass.push_back(&c);
			++secondary_pass_cubes;
			continue;
		} else if(distance >= downsampling_distance_) {
			min_weight = 1.0 - std::pow(downsampling_distance_/distance, downsampling_level_);
			++downsampled_cubes;
		}
		
		std::size_t extracted = c.extract_points_with_minimal_weight(buf, remaining, min_weight);
		buf += extracted;
		remaining -= extracted;
		total += extracted;
		
		if(! remaining) break;
	}
	
	float secondary_min_weight = 1.0 - std::pow(downsampling_distance_/secondary_pass_distance_, downsampling_level_);
	for(const cubes_structure::cube* c : secondary_pass) {
		std::size_t extracted = c->extract_points_with_minimal_weight(buf, remaining, secondary_min_weight);
		buf += extracted;
		remaining -= extracted;
		total += extracted;
		
		if(! remaining) break;
	}
		
	return total;

}
	
std::size_t cubes_structure_memory_loader::memory_size_() const {
	return structure_.size();
}

std::size_t cubes_structure_memory_loader::file_size_() const {
	return 0;
}

std::size_t cubes_structure_memory_loader::total_points_() const {
	return structure_.total_number_of_points();
}


}
