#ifndef DYPC_RANDOM_MODEL_H_
#define DYPC_RANDOM_MODEL_H_

#include "model.h"
#include <random>

namespace dypc {

class random_model : public model {
protected:
	using random_generator_t = std::mt19937;

private:
	class handle : public model::handle {
	private:
		random_model& model_;
		std::size_t remaining_;
		random_generator_t random_generator_;
	
	public:
		explicit handle(random_model& mod) : model_(mod), remaining_(mod.number_of_points()) { }
	
		~handle() override { }
		
		bool read(point& pt) override {
			if(remaining_) {
				pt = model_.compute_point_(random_generator_, remaining_--);
				return true;
			} else {
				return false;
			}
		}
		
		std::unique_ptr<model::handle> clone() override {
			handle* h = new handle(model_);
			h->remaining_ = remaining_;
			h->random_generator_ = random_generator_;
			return std::unique_ptr<model::handle>(h);
		}
	};
	
protected:
	std::unique_ptr<model::handle> make_handle_() override {
		return std::unique_ptr<model::handle>(new handle(*this));
	}
	
	random_model(std::size_t n, glm::vec3 minimum, glm::vec3 maximum) {
		number_of_points_ = n;
		minimum_ = minimum;
		maximum_ = maximum;
	}
	
	virtual point compute_point_(random_generator_t&, std::size_t n) const = 0;
};

}

#endif
