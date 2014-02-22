#ifndef DYPC_KDTREE_STRUCTURE_H_
#define DYPC_KDTREE_STRUCTURE_H_

#include "../tree_structure_piecewise.h"
#include "kdtree_structure_splitter.h"
#include <string>

namespace dypc {

template<std::size_t Levels>
class kdtree_structure : public tree_structure_piecewise<kdtree_structure_splitter, Levels> {
public:
	using tree_structure_piecewise<kdtree_structure_splitter, Levels>::tree_structure_piecewise;
	
	static std::string structure_name() {
		if(Levels == 1) return "KdTree Structure";
		else return "KdTree " + std::to_string(Levels) + "-Mipmap Structure";
	}
};

extern template class kdtree_structure<1>;
extern template class kdtree_structure<4>;
extern template class kdtree_structure<8>;
extern template class kdtree_structure<16>;


}

#endif

