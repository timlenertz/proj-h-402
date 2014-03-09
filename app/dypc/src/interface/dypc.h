#ifndef DYPC_INTERFACE_DYPC_H_
#define DYPC_INTERFACE_DYPC_H_

#include "types.h"
#include "loader.h"
#include "model.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* dypc_progress;

typedef struct {
	dypc_progress (*open_progress)(const char* label, unsigned maximum, dypc_progress parent);
	void (*set_progress)(dypc_progress id, unsigned value);
	void (*close_progress)(dypc_progress id);
	
	void (*error_message)(const char* title, const char* msg);
} dypc_callbacks;


dypc_callbacks* dypc_get_callbacks();


#ifdef __cplusplus
}
#endif

#endif
