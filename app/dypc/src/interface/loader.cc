#include "loader.h"
#include "../src/point.h"
#include "../src/util.h"
#include "../src/model/model.h"
#include "../src/loader/loader.h"
#include "../src/loader/direct_model_loader.h"
#include "../src/structure/structure_loader_factory.h"
#include "../src/structure/cubes/cubes_structure_memory_loader.h"
#include "../src/structure/cubes/cubes_structure_hdf_loader.h"
#include "../src/structure/cubes/cubes_structure_sqlite_loader.h"
#include "../src/structure/cubes_mipmap/cubes_mipmap_structure_memory_loader.h"
#include "../src/structure/cubes_mipmap/cubes_mipmap_structure_hdf_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static dypc::structure_type_t convert_structure_type(dypc_tree_structure_type typ) {
	switch(typ) {
		case dypc_octree_tree_structure_type: return dypc::octree_structure_type;
		case dypc_kdtree_tree_structure_type: return dypc::kdtree_structure_type;
		case dypc_kdtree_half_tree_structure_type: return dypc::kdtree_half_structure_type;
	}
}

static dypc::downsampling_mode_t convert_downsampling_mode(dypc_downsampling_mode dmode) {
	switch(dmode) {
		case dypc_random_downsampling_mode: return dypc::random_downsampling_mode;
		case dypc_uniform_downsampling_mode: return dypc::uniform_downsampling_mode;
	}
}

static dypc::tree_structure_loader_type_t convert_tree_structure_loader_type(dypc_tree_structure_loader_type ltype) {
	switch(ltype) {
		case dypc_simple_tree_structure_loader_type: return dypc::simple_tree_structure_loader_type;
		case dypc_ordered_tree_structure_loader_type: return dypc::ordered_tree_structure_loader_type;
		case dypc_occluding_tree_structure_loader_type: return dypc::occluding_tree_structure_loader_type;
	}
}

static dypc::loader::request_t convert_loader_request(const dypc_loader_request& req) {
	return dypc::loader::request_t(
		glm::make_vec3(req.position),
		glm::make_vec3(req.velocity),
		glm::quat(req.orientation[0], req.orientation[1], req.orientation[2], req.orientation[3]),
		glm::make_mat4(req.view_projection_matrix)
	);
}

dypc_loader dypc_create_direct_model_loader(dypc_model m) {
	dypc::model* mod = (dypc::model*)m;
	dypc::direct_model_loader* ld = new dypc::direct_model_loader(mod);
	return (dypc_loader)ld;
}

dypc_loader dypc_create_file_structure_loader(const char* filename, dypc_tree_structure_loader_type ltype) {
	dypc::loader* ld = dypc::create_structure_file_loader(filename, convert_tree_structure_loader_type(ltype));
	return (dypc_loader)ld;
}

void dypc_delete_loader(dypc_loader ld) {
	delete (dypc::loader*)ld;
}


dypc_loader dypc_create_cubes_structure_loader(dypc_model m, float side) {
	dypc::model* mod = (dypc::model*)m;
	dypc::cubes_structure_memory_loader* ld = new dypc::cubes_structure_memory_loader(side, *mod);
	return (dypc_loader)ld;
}


dypc_loader dypc_create_mipmap_cubes_structure_loader(dypc_model m, float side, unsigned levels, float mmfac, dypc_downsampling_mode dmode) {
	dypc::model* mod = (dypc::model*)m;
	dypc::cubes_mipmap_structure_memory_loader* ld = new dypc::cubes_mipmap_structure_memory_loader(side, levels, mmfac, convert_downsampling_mode(dmode), *mod);
	return (dypc_loader)ld;
}


dypc_loader dypc_create_tree_structure_loader(dypc_model m, dypc_tree_structure_type str, unsigned levels, dypc_size leaf_cap, float mmfac, dypc_downsampling_mode dmode, dypc_size dmax, dypc_tree_structure_loader_type ltype) {
	dypc::model* mod = (dypc::model*)m;
	dypc::tree_structure_loader* ld = dypc::create_tree_structure_memory_loader(
		convert_structure_type(str),
		levels, 
		leaf_cap,
		mmfac,
		convert_downsampling_mode(dmode),
		dmax,
		*mod,
		convert_tree_structure_loader_type(ltype)
	);
	return (dypc_loader)ld;
}

void dypc_write_cubes_structure_to_file(const char* filename, dypc_model m, float side) {
	dypc::model* mod = (dypc::model*)m;
	dypc::cubes_structure s(side, *mod);
	auto ext = dypc::file_path_extension(filename);
	if(ext == "hdf") dypc::cubes_structure_hdf_loader::write(filename, s);
	else if(ext == "sqlite") dypc::cubes_structure_sqlite_loader::write(filename, s);
}

void dypc_write_mipmap_cubes_structure_to_file(const char* filename, dypc_model m, float side, unsigned levels, float mmfac, dypc_downsampling_mode dmode) {
	dypc::model* mod = (dypc::model*)m;
	dypc::cubes_mipmap_structure s(side, levels, mmfac, convert_downsampling_mode(dmode), *mod);
	auto ext = dypc::file_path_extension(filename);
	if(ext == "hdf") dypc::cubes_mipmap_structure_hdf_loader::write(filename, s);
}

void dypc_write_tree_structure_to_file(const char* filename, dypc_model m, dypc_tree_structure_type str, unsigned levels, dypc_size leaf_cap, float mmfac, dypc_downsampling_mode dmode, dypc_size dmax) {
	dypc::model* mod = (dypc::model*)m;
	dypc::write_tree_structure_file(
		filename,
		convert_structure_type(str),
		levels,
		leaf_cap,
		mmfac,
		convert_downsampling_mode(dmode),
		dmax,
		*mod
	);
}


const char* dypc_loader_name(dypc_loader l) {
	dypc::loader* ld = (dypc::loader*)l;
	static thread_local std::string buffer = std::string();
	buffer = ld->loader_name();
	return buffer.c_str();
}

void dypc_loader_set_configuration(dypc_loader, const char* key, double value) {
	
}

double dypc_loader_get_configuration(dypc_loader, const char* key) {
	
}


void dypc_loader_compute_points(dypc_loader l, const dypc_loader_request* request, dypc_points_buffer buffer, dypc_size* count_ptr) {
	dypc::loader* ld = (dypc::loader*)l;
	std::size_t capacity = *count_ptr;
	std::size_t count = 0;
	ld->compute_points(convert_loader_request(*request), (dypc::point*)buffer, count, capacity);
	*count_ptr = count;
}

dypc_bool dypc_loader_should_compute_points(dypc_loader l, const dypc_loader_request* request, const dypc_loader_request* previous_request, dypc_milliseconds dtime) {
	dypc::loader* ld = (dypc::loader*)l;
	return ld->should_compute_points(convert_loader_request(*request), convert_loader_request(*previous_request), std::chrono::milliseconds(dtime));
}
