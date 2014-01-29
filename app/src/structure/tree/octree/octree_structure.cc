#include "octree_structure.h"
#include <algorithm>

namespace dypc {

cuboid octree_structure_splitter::root_coboid(const model& mod) {
	glm::vec3 origin(mod.x_minimum(), mod.y_minimum(), mod.z_minimum());
	float side_length = std::max({ mod.x_range(), mod.y_range(), mod.z_range() });
	origin[0] -= (side_length - mod.x_range())/2;
	origin[1] -= (side_length - mod.y_range())/2;
	origin[2] -= (side_length - mod.z_range())/2;
	return cuboid(origin, side_length);
}

std::ptrdiff_t octree_structure_splitter::node_child_for_point(const octree_structure::node& nd, const point& pt, const cuboid& cub) {
	auto c = cub.center();
	std::ptrdiff_t idx = 0;
	if(pt.x > c.x) idx += 1;
	if(pt.y > c.y) idx += 2;
	if(pt.z > c.z) idx += 4;
	return idx;
}


cuboid octree_structure_splitter::node_child_cuboid(const octree_structure::node& nd, const std::ptrdiff_t idx, const cuboid& cub) {
	auto s = cub.side_length_x() / 2;
	auto o = cub.origin();
	if(idx % 2) o.x += s;
	if(idx % 4 > 1) o.y += s;
	if(idx >= 4) o.z += s;
	return cuboid(o, s);
}

}
