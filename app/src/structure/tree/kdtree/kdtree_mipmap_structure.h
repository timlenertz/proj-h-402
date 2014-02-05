#ifndef DYPC_KDTREE_MIPMAP_STRUCTURE_H_
#define DYPC_KDTREE_MIPMAP_STRUCTURE_H_

#include "../tree_mipmap_structure.h"
#include "kdtree_structure_splitter.h"

namespace dypc {

class kdtree_mipmap_structure : public tree_mipmap_structure<kdtree_structure_splitter> {
public:
	using tree_mipmap_structure<kdtree_structure_splitter>::tree_mipmap_structure;
	static std::string structure_name() { return "KdTree Mipmap Structure"; }
};

}

#endif

