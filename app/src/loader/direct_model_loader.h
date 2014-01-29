#ifndef DYPC_DIRECT_MODEL_LOADER_H_
#define DYPC_DIRECT_MODEL_LOADER_H_

#include "loader.h"
#include "../model/model.h"
#include <utility>

namespace dypc {

template<class Model>
class direct_model_loader : public loader {
private:
	Model model_;

public:
	template<class... Args>
	direct_model_loader(Args&&... args) : model_(std::forward<Args>(args)...) { }
	
	void compute_points_(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) override;
	bool should_compute_points_(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) override;
};


template<class Model>
void direct_model_loader<Model>::compute_points_(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) {
	std::size_t ct = 0;
	point_buffer_t current = points;
	for(point pt : model_) {
		if(ct == capacity) break; 
		*(current++) = pt;
		++ct;
	}
	count = ct;
}


template<class Model>
bool direct_model_loader<Model>::should_compute_points_(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) {
	return false;
}


}

#endif
