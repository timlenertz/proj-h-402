#include <dypc/dypc.h>

int main() {
	dypc_model mod = dypc_create_concentric_spheres_model(
		20000, 10.0, 20.0, 3
	);
	dypc_write_tree_structure_to_file(
		"spheres.hdf",
		mod,
		dypc_octree_tree_structure_type,
		8,
		1000,
		10000,
		2.0,
		dypc_random_downsampling_mode,
		5000,
		2
	);
	return 0;
}

