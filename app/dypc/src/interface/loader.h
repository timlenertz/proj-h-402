#ifndef DYPC_INTERFACE_LOADER_H_
#define DYPC_INTERFACE_LOADER_H_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

dypc_loader dypc_create_direct_model_loader(dypc_model) DYPC_INTERFACE_DEC;
dypc_loader dypc_create_file_structure_loader(const char* filename, dypc_tree_structure_loader_type ltype) DYPC_INTERFACE_DEC;
void dypc_delete_loader(dypc_loader) DYPC_INTERFACE_DEC;

dypc_loader dypc_create_cubes_structure_loader(dypc_model mod, float side) DYPC_INTERFACE_DEC;
dypc_loader dypc_create_mipmap_cubes_structure_loader(dypc_model mod, float side, unsigned levels, dypc_size dmin, float damount, dypc_downsampling_mode dmode) DYPC_INTERFACE_DEC;
dypc_loader dypc_create_tree_structure_loader(dypc_model mod, dypc_structure_type str, unsigned levels, dypc_size leaf_cap, dypc_size dmin, float damount, dypc_downsampling_mode dmode, dypc_tree_structure_loader_type ltype) DYPC_INTERFACE_DEC;

void dypc_write_cubes_structure_to_file(const char* filename, dypc_model mod, float side) DYPC_INTERFACE_DEC;
void dypc_write_mipmap_cubes_structure_to_file(const char* filename, dypc_model mod, float side, unsigned levels, dypc_size dmin, float damount, dypc_downsampling_mode dmode) DYPC_INTERFACE_DEC;
void dypc_write_tree_structure_to_file(const char* filename, dypc_model mod, dypc_structure_type str, unsigned levels, dypc_size leaf_cap, dypc_size dmin, float damount, dypc_downsampling_mode dmode) DYPC_INTERFACE_DEC;

dypc_loader_type dypc_loader_loader_type(dypc_loader) DYPC_INTERFACE_DEC;
const char* dypc_loader_name(dypc_loader) DYPC_INTERFACE_DEC;

void dypc_loader_set_setting(dypc_loader, const char* key, double value) DYPC_INTERFACE_DEC;
double dypc_loader_get_setting(dypc_loader, const char* key) DYPC_INTERFACE_DEC;

void dypc_loader_compute_points(dypc_loader, const dypc_loader_request* request, dypc_points_buffer buffer, dypc_size* count) DYPC_INTERFACE_DEC;
dypc_bool dypc_loader_should_compute_points(dypc_loader, const dypc_loader_request* request, const dypc_loader_request* previous_request, dypc_milliseconds dtime) DYPC_INTERFACE_DEC;


#ifdef __cplusplus
}
#endif

#endif
