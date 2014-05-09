#ifndef DYPC_INTERFACE_PROGRESS_H_
#define DYPC_INTERFACE_PROGRESS_H_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	dypc_progress (*open)(const char* label, unsigned maximum, 
	dypc_progress parent);
	void (*close)(dypc_progress id);
	void (*set)(dypc_progress id, unsigned value);
	void (*pulse)(dypc_progress id);
	void (*message)(dypc_progress id, const char* msg);
} dypc_progress_callbacks;

extern dypc_progress_callbacks dypc_current_progress_callbacks;

void dypc_set_progress_callbacks(const dypc_progress_callbacks*) DYPC_INTERFACE_DEC;
void dypc_get_progress_callbacks(dypc_progress_callbacks*) DYPC_INTERFACE_DEC;

void dypc_set_null_progress_callbacks() DYPC_INTERFACE_DEC;

#ifdef __cplusplus
}
#endif

#endif
