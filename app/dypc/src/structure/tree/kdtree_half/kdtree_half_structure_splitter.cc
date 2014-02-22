#include "kdtree_half_structure_splitter.h"
#include "../../../point.h"
#include <algorithm>

namespace dypc {

std::ptrdiff_t kdtree_half_structure_splitter::node_child_for_point(const point& pt, const cuboid& cub, const node_points_information& info, unsigned depth) {
	unsigned dimension = depth % 3;
	auto c = cub.center();
	if(dimension == 0) return (pt.x < c[0] ? 0 : 1);
	else if(dimension == 1) return (pt.y < c[1] ? 0 : 1);
	else if(dimension == 2) return (pt.z < c[2] ? 0 : 1);
	else return 0;
}


cuboid kdtree_half_structure_splitter::node_child_cuboid(const std::ptrdiff_t idx, const cuboid& cub, const node_points_information& info, unsigned depth) {
	glm::vec3 origin = cub.origin();
	glm::vec3 side_lengths = cub.side_lengths();

	unsigned dimension = depth % 3;
	auto c = cub.center();
	if(idx) { origin[dimension] = c[dimension]; side_lengths[dimension] -= c[dimension] - origin[dimension]; }
	else { side_lengths[dimension] = c[dimension] - origin[dimension]; }

	return cuboid(origin, side_lengths);
}

}
