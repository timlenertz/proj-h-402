#include "kdtree_structure_splitter.h"
#include "../../../point.h"

namespace dypc {

std::ptrdiff_t kdtree_structure_splitter::node_child_for_point(const point& pt, const cuboid& cub, const node_points_information& info, unsigned depth) {
	unsigned dimension = depth % 3;
	if(dimension == 0) return (pt.x < info.split_plane ? 0 : 1);
	else if(dimension == 1) return (pt.y < info.split_plane ? 0 : 1);
	else if(dimension == 2) return (pt.z < info.split_plane ? 0 : 1);
	else return 0;
}


cuboid kdtree_structure_splitter::node_child_cuboid(const std::ptrdiff_t idx, const cuboid& cub, const node_points_information& info, unsigned depth) {
	glm::vec3 origin = cub.origin();
	glm::vec3 side_lengths = cub.side_lengths();

	unsigned dimension = depth % 3;
	if(idx) { origin[dimension] = info.split_plane; side_lengths[dimension] -= info.split_plane - origin[dimension]; }
	else { side_lengths[dimension] = info.split_plane - origin[dimension]; }

	return cuboid(origin, side_lengths);
}


}
