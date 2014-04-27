#ifndef DYPC_TORUS_MODEL_H_
#define DYPC_TORUS_MODEL_H_

#include "random_model.h"

namespace dypc {

class torus_model : public random_model {
private:
	const float r0_;
	const float r1_;

protected:
	point compute_point_(random_generator_t&, std::size_t n) const override;

public:
	torus_model(std::size_t count, float r0, float r1);
};

}

#endif
