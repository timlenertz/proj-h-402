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

	unsigned char r = 55.0 + 200.0*(x - minimum_[0])/(maximum_[0] - minimum_[0]);
	unsigned char g = 55.0 + 200.0*(y - minimum_[1])/(maximum_[1] - minimum_[1]);
	unsigned char b = 55.0 + 200.0*(z - minimum_[2])/(maximum_[2] - minimum_[2]);

	pt = point(x, y, z, r, g, b);
	--remaining_;
	return true;
}

}
