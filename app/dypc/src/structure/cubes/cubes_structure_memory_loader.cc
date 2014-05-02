#include "cubes_structure_memory_loader.h"
#include "../../downsampling.h"
#include <vector>

namespace dypc {

std::size_t cubes_structure_memory_loader::compute_downsampled_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
	std::size_t remaining = capacity;
	std::size_t total = 0;

	std::vector<const cubes_structure::cube*> secondary_pass;

	point_buffer_t buf = points;
	for(const auto& p : structure_.cubes()) {		
		const cubes_structure::cube& c = p.second;
		cuboid cube = cube_from_index_(p.first, structure_.get_side_length());
		
		if(frustum_culling_ && !req.view_frustum.contains_cuboid(cube)) continue;

		float distance = std::abs(glm::distance(req.position, cube.center()));
		float min_weight = 1.0 - downsampling_ratio_(distance, capacity, structure_.total_number_of_points());
				
		if(distance >= secondary_pass_distance_) {
			secondary_pass.push_back(&c);
			continue;
		}
		
		std::size_t extracted = c.extract_points_with_minimal_weight(buf, remaining, min_weight);
		buf += extracted;
		remaining -= extracted;
		total += extracted;
			
		if(! remaining) break;
	}
	
	float secondary_min_weight = 1.0 - downsampling_ratio_(secondary_pass_distance_, capacity, structure_.total_number_of_points());
	for(const cubes_structure::cube* c : secondary_pass) {
		std::size_t extracted = c->extract_points_with_minimal_weight(buf, remaining, secondary_min_weight);
		buf += extracted;
		remaining -= extracted;
		total += extracted;
		
		if(! remaining) break;
	}
		
	return total;

}
	
std::size_t cubes_structure_memory_loader::memory_size() const {
	return structure_.size();
}

std::size_t cubes_structure_memory_loader::rom_size() const {
	return 0;
}

std::size_t cubes_structure_memory_loader::number_of_points() const {
	return structure_.total_number_of_points();
}


}
