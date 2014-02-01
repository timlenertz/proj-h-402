#ifndef DYPC_KDTREE_MIPMAP_STRUCTURE_H_
#define DYPC_KDTREE_MIPMAP_STRUCTURE_H_

#include "../tree_mipmap_structure.h"
#include "../tree_structure_memory_loader.h"
#include "../tree_structure_hdf_loader.h"
#include "../tree_structure_hdf_separate_loader.h"
#include "kdtree_structure_splitter.h"

namespace dypc {

using kdtree_mipmap_structure = tree_mipmap_structure<kdtree_structure_splitter>;

class kdtree_mipmap_structure_memory_loader : public tree_structure_memory_loader<kdtree_mipmap_structure> {
public:
	using tree_structure_memory_loader<kdtree_mipmap_structure>::tree_structure_memory_loader;
	std::string loader_name() const override { return "KdTree Mipmap Structure Memory Loader"; }
};

class kdtree_mipmap_structure_hdf_loader : public tree_structure_hdf_loader<kdtree_mipmap_structure> {
public:
	using tree_structure_hdf_loader<kdtree_mipmap_structure>::tree_structure_hdf_loader;
	std::string loader_name() const override { return "KdTree Mipmap Structure HDF Loader (Striped)"; }
};

class kdtree_mipmap_structure_hdf_separate_loader : public tree_structure_hdf_separate_loader<kdtree_mipmap_structure> {
public:
	using tree_structure_hdf_separate_loader<kdtree_mipmap_structure>::tree_structure_hdf_separate_loader;
	std::string loader_name() const override { return "KdTree Mipmap Structure HDF Loader (Separate)"; }
};


}

#endif
