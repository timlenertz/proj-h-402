#ifndef DYPC_DOWNSAMPLING_CONTROLLER_H_
#define DYPC_DOWNSAMPLING_CONTROLLER_H_

#include <cstddef>

namespace dypc {

class downsampling_controller {
public:
	virtual ~downsampling_controller() { }
	virtual float adapt_setting(float current_setting, float current_error) = 0;
};

}

#endif
