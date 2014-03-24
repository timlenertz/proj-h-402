#include <stdio.h>
#include <dypc/dypc.h>

int main(int argc, const char* argv[]) {
	if(argc <= 2) { printf("Usage: ply2hdf input.ply output.hdf\n"); return 1; }
	
	const char* ply = argv[1];
	const char* hdf = argv[2];
	
	printf("Opening PLY file...\n");
	dypc_model model = dypc_create_ply_model(ply, 1.0);
	if(! model) { printf("Could not load model."); return 1; }
	
	printf("Writing tree structure into HDF...\n");
	dypc_write_tree_structure_to_file(
		hdf,
		model,
		dypc_octree_tree_structure_type,
		16,
		1000,
		1.4,
		dypc_random_downsampling_mode,
		0
	);
	
	dypc_delete_model(model);
	
	printf("Done.\n");
	
	return 0;
}
