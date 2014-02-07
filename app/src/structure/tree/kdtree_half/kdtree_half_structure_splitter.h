#ifndef DYPC_KDTREE_HALF_STRUCTURE_SPLITTER_H_
#define DYPC_KDTREE_HALF_STRUCTURE_SPLITTER_H_

#include "../tree_structure_splitter.h"

namespace dypc {

class kdtree_half_structure_splitter : public tree_structure_splitter {
public:	
	static constexpr std::size_t number_of_node_children = 2;
	
	static std::ptrdiff_t node_child_for_point(const point& pt, const cuboid& cub, const node_points_information& info, unsigned depth);
	static cuboid node_child_cuboid(const std::ptrdiff_t i, const cuboid& cub, const node_points_information& info, unsigned depth);
};


}

#endif
