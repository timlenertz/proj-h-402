#ifndef DYPC_INTERFACE_DYPC_H_
#define DYPC_INTERFACE_DYPC_H_

#include "common.h"
#include "loader.h"
#include "model.h"
#include "progress.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int dypc_error DYPC_INTERFACE_DEC;

const char* dypc_error_message() DYPC_INTERFACE_DEC;

void dypc_clear_error() DYPC_INTERFACE_DEC;
void dypc_set_error_message(const char*) DYPC_INTERFACE_DEC;

#ifdef __cplusplus
}
#endif

#endif
