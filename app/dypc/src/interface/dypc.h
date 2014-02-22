#ifndef DYPC_INTERFACE_DYPC_H_
#define DYPC_INTERFACE_DYPC_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef int dypc_progress_id;

typedef struct {
	dypc_progress_id (*open_progress)(const char* label, int maximum, dypc_progress_id parent);
	void (*set_progress)(dypc_progress_id id, unsigned value);
	void (*close_progress)(dypc_progress_id id);
	
	void (*error_message)(const char* title, const char* msg);
} dypc_callbacks;


dypc_callbacks* dypc_get_callbacks();


#ifdef __cplusplus
}
#endif

#endif
