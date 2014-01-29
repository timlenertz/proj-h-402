#include <cstdlib>
#include <cmath>
#include "concentric_spheres_model.h"

namespace dypc {

void concentric_spheres_model::rewind() {
	remaining_ = number_of_points_;
}

bool concentric_spheres_model::next_point(point& pt) {
	if(! remaining_) return false;
	
	const unsigned char colors[]  = {
		220, 30, 30,
		60, 180, 60,
		30, 30, 220,
		150, 150, 150,
		200, 200, 60,
		200, 60, 200,
		60, 200, 200
	};
	auto colors_count = sizeof(colors)/(3*sizeof(unsigned char));
	
	const float factor = M_PI * 2.0 / RAND_MAX;
	const float radius_diff = (steps_>1 ? (outer_ - inner_)/(steps_ - 1) : 0);
	
	unsigned step = std::rand() % steps_;
	float radius = inner_ + step*radius_diff;
	const unsigned char* col = colors + 3*(step % colors_count);
	
	float theta = factor * std::rand();
	float phi = factor/2 * std::rand();
	
	float x = radius * std::cos(theta) * std::sin(phi);
	float y = radius * std::sin(theta) * std::sin(phi);
	float z = radius * std::cos(phi);
	
	pt = point(x, y, z, col[0], col[1], col[2]);
	--remaining_;
	return true;
}

}
