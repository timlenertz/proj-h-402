#include "loader.h"
#include "../enums.h"
#include "../point.h"
#include "../util.h"
#include "../model/model.h"
#include "../loader/loader.h"
#include "../loader/direct_model_loader.h"
#include "../structure/structure_loader_factory.h"
#include "../structure/cubes/cubes_structure_memory_loader.h"
#include "../structure/cubes/cubes_structure_hdf_loader.h"
#include "../structure/cubes/cubes_structure_sqlite_loader.h"
#include "../structure/cubes_mipmap/cubes_mipmap_structure_memory_loader.h"
#include "../structure/cubes_mipmap/cubes_mipmap_structure_hdf_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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
	dypc::loader* ld = dypc::create_structure_file_loader(filename, (dypc::tree_structure_loader_type)(ltype));
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
	dypc::cubes_mipmap_structure_memory_loader* ld = new dypc::cubes_mipmap_structure_memory_loader(side, levels, mmfac, (dypc::downsampling_mode)(dmode), *mod);
	return (dypc_loader)ld;
}


dypc_loader dypc_create_tree_structure_loader(dypc_model m, dypc_structure_type str, unsigned levels, dypc_size leaf_cap, float mmfac, dypc_downsampling_mode dmode, dypc_size dmax, dypc_tree_structure_loader_type ltype) {
	dypc::model* mod = (dypc::model*)m;
	dypc::tree_structure_loader* ld = dypc::create_tree_structure_memory_loader(
		(dypc::structure_type)(str),
		levels, 
		leaf_cap,
		mmfac,
		(dypc::downsampling_mode)(dmode),
		dmax,
		*mod,
		(dypc::tree_structure_loader_type)(ltype)
	);
	return (dypc_loader)ld;
}

dypc_loader_type dypc_loader_loader_type(dypc_loader l) {
	dypc::loader& ld = *(dypc::loader*)l;
	return (dypc_loader_type)ld.get_loader_type();
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
	dypc::cubes_mipmap_structure s(side, levels, mmfac, (dypc::downsampling_mode)(dmode), *mod);
	auto ext = dypc::file_path_extension(filename);
	if(ext == "hdf") dypc::cubes_mipmap_structure_hdf_loader::write(filename, s);
}

void dypc_write_tree_structure_to_file(const char* filename, dypc_model m, dypc_structure_type str, unsigned levels, dypc_size leaf_cap, float mmfac, dypc_downsampling_mode dmode, dypc_size dmax) {
	dypc::model* mod = (dypc::model*)m;
	dypc::write_tree_structure_file(
		filename,
		(dypc::structure_type)(str),
		levels,
		leaf_cap,
		mmfac,
		(dypc::downsampling_mode)(dmode),
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

void dypc_loader_set_setting(dypc_loader l, const char* key, double value) {
	dypc::loader* ld = (dypc::loader*)l;
	ld->set_setting(key, value);
}

double dypc_loader_get_setting(dypc_loader l, const char* key) {
	dypc::loader* ld = (dypc::loader*)l;
	return ld->get_setting(key);
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
