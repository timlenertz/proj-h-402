#ifndef DYPC_OCTREE_MIPMAP_STRUCTURE_H_
#define DYPC_OCTREE_MIPMAP_STRUCTURE_H_

#include "../tree_mipmap_structure.h"
#include "octree_structure_splitter.h"

namespace dypc {

class octree_mipmap_structure : public tree_mipmap_structure<octree_structure_splitter> {
public:
	using tree_mipmap_structure<octree_structure_splitter>::tree_mipmap_structure;
	static std::string structure_name() { return "Octree Mipmap Structure"; }
};

}

#endif

