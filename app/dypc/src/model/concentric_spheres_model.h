#ifndef DYPC_CONCENTRIC_SPHERES_MODEL_H_
#define DYPC_CONCENTRIC_SPHERES_MODEL_H_

#include "random_model.h"

namespace dypc {

/**
 * Randomly generated model consisting of concentric spheres.
 * Consists of a given number of concentric spheres of different colors, with a given
 * inner and outer radius. The points are distributed and ordered randomly.
 */
class concentric_spheres_model : public random_model {
private:
	const float inner_; ///< Radius of innermost sphere.
	const float outer_; ///< Radius of outermost sphere.
	const unsigned steps_; ///< Number of spheres.

protected:
	point compute_point_(random_generator_t&, std::size_t n) const override;
	
public:
	/**
	 * Create concentric spheres model.
	 * @param count Total number of points.
	 * @param inner Radius of innermost sphere.
	 * @param outer Radius of outermost sphere.
	 * @param steps Number of spheres.
	 */
	concentric_spheres_model(std::size_t count, float inner, float outer, unsigned steps);
};

}

#endif
