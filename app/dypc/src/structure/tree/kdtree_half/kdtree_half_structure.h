#ifndef DYPC_KDTREE_HALF_STRUCTURE_H_
#define DYPC_KDTREE_HALF_STRUCTURE_H_

#include "../tree_structure_piecewise.h"
#include "kdtree_half_structure_splitter.h"
#include <string>

namespace dypc {

template<std::size_t Levels>
class kdtree_half_structure : public tree_structure_piecewise<kdtree_half_structure_splitter, Levels> {
public:
	using tree_structure_piecewise<kdtree_half_structure_splitter, Levels>::tree_structure_piecewise;
	
	static std::string structure_name() {
		if(Levels == 1) return "KdTree Half Structure";
		else return "KdTree Half " + std::to_string(Levels) + "-Mipmap Structure";
	}
};

extern template class kdtree_half_structure<1>;
extern template class kdtree_half_structure<4>;
extern template class kdtree_half_structure<8>;
extern template class kdtree_half_structure<16>;


}

#endif

