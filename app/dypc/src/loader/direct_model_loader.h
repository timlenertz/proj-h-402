#ifndef DYPC_DIRECT_MODEL_LOADER_H_
#define DYPC_DIRECT_MODEL_LOADER_H_

#include "loader.h"
#include "../model/model.h"
#include <utility>

namespace dypc {

class direct_model_loader : public loader {
private:
	std::unique_ptr<model> model_;

public:
	explicit direct_model_loader(model* md) : model_(md) { }
	
	void compute_points(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) override;
	bool should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) override;
	
	std::string loader_name() const override { return "Direct Model Loader"; }
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
