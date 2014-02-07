#ifndef DYPC_OCTREE_STRUCTURE_H_
#define DYPC_OCTREE_STRUCTURE_H_

#include "../tree_structure.h"
#include "octree_structure_splitter.h"
#include <string>

namespace dypc {

template<std::size_t Levels>
class octree_structure : public tree_structure<octree_structure_splitter, Levels> {
public:
	using tree_structure<octree_structure_splitter, Levels>::tree_structure;
	
	static std::string structure_name() {
		if(Levels == 1) return "Octree Structure";
		else return "Octree " + std::to_string(Levels) + "-Mipmap Structure";
	}
};

}

#endif

