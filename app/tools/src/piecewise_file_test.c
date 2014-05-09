#include <dypc/dypc.h>

int main() {
	dypc_model mod = dypc_create_concentric_spheres_model(
		2000, 10.0, 20.0, 3
	);
	dypc_write_tree_structure_to_file(
		"spheres.hdf",
		mod,
		dypc_octree_tree_structure_type,
		4,
		400,
		1500,
		2.0,
		dypc_random_downsampling_mode,
		1000,
		2
	);
	return 0;
}

