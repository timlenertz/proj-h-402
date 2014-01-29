#ifndef DYPC_CONCENTRIC_SPHERES_MODEL_H_
#define DYPC_CONCENTRIC_SPHERES_MODEL_H_

#include "model.h"

namespace dypc {

class concentric_spheres_model : public model {
private:
	const float inner_;
	const float outer_;
	const unsigned steps_;
	
	std::size_t remaining_;
	
public:
	concentric_spheres_model(std::size_t count, float inner, float outer, unsigned steps) : inner_(inner), outer_(outer), steps_(steps), remaining_(count) {
		number_of_points_ = count;
		minimum_ = glm::vec3(-outer, -outer, -outer);
		maximum_ = glm::vec3(outer, outer, outer);
	}
	
	concentric_spheres_model(const concentric_spheres_model&) = default;
	
	void rewind() override;
	bool next_point(point&) override;
};

}

#endif
