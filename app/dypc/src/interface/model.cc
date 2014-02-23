#include "model.h"
#include "../src/model/model.h"
#include "../src/model/ply_model.h"
#include "../src/model/concentric_spheres_model.h"
#include "../src/model/torus_model.h"
#include "../src/model/random_model.h"
#include <iostream>

dypc_model dypc_create_ply_model(const char* filename, float scale) {
	dypc::ply_model* mod = new dypc::ply_model(filename, scale);
	return (dypc_model)mod;
}

dypc_model dypc_create_concentric_spheres_model(dypc_size count, float inner, float outer, unsigned steps) {
	dypc::concentric_spheres_model* mod = new dypc::concentric_spheres_model(count, inner, outer, steps);
	return (dypc_model)mod;
}

dypc_model dypc_create_torus_model(dypc_size count, float r0, float r1) {
	dypc::torus_model* mod = new dypc::torus_model(count, r0, r1);
	std::cout << mod->number_of_points() << std::endl;
	return (dypc_model)mod;
}

dypc_model dypc_create_random_model(dypc_size count, float side) {
	dypc::random_model* mod = new dypc::random_model(count, side);
	return (dypc_model)mod;
}

void dypc_delete_model(dypc_model mod) {
	delete (dypc::model*)(mod);
}
