#ifndef DYPC_OCTREE_STRUCTURE_H_
#define DYPC_OCTREE_STRUCTURE_H_

#include "../tree_structure.h"
#include "../tree_structure_splitter.h"
#include "../tree_structure_memory_loader.h"

namespace dypc {

class octree_structure_splitter;

using octree_structure = tree_structure<octree_structure_splitter>;
using octree_structure_memory_loader = tree_structure_memory_loader<octree_structure>;

class octree_structure_splitter : public tree_structure_splitter {
public:
	static constexpr std::size_t number_of_node_children = 8;
	
	static cuboid root_coboid(const model& mod);
	static std::ptrdiff_t node_child_for_point(const octree_structure::node& nd, const point& pt, const cuboid& cub);
	static cuboid node_child_cuboid(const octree_structure::node& nd, const std::ptrdiff_t i, const cuboid& cub);
};


}

#endif
