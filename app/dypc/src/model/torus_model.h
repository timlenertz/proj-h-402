#ifndef DYPC_TORUS_MODEL_H_
#define DYPC_TORUS_MODEL_H_

#include "model.h"

namespace dypc {

class torus_model : public model {
private:
	const float r0_;
	const float r1_;
	
	std::size_t remaining_;
	
public:
	torus_model(std::size_t count, float r0, float r1) : r0_(r0), r1_(r1), remaining_(count) {
		number_of_points_ = count;
		minimum_ = glm::vec3(-(r0 + r1), -(r0 + r1), -(r0 + r1));
		maximum_ = glm::vec3(r0 + r1, r0 + r1, r0 + r1);
	}
	torus_model(const torus_model&) = default;
	
	void rewind() override;
	bool next_point(point&) override;
};

}

#endif
