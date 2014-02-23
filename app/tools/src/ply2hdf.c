#include <stdio.h>
#include <dypc/dypc.h>

int main(int argc, const char* argv[]) {
	if(argc <= 2) { printf("Usage: ply2hdf input.ply output.hdf"); return 1; }
	
	const char* ply = argv[1];
	const char* hdf = argv[2];
	
	dypc_model model = dypc_create_ply_model(ply, 1.0);
	if(! model) { printf("Could not load model."); return 1; }
	
	dypc_write_tree_structure_to_file(
		hdf,
		model,
		dypc_octree_tree_structure_type,
		16.0,
		1000,
		1.4,
		dypc_random_downsampling_mode,
		0
	);
	
	dypc_delete_model(model);
	
	return 0;
}
