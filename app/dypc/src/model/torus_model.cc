#include <cstdlib>
#include <cmath>
#include <random>
#include "torus_model.h"

namespace dypc {
	
torus_model::torus_model(std::size_t count, float r0, float r1) :
random_model(
	count,
	glm::vec3(-(r0 + r1), -(r0 + r1), -(r0 + r1)),
	glm::vec3(r0 + r1, r0 + r1, r0 + r1)
), r0_(r0), r1_(r1) { }


point torus_model::compute_point_(random_generator_t& gen, std::size_t n) const {
	std::uniform_real_distribution<float> dist(0.0, M_PI*2.0);
		
	float theta = dist(gen), phi = dist(gen);
	
	float x = std::cos(theta) * (r0_ + r1_*std::cos(phi));
	float y = std::sin(theta) * (r0_ + r1_*std::cos(phi));
	float z = r1_ * std::sin(phi);

	unsigned char r = 55.0 + 200.0*(x - minimum_[0])/(maximum_[0] - minimum_[0]);
	unsigned char g = 55.0 + 200.0*(y - minimum_[1])/(maximum_[1] - minimum_[1]);
	unsigned char b = 55.0 + 200.0*(z - minimum_[2])/(maximum_[2] - minimum_[2]);

	return point(x, y, z, r, g, b);
}

}
