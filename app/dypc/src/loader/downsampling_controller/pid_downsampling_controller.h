#ifndef DYPC_PID_DOWNSAMPLING_CONTROLLER_H_
#define DYPC_PID_DOWNSAMPLING_CONTROLLER_H_

#include "downsampling_controller.h"

namespace dypc {

class pid_downsampling_controller : public downsampling_controller {
private:
	float proportional_gain = 1.0;
	float integral_gain = 1.0;
	float derivative_gain = 1.0;

public:
	float adapt_setting(float current_setting, float current_error) override;
};
	
}

#endif
