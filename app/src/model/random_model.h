#ifndef DYPC_RANDOM_MODEL_H_
#define DYPC_RANDOM_MODEL_H_

#include "model.h"

namespace dypc {

class random_model : public model {
private:
	const float side_;
	
	std::size_t remaining_;
	
public:
	random_model(std::size_t count, std::size_t side) : side_(side), remaining_(count) {
		number_of_points_ = count;
		minimum_ = glm::vec3(-side, -side, -side);
		maximum_ = glm::vec3(side, side, side);
	}
	random_model(const random_model&) = default;
	
	void rewind() override;
	bool next_point(point&) override;
};

}

#endif
