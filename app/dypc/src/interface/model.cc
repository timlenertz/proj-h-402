#include "model.h"
#include "dypc.h"
#include "../model/model.h"
#include "../model/ply_model.h"
#include "../model/concentric_spheres_model.h"
#include "../model/torus_model.h"
#include "../downsampling.h"
#include "../util.h"
#include <iostream>
#include <sstream>
#include <cstring>

dypc_model dypc_create_ply_model(const char* filename, float scale) {
	DYPC_INTERFACE_BEGIN;
	dypc::ply_model* mod = new dypc::ply_model(filename, scale);
	DYPC_INTERFACE_END_RETURN((dypc_model)mod, nullptr);
}

dypc_model dypc_create_concentric_spheres_model(dypc_size count, float inner, float outer, unsigned steps) {
	DYPC_INTERFACE_BEGIN;
	dypc::concentric_spheres_model* mod = new dypc::concentric_spheres_model(count, inner, outer, steps);
	DYPC_INTERFACE_END_RETURN((dypc_model)mod, nullptr);
}

dypc_model dypc_create_torus_model(dypc_size count, float r0, float r1) {
	DYPC_INTERFACE_BEGIN;
	dypc::torus_model* mod = new dypc::torus_model(count, r0, r1);
	std::cout << mod->number_of_points() << std::endl;
	DYPC_INTERFACE_END_RETURN((dypc_model)mod, nullptr);
}

void dypc_delete_model(dypc_model m) {
	DYPC_INTERFACE_BEGIN;
	delete (dypc::model*)(m);
	DYPC_INTERFACE_END;
}

void dypc_model_get_bounds(dypc_model m, float* x, float* y, float* z) {
	DYPC_INTERFACE_BEGIN;
	dypc::model* mod = (dypc::model*)(m);
	x[0] = mod->x_minimum(); x[1] = mod->x_maximum();
	y[0] = mod->y_minimum(); y[1] = mod->y_maximum();
	z[0] = mod->z_minimum(); z[1] = mod->z_maximum();
	DYPC_INTERFACE_END;
}

dypc_size dypc_model_number_of_points(dypc_model m) {
	DYPC_INTERFACE_BEGIN;
	dypc::model* mod = (dypc::model*)(m);
	DYPC_INTERFACE_END_RETURN(mod->number_of_points(), 0);
}

size_t dypc_uniform_downsampling_side_length_statistics(char* output, size_t output_max, dypc_model m, float max_side, float step) {
	DYPC_INTERFACE_BEGIN;
	dypc::model* mod = (dypc::model*)(m);
	std::stringstream sstr;
	std::vector<dypc::point> points;
	for(const dypc::point& pt : *mod) points.push_back(pt);
	dypc::uniform_downsampling_side_length_statistics(sstr, points.begin(), points.end(), max_side, step);
	size_t sz = sstr.str().size();
	if(sz > output_max) sz = output_max;
	std::memcpy(output, sstr.str().c_str(), sz);
	DYPC_INTERFACE_END_RETURN(sz, 0);
}

void dypc_uniform_downsampling_side_length_statistics_print(dypc_model m, float max_side, float step) {
	DYPC_INTERFACE_BEGIN;
	dypc::model* mod = (dypc::model*)(m);
	std::vector<dypc::point> points;
	for(const dypc::point& pt : *mod) points.push_back(pt);
	dypc::uniform_downsampling_side_length_statistics(std::cout, points.begin(), points.end(), max_side, step);
	DYPC_INTERFACE_END;
}
