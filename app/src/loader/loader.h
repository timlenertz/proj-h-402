#ifndef DYPC_LOADER_H_
#define DYPC_LOADER_H_

#include "../point.h"
#include "../statistics.h"
#include "../frustum.h"


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

class wxWindow;

namespace dypc {

class loader {
public:
	struct request_t {
		glm::vec3 position;
		glm::vec3 velocity;
		glm::quat orientation;
		glm::mat4 view_projection_matrix;
		frustum view_frustum;
	};
	
	virtual ~loader();

	virtual void compute_points(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) = 0;
	virtual bool should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) = 0;
	
	virtual std::string loader_name() const;
	virtual ::wxWindow* create_panel(::wxWindow* parent);
};
	
}

#endif
