#ifndef DYPC_KDTREE_STRUCTURE_H_
#define DYPC_KDTREE_STRUCTURE_H_

#include "../tree_structure.h"
#include "kdtree_structure_splitter.h"
#include <string>

namespace dypc {

template<std::size_t Levels>
class kdtree_structure : public tree_structure<kdtree_structure_splitter, Levels> {
public:
	using tree_structure<kdtree_structure_splitter, Levels>::tree_structure;
	
	static std::string structure_name() {
		if(Levels == 1) return "KdTree Structure";
		else return "KdTree " + std::to_string(Levels) + "-Mipmap Structure";
	}
};

}

#endif

