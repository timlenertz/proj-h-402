#ifndef DYPC_TORUS_MODEL_H_
#define DYPC_TORUS_MODEL_H_

#include "random_model.h"

namespace dypc {

/**
 * Randomly generated model consisting of torus.
 * The torus has a given inner and outer radius, and its color varies.
 * The points are distributed and ordered randomly.
 */
class torus_model : public random_model {
private:
	const float r0_; ///< Outer radius, i.e. radius of ring.
	const float r1_; ///< Inner radius, i.e. thickness of ring.
 
protected:
	point compute_point_(random_generator_t&, std::size_t n) const override;

public:
	/**
	 * Create torus model.
	 * @param count Total number of points.
	 * @param r0 Radius of ring.
	 * @param r1 Thickness of ring.
	 */
	torus_model(std::size_t count, float r0, float r1);
};

}

#endif
