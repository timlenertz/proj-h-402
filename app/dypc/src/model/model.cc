#include "model.h"
#include "../progress.h"

namespace dypc {

cuboid model::bounding_cuboid(float ep) {
	float two_ep = ep + ep;
	return cuboid(
		glm::vec3(x_minimum() - ep, y_minimum() - ep, z_minimum() - ep),
		glm::vec3(x_range() + two_ep, y_range() + two_ep, z_range() + two_ep)
	);
}

void model::find_bounds_() {
	auto it = begin();
	auto it_end = end();
	
	minimum_ = maximum_ = *it;
	
	progress_foreach(it, it_end, number_of_points(), "Finding bounds of model", [&](const point& pt) {
		if(pt.x < minimum_[0]) minimum_[0] = pt.x;
		else if(pt.x > maximum_[0]) maximum_[0] = pt.x;
		
		if(pt.y < minimum_[1]) minimum_[1] = pt.y;
		else if(pt.y > maximum_[1]) maximum_[1] = pt.y;

		if(pt.z < minimum_[2]) minimum_[2] = pt.z;
		else if(pt.z > maximum_[2]) maximum_[2] = pt.z;
	});
}
	
}
