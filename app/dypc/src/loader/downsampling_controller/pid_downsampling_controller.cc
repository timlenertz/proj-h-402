#include "pid_downsampling_controller.h"

namespace dypc {

float pid_downsampling_controller::adapt_setting(float current_setting, float current_error) {
	float s = current_setting + 0.5*current_error;
	if(s < 0.0) s = 0.0;
	return s;
}
	
}
