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
	unsigned dimension = depth % 3;
	float c = cub.center()[dimension];
	if(idx) {
		glm::vec3 origin = cub.origin;
		origin[dimension] = c;
		return make_cuboid(origin, cub.extremity);
	} else {
		glm::vec3 extremity = cub.extremity;
		extremity[dimension] = c;
		return make_cuboid(cub.origin, extremity);
	}
}

}
