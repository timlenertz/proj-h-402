#ifndef DYPC_OCTREE_STRUCTURE_H_
#define DYPC_OCTREE_STRUCTURE_H_

#include "../tree_structure_piecewise.h"
#include "octree_structure_splitter.h"
#include <string>

namespace dypc {

/**
 * Octree structure.
 * Each node cubic and will be subdivided into 8 child cubes
 */
template<std::size_t Levels>
class octree_structure : public tree_structure_piecewise<octree_structure_splitter, Levels> {
public:
	using tree_structure_piecewise<octree_structure_splitter, Levels>::tree_structure_piecewise;
	
	static std::string structure_name() {
		if(Levels == 1) return "Octree Structure";
		else return "Octree " + std::to_string(Levels) + "-Mipmap Structure";
	}
};

extern template class octree_structure<1>; ///< Octree structure with no downsampling.
extern template class octree_structure<4>; ///< Octree structure with 4 mipmap levels.
extern template class octree_structure<8>; ///< Octree structure with 8 mipmap levels.
extern template class octree_structure<16>; ///< Octree structure with 16 mipmap levels.

}

#endif

