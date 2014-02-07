#ifndef DYPC_KDTREE_HALF_STRUCTURE_H_
#define DYPC_KDTREE_HALF_STRUCTURE_H_

#include "../tree_structure.h"
#include "kdtree_half_structure_splitter.h"
#include <string>

namespace dypc {

template<std::size_t Levels>
class kdtree_half_structure : public tree_structure<kdtree_half_structure_splitter, Levels> {
public:
	using tree_structure<kdtree_half_structure_splitter, Levels>::tree_structure;
	
	static std::string structure_name() {
		if(Levels == 1) return "KdTree Half Structure";
		else return "KdTree Half " + std::to_string(Levels) + "-Mipmap Structure";
	}
};

}

#endif

