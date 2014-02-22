#ifndef DYPC_INTERFACE_LOADER_H_
#define DYPC_INTERFACE_LOADER_H_

#include "model.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dypc_loader_opaque;
typedef struct dypc_loader_opaque* dypc_loader;


dypc_loader dypc_create_direct_model_loader(dypc_model);
dypc_loader dypc_create_file_structure_loader(const char* filename, dypc_tree_structure_loader_type ltype);
void dypc_delete_loader(dypc_loader);

dypc_loader dypc_create_cubes_structure_loader(dypc_model mod, float side);
dypc_loader dypc_create_mipmap_cubes_structure_loader(dypc_model mod, float side, unsigned levels, float mmfac, dypc_downsampling_mode dmode);
dypc_loader dypc_create_tree_structure_loader(dypc_model mod, dypc_tree_structure_type str, unsigned levels, dypc_size leaf_cap, float mmfac, dypc_downsampling_mode dmode, dypc_size dmax, dypc_tree_structure_loader_type ltype);


void dypc_write_cubes_structure_to_file(const char* filename, dypc_model mod, float side);
void dypc_write_mipmap_cubes_structure_to_file(const char* filename, dypc_model mod, float side, unsigned levels, float mmfac, dypc_downsampling_mode dmode);
void dypc_write_tree_structure_to_file(const char* filename, dypc_model mod, dypc_tree_structure_type str, unsigned levels, dypc_size leaf_cap, float mmfac, dypc_downsampling_mode dmode, dypc_size dmax);

const char* dypc_loader_name(dypc_loader);

void dypc_loader_set_configuration(dypc_loader, const char* key, double value);
double dypc_loader_get_configuration(dypc_loader, const char* key);


typedef struct {
	dypc_vector3 position;
	dypc_vector3 velocity;
	dypc_quaternion orientation;
	dypc_matrix4 view_projection_matrix;
} dypc_loader_request;

void dypc_loader_compute_points(dypc_loader, const dypc_loader_request* request, dypc_points_buffer buffer, dypc_size* count);
dypc_bool dypc_loader_should_compute_points(dypc_loader, const dypc_loader_request* request, const dypc_loader_request* previous_request, dypc_milliseconds dtime);


#ifdef __cplusplus
}
#endif

#endif
