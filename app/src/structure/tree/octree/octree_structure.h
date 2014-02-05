#ifndef DYPC_OCTREE_STRUCTURE_H_
#define DYPC_OCTREE_STRUCTURE_H_

#include "../tree_structure.h"
#include "octree_structure_splitter.h"

namespace dypc {

class octree_structure : public tree_structure<octree_structure_splitter> {
public:
	using tree_structure<octree_structure_splitter>::tree_structure;
	static std::string structure_name() { return "Octree Structure"; }
};

}

#endif

