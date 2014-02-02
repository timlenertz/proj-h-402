#ifndef DYPC_OCTREE_STRUCTURE_SPLITTER_H_
#define DYPC_OCTREE_STRUCTURE_SPLITTER_H_

#include "../tree_structure_splitter.h"

namespace dypc {

class octree_structure_splitter : public tree_structure_splitter {
public:	
	static constexpr std::size_t number_of_node_children = 8;
	
	static cuboid root_cuboid(const model& mod);
	static std::ptrdiff_t node_child_for_point(const point& pt, const cuboid& cub, const node_points_information& info, unsigned depth);
	static cuboid node_child_cuboid(const std::ptrdiff_t i, const cuboid& cub, const node_points_information& info, unsigned depth);
	using tree_structure_splitter::compute_node_points_information;
};


}

#endif
