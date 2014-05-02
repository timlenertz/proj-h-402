#ifndef DYPC_ENUMS_H_
#define DYPC_ENUMS_H_

#include "interface/common.h"

namespace dypc {

/**
 * Types of structures.
 */
enum class structure_type {
	cubes = dypc_cubes_structure_type, ///< Cubes structure. @see cubes_structure
	cubes_mipmap = dypc_cubes_mipmap_structure_type, ///< Cubes mipmap structure. @see cubes_mipmap_structure
	octree = dypc_octree_tree_structure_type, ///< Octree structure.  @see octree_structure
	kdtree = dypc_kdtree_tree_structure_type, ///< KdTree structure with median split plane.  @see kdtree_structure
	kdtree_half = dypc_kdtree_half_tree_structure_type ///< KdTree structure width half split plane. @see kdtree_half_structure
};


/**
 * Types of loaders.
 */
enum class loader_type {
	direct = dypc_direct_loader_type, ///< Direct model loader. @see direct_model_loader
	cubes = dypc_cubes_loader_type, ///< Cubes structure loader. @see cubes_structure_loader
	cubes_mipmap = dypc_cubes_mipmap_loader_type, ///< Cubes mipmap structure loader. @see cubes_mipmap_structure_loader
	tree = dypc_tree_loader_type ///< Tree structure loader. @see tree_structure_loader
};


/**
 * Types of tree structure loaders.
 */
enum class tree_structure_loader_type {
	simple = dypc_simple_tree_structure_loader_type, ///< Simple tree structure loader. @see tree_structure_simple_loader
	ordered = dypc_ordered_tree_structure_loader_type ///< Ordered tree structure loader. Loads cuboids closer to camera first. @see tree_structure_ordered_loader
};

/**
 * Downsampling modes.
 */
enum class downsampling_mode {
	random = dypc_random_downsampling_mode, ///< Random downsampling. @see random_downsampling
	uniform = dypc_uniform_downsampling_mode ///< Uniform downsampling. @see uniform_downsampling
};


/**
 * Ways to define cuboid-to-point distance.
 */
enum class cuboid_distance_mode {
	minimal = dypc_minimal_cuboid_distance_mode, ///< Minimal distance from point to cuboid.
	maximal = dypc_maximal_cuboid_distance_mode, ///< Maximal distance from point to cuboid.
	mean = dypc_mean_cuboid_distance_mode, ///< Mean of minimal and maximal distance.
	center = dypc_center_cuboid_distance_mode ///< Distance to center of cuboid.
};

}

#endif
