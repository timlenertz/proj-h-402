#include <cstdlib>
#include <cmath>
#include <random>
#include "concentric_spheres_model.h"

namespace dypc {

concentric_spheres_model::concentric_spheres_model(std::size_t count, float inner, float outer, unsigned steps) :
random_model(
	count,
	glm::vec3(-outer, -outer, -outer),
	glm::vec3(outer, outer, outer)
), inner_(inner), outer_(outer), steps_(steps) { }


point concentric_spheres_model::compute_point_(random_generator_t& gen, std::size_t n) const {
	using dist_t = std::uniform_real_distribution<float>;
	
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
	
	const float radius_diff = (steps_>1 ? (outer_ - inner_)/(steps_ - 1) : 0);
	
	unsigned step = std::uniform_int_distribution<unsigned>(0, steps_ - 1)(gen);
	float radius = inner_ + step*radius_diff;
	const unsigned char* col = colors + 3*(step % colors_count);
	
	float theta = dist_t(0.0, M_PI*2.0)(gen);
	float phi = dist_t(0.0, M_PI)(gen);
	
	float x = radius * std::cos(theta) * std::sin(phi);
	float y = radius * std::sin(theta) * std::sin(phi);
	float z = radius * std::cos(phi);
	
	return point(x, y, z, col[0], col[1], col[2]);
}


}
