#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <dypc/dypc.h>

int main(int argc, const char* argv[]) {
	if(argc <= 1) { printf("Usage: ply2hdf input.ply"); return 1; }
	
	const char* ply = argv[1];
	
	dypc_model model = dypc_create_ply_model(ply, 1.0);
	if(! model) { printf("Could not load model."); return 1; }


	dypc_uniform_downsampling_side_length_statistics_print(model, 0.5, 0.003);
	dypc_delete_model(model);
	
	return 0;
}
