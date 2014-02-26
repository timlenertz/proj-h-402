#include "model.h"
#include "../src/model/model.h"
#include "../src/model/ply_model.h"
#include "../src/model/concentric_spheres_model.h"
#include "../src/model/torus_model.h"
#include "../src/model/random_model.h"
#include "../src/downsampling.h"
#include <iostream>
#include <sstream>
#include <cstring>

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

void dypc_model_get_bounds(dypc_model m, float* x, float* y, float* z) {
	dypc::model* mod = (dypc::model*)(m);
	x[0] = mod->x_minimum();
	x[1] = mod->x_maximum();
	y[0] = mod->y_minimum();
	y[1] = mod->y_maximum();
	z[0] = mod->z_minimum();
	z[1] = mod->z_maximum();
}

size_t dypc_uniform_downsampling_side_length_statistics(char* output, size_t output_max, dypc_model m, float max_side, float step) {
	dypc::model* mod = (dypc::model*)(m);
	std::stringstream sstr;
	std::vector<dypc::point> points;
	for(const dypc::point& pt : *mod) points.push_back(pt);
	dypc::uniform_downsampling_side_length_statistics(sstr, points.begin(), points.end(), max_side, step);
	size_t sz = sstr.str().size();
	if(sz > output_max) sz = output_max;
	std::memcpy(output, sstr.str().c_str(), sz);
	return sz;
}

void dypc_uniform_downsampling_side_length_statistics_print(dypc_model m, float max_side, float step) {
	dypc::model* mod = (dypc::model*)(m);
	std::vector<dypc::point> points;
	for(const dypc::point& pt : *mod) points.push_back(pt);
	dypc::uniform_downsampling_side_length_statistics(std::cout, points.begin(), points.end(), max_side, step);

}
