#ifndef DYPC_ENUMS_H_
#define DYPC_ENUMS_H_

#include "interface/common.h"

namespace dypc {

/**
 * Types of structures.
 */
enum class structure_type {
	cubes = dypc_cubes_structure_type,
	cubes_mipmap = dypc_cubes_mipmap_structure_type,
	octree = dypc_octree_tree_structure_type,
	kdtree = dypc_kdtree_tree_structure_type,
	kdtree_half = dypc_kdtree_half_tree_structure_type
};


/**
 * Types of loaders.
 */
enum class loader_type {
	direct = dypc_direct_loader_type,
	cubes = dypc_cubes_loader_type,
	cubes_mipmap = dypc_cubes_mipmap_loader_type,
	tree = dypc_tree_loader_type
};


enum class tree_structure_loader_type {
	simple = dypc_simple_tree_structure_loader_type,
	ordered = dypc_ordered_tree_structure_loader_type,
	sorted = dypc_sorted_tree_structure_loader_type
};

enum class downsampling_mode {
	random = dypc_random_downsampling_mode,
	uniform = dypc_uniform_downsampling_mode
};

enum class cuboid_distance_mode {
	minimal = dypc_minimal_cuboid_distance_mode,
	maximal = dypc_maximal_cuboid_distance_mode,
	mean = dypc_mean_cuboid_distance_mode,
	center = dypc_center_cuboid_distance_mode
};

}

#endif
