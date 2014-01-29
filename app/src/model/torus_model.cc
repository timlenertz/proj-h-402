#include <cstdlib>
#include <cmath>
#include "torus_model.h"

namespace dypc {

void torus_model::rewind() {
	remaining_ = number_of_points_;
}

bool torus_model::next_point(point& pt) {
	if(! remaining_) return false;
	
	const float factor = M_PI * 2.0 / RAND_MAX;
	
	float theta = factor * std::rand();
	float phi = factor * std::rand();
	
	float x = std::cos(theta) * (r0_ + r1_*std::cos(phi));
	float y = std::sin(theta) * (r0_ + r1_*std::cos(phi));
	float z = r1_ * std::sin(phi);

	pt = point(x, y, z);
	--remaining_;
	return true;
}

}
