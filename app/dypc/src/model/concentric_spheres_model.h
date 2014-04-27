#ifndef DYPC_CONCENTRIC_SPHERES_MODEL_H_
#define DYPC_CONCENTRIC_SPHERES_MODEL_H_

#include "random_model.h"

namespace dypc {

class concentric_spheres_model : public random_model {
private:
	const float inner_;
	const float outer_;
	const unsigned steps_;

protected:
	point compute_point_(random_generator_t&, std::size_t n) const override;
	
public:
	concentric_spheres_model(std::size_t count, float inner, float outer, unsigned steps);
};

}

#endif
