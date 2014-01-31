#ifndef DYPC_OCTREE_STRUCTURE_H_
#define DYPC_OCTREE_STRUCTURE_H_

#include "../tree_structure.h"
#include "../tree_structure_memory_loader.h"
#include "../tree_structure_hdf_loader.h"
#include "../tree_structure_hdf_separate_loader.h"
#include "octree_structure_splitter.h"

namespace dypc {

using octree_structure = tree_structure<octree_structure_splitter>;

class octree_structure_memory_loader : public tree_structure_memory_loader<octree_structure> {
public:
	using tree_structure_memory_loader<octree_structure>::tree_structure_memory_loader;
	std::string loader_name() const override { return "Octree Structure Memory Loader"; }
};

class octree_structure_hdf_loader : public tree_structure_hdf_separate_loader<octree_structure> {
public:
	using tree_structure_hdf_separate_loader<octree_structure>::tree_structure_hdf_separate_loader;
	std::string loader_name() const override { return "Octree Structure HDF Loader"; }
};

}

#endif

