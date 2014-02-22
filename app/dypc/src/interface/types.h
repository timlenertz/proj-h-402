#ifndef DYPC_INTERFACE_TYPES_H_
#define DYPC_INTERFACE_TYPES_H_

#include <stddef.h>

typedef enum {
	dypc_octree_tree_structure_type = 0,
	dypc_kdtree_tree_structure_type,
	dypc_kdtree_half_tree_structure_type
} dypc_tree_structure_type;


typedef enum {
	dypc_simple_tree_structure_loader_type = 0,
	dypc_ordered_tree_structure_loader_type,
	dypc_occluding_tree_structure_loader_type
} dypc_tree_structure_loader_type;


typedef enum {
	dypc_random_downsampling_mode = 0,
	dypc_uniform_downsampling_mode
} dypc_downsampling_mode;


typedef float dypc_vector3[3];
typedef float dypc_quaternion[4];
typedef float dypc_matrix4[16];

typedef struct {
	float x, y, z;
	unsigned char r, g, b;
} dypc_point;

typedef dypc_point* dypc_points_buffer;

typedef size_t dypc_size;
typedef int dypc_bool;
typedef unsigned dypc_milliseconds;

#endif
