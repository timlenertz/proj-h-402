#ifndef DYPC_INTERFACE_MODEL_H_
#define DYPC_INTERFACE_MODEL_H_

#include "dypc.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dypc_model_opaque;
typedef struct dypc_model_opaque* dypc_model;

dypc_model dypc_create_ply_model(const char* filename, float scale) DYPC_INTERFACE_DEC;
dypc_model dypc_create_concentric_spheres_model(dypc_size count, float inner, float outer, unsigned steps) DYPC_INTERFACE_DEC;
dypc_model dypc_create_torus_model(dypc_size count, float r0, float r1) DYPC_INTERFACE_DEC;
dypc_model dypc_create_random_model(dypc_size count, float side) DYPC_INTERFACE_DEC;
void dypc_delete_model(dypc_model);

void dypc_model_get_bounds(dypc_model, float* x, float* y, float* z) DYPC_INTERFACE_DEC;

size_t dypc_uniform_downsampling_side_length_statistics(char* output, size_t output_max, dypc_model mod, float max_side, float step) DYPC_INTERFACE_DEC;
void dypc_uniform_downsampling_side_length_statistics_print(dypc_model mod, float max_side, float step) DYPC_INTERFACE_DEC;

#ifdef __cplusplus
}
#endif

#endif
