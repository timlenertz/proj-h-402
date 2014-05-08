#ifndef DYPC_RANDOM_MODEL_H_
#define DYPC_RANDOM_MODEL_H_

#include "model.h"
#include "../util.h"
#include <random>

namespace dypc {

/**
 * Model that generates points based on random number generator.
 */
class random_model : public model {
private:
	class handle : public model::handle {
	private:
		random_model& model_; ///< The random model.
		std::size_t remaining_; ///< Remaining number of points.
		random_generator_t random_generator_; ///< Random number generator. Stores seed.
	
	public:
		explicit handle(random_model& mod) : model_(mod), remaining_(mod.number_of_points()) { }
	
		~handle() override { }
		
		std::size_t read(point* buffer, std::size_t n) override {
			if(n > remaining_) n = remaining_;
			for(std::size_t i = 0; i < n; ++i) *(buffer++) = model_.compute_point_(random_generator_, remaining_--);
			return n;
		}
		
		bool eof() override {
			return (remaining_ == 0);
		}
		
		std::unique_ptr<model::handle> clone() override {
			handle* h = new handle(model_);
			h->remaining_ = remaining_;
			h->random_generator_ = random_generator_; // copies current seed
			return std::unique_ptr<model::handle>(h);
		}
	};
	
protected:
	std::unique_ptr<model::handle> make_handle_() override {
		return std::unique_ptr<model::handle>(new handle(*this));
	}
	
	/**
	 * Create random model.
	 * Called by subclass.
	 * @param n Total number of points.
	 * @param minimum Minimum point coordinate.
	 * @param minimum Maximum point coordinate.
	 */
	random_model(std::size_t n, glm::vec3 minimum, glm::vec3 maximum) {
		number_of_points_ = n;
		minimum_ = minimum;
		maximum_ = maximum;
	}
	
	virtual point compute_point_(random_generator_t&, std::size_t n) const = 0;
};

}

#endif
