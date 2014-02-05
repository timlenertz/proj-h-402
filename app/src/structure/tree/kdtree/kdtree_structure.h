#ifndef DYPC_KDTREE_STRUCTURE_H_
#define DYPC_KDTREE_STRUCTURE_H_

#include "../tree_structure.h"
#include "kdtree_structure_splitter.h"

namespace dypc {

class kdtree_structure : public tree_structure<kdtree_structure_splitter> {
public:
	using tree_structure<kdtree_structure_splitter>::tree_structure;
	static std::string structure_name() { return "KdTree Structure"; }
};

}

#endif

