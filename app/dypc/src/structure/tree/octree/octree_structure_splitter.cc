#include "octree_structure_splitter.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include "../../../debug.h"

namespace dypc {

cuboid octree_structure_splitter::adjust_root_cuboid(const cuboid& cub) {
	// Generate larger cube, that encloses the cuboid. The cuboid will be at the center of the cube.
	float side_length = std::max({ cub.side_length_x(), cub.side_length_y(), cub.side_length_z() });
	glm::vec3 origin = cub.origin;
	origin[0] -= (side_length - cub.side_length_x())/2;
	origin[1] -= (side_length - cub.side_length_y())/2;
	origin[2] -= (side_length - cub.side_length_z())/2;
	return cuboid(origin, side_length);
}

std::ptrdiff_t octree_structure_splitter::node_child_for_point(const point& pt, const cuboid& cub, const node_points_information& info, unsigned depth) {
	// Child node index is determined by position of points relative to center of this cube
	assert(cub.is_cube());
	assert(cub.in_range(pt));
	
	glm::vec3 c = cub.center();
	std::ptrdiff_t idx = 0;
	if(pt.x >= c[0]) idx += 1;
	if(pt.y >= c[1]) idx += 2;
	if(pt.z >= c[2]) idx += 4;

	return idx;
}

cuboid octree_structure_splitter::node_child_cuboid(const std::ptrdiff_t idx, const cuboid& cub, const node_points_information& info, unsigned depth) {
	// Generate child cuboid/cube.
	// Important: The cuboids are stored as origin+extremity vectors. This procedure does not change the origin, or does not change the extremity floating point value, and so no errors due to floating point imprecision can occur on the borders of the cube. That is, the cube will always contain the points when node_child_for_point says they do.
	
	assert(cub.is_cube());

	glm::vec3 c = cub.center();

	bool x = (idx % 2);
	bool y = (idx % 4 > 1);
	bool z = (idx >= 4);

	glm::vec3 origin(
		x ? c.x : cub.origin.x,
		y ? c.y : cub.origin.y,
		z ? c.z : cub.origin.z
	);
	
	glm::vec3 extremity(
		x ? cub.extremity.x : c.x,
		y ? cub.extremity.y : c.y,
		z ? cub.extremity.z : c.z
	);

	return make_cuboid(origin, extremity);
}

}
