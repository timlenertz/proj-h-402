#ifndef DYPC_DIRECT_MODEL_LOADER_H_
#define DYPC_DIRECT_MODEL_LOADER_H_

#include "loader.h"
#include "../model/model.h"
#include <utility>
#include <memory>

namespace dypc {

/**
 * Loader which directly loads points from model.
 * Does no filtering of model based on request, and does not buffer points.
 */
class direct_model_loader : public loader {
private:
	std::unique_ptr<model> model_;

public:
	/**
	 * Generate direct model loader for given model.
	 * @param md Pointer to model. The direct model loaded takes ownership of model object.
	 */
	explicit direct_model_loader(model* md) : model_(md) { }
	
	/**
	 * Loads points directly from model.
	 * Reads as many points as fit into capacity.
	 */
	void compute_points(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) override;
	
	/**
	 * Returns false.
	 */
	bool should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) override;
	
	std::string loader_name() const override { return "Direct Model Loader"; }

	loader_type get_loader_type() const override { return loader_type::direct; }
	
	std::size_t number_of_points() const override { return model_->number_of_points(); }
};


inline void direct_model_loader::compute_points(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) {
	std::size_t ct = 0;
	point_buffer_t current = points;
	for(point pt : *model_) {
		if(ct == capacity) break; 
		*(current++) = pt;
		++ct;
	}
	count = ct;
}


inline bool direct_model_loader::should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) {
	return false;
}


}

#endif
