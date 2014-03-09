#include "octree_structure_splitter.h"
#include <algorithm>
#include <cassert>

#include <iostream>

namespace dypc {

cuboid octree_structure_splitter::root_cuboid(model& mod) {
	glm::vec3 origin(mod.x_minimum(), mod.y_minimum(), mod.z_minimum());
	float side_length = std::max({ mod.x_range(), mod.y_range(), mod.z_range() });
	origin[0] -= (side_length - mod.x_range())/2;
	origin[1] -= (side_length - mod.y_range())/2;
	origin[2] -= (side_length - mod.z_range())/2;
	return cuboid(origin, side_length);
}

std::ptrdiff_t octree_structure_splitter::node_child_for_point(const point& pt, const cuboid& cub, const node_points_information& info, unsigned depth) {	
	assert(cub.is_cube());
	glm::vec3 c = cub.center();
	std::ptrdiff_t idx = 0;
	if(pt.x > c[0]) idx += 1;
	if(pt.y > c[1]) idx += 2;
	if(pt.z > c[2]) idx += 4;
	assert(node_child_cuboid(idx, cub, info, depth).in_range(pt));	
	return idx;
}

cuboid octree_structure_splitter::node_child_cuboid(const std::ptrdiff_t idx, const cuboid& cub, const node_points_information& info, unsigned depth) {
	assert(cub.is_cube());
	float s = cub.side_length_x() / 2;
	glm::vec3 o = cub.origin();
	if(idx % 2) o[0] += s;
	if(idx % 4 > 1) o[1] += s;
	if(idx >= 4) o[2] += s;
	return cuboid(o, s);
}

}
