#ifndef DYPC_INTERFACE_DYPC_H_
#define DYPC_INTERFACE_DYPC_H_

#include "types.h"

#define DYPC_INTERFACE_DEC \
	__attribute__(( visibility("default") ))

#ifdef __cplusplus
extern "C" {
#endif

typedef void* dypc_progress;

typedef struct {
	dypc_progress (*open_progress)(const char* label, unsigned maximum, dypc_progress parent);
	void (*set_progress)(dypc_progress id, unsigned value);
	void (*close_progress)(dypc_progress id);
} dypc_callbacks;


dypc_callbacks* dypc_get_callbacks() DYPC_INTERFACE_DEC;

int dypc_error() DYPC_INTERFACE_DEC;
const char* dypc_error_message() DYPC_INTERFACE_DEC;

void dypc_clear_error() DYPC_INTERFACE_DEC;
void dypc_set_error_message(const char*) DYPC_INTERFACE_DEC;

#ifdef __cplusplus
}
#endif

#endif
