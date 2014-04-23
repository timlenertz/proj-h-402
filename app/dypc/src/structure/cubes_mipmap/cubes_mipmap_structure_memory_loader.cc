#include "cubes_mipmap_structure_memory_loader.h"
#include <vector>

namespace dypc {

std::size_t cubes_mipmap_structure_memory_loader::extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
	std::size_t remaining = capacity;
	std::size_t total = 0;

	point_buffer_t buf = points;
	for(const auto& p : structure_.cubes()) {
		const cubes_mipmap_structure::cube& c = p.second;
		cuboid cube = cube_from_index_(p.first, structure_.get_side_length());
		
		if(frustum_culling_ && !req.view_frustum.contains_cuboid(cube)) continue;

		float distance = std::abs(glm::distance(req.position, cube.center()));
		std::size_t lvl = choose_downsampling_level(structure_.get_downsampling_levels(), distance, downsampling_setting_);

		std::size_t extracted = c.extract_points_at_level(buf, remaining, lvl);
		buf += extracted;
		remaining -= extracted;
		total += extracted;
		
		if(! remaining) break;
	}
		
	return total;

}
	
std::size_t cubes_mipmap_structure_memory_loader::memory_size() const {
	return structure_.size();
}

std::size_t cubes_mipmap_structure_memory_loader::rom_size() const {
	return 0;
}

std::size_t cubes_mipmap_structure_memory_loader::number_of_points() const {
	return structure_.total_number_of_points();
}


}
