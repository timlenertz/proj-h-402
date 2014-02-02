#ifndef DYPC_KDTREE_STRUCTURE_SPLITTER_H_
#define DYPC_KDTREE_STRUCTURE_SPLITTER_H_

#include "../tree_structure_splitter.h"

namespace dypc {

class kdtree_structure_splitter : public tree_structure_splitter {
public:	
	static constexpr std::size_t number_of_node_children = 2;
	
	struct node_points_information {
		float split_plane;
	};
	
	static std::ptrdiff_t node_child_for_point(const point& pt, const cuboid& cub, const node_points_information& info, unsigned depth);
	static cuboid node_child_cuboid(const std::ptrdiff_t i, const cuboid& cub, const node_points_information& info, unsigned depth);
	static node_points_information compute_node_points_information(const std::vector<point>& points, const cuboid& cub, unsigned depth);
};


}

#endif
