#ifndef DYPC_INTERFACE_MODEL_H_
#define DYPC_INTERFACE_MODEL_H_

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dypc_model_opaque;
typedef struct dypc_model_opaque* dypc_model;

dypc_model dypc_create_ply_model(const char* filename, float scale);
dypc_model dypc_create_concentric_spheres_model(dypc_size count, float inner, float outer, unsigned steps);
dypc_model dypc_create_torus_model(dypc_size count, float r0, float r1);
dypc_model dypc_create_random_model(dypc_size count, float side);
void dypc_delete_model(dypc_model);

#ifdef __cplusplus
}
#endif

#endif
