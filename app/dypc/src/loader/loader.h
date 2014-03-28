#ifndef DYPC_LOADER_H_
#define DYPC_LOADER_H_

#include "../point.h"
#include "../frustum.h"
#include "../enums.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <chrono>
#include <stdexcept>

namespace dypc {

/**
 * Abstract base class for loader.
 * The loader generates the set of points to be rendered, based on a current request (@see loader::request_t).
 */
class loader {
protected:
	bool adaptive_ = true;
	
public:
	/**
	 * Request for the loader.
	 * Indicates the current position, orientation, etc. of the camera in the model, and used by loader to determine
	 * set of points to be loaded.
	 */
	struct request_t {
		request_t() = default;
		
		/**
		 * Generate request with given values.
		 * Sets the data members. The view frustum is deduced from the matrix.
		 */
		request_t(const glm::vec3& pos, const glm::vec3& vel, const glm::quat& ori, const glm::mat4& mat) :
			position(pos), velocity(vel), orientation(ori), view_projection_matrix(mat), view_frustum(mat) { }
		
		
		/**
		 * Position of the camera in the point cloud.
		 */
		glm::vec3 position;
		
		/**
		 * Velocity at which the camera is currently moving through the point cloud.
		 */
		glm::vec3 velocity;
		
		/**
		 * Direction the camera is looking at, expressed as quaternion.
		 */
		glm::quat orientation;
		
		/**
		 * Current View-projection matrix. 
		 */
		glm::mat4 view_projection_matrix;
		
		/**
		 * Current view frustum.
		 * Automatically generated from matrix by constructor.
		 */
		frustum view_frustum;
	};
	
	virtual ~loader();

	/**
	 * Load a point set, and write it into buffer.
	 * @param request The request_t object.
	 * @param points Buffer to write points into.
	 * @param count On output: Number of points written into buffer.
	 * @param capacity Maximal number of points that may be loaded and written to buffer.
	 */
	virtual void compute_points(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) = 0;
	
	/**
	 * Asks the loader whether a new point set should be loaded.
	 * @param request The current request.
	 * @param previous The previous request for which compute_points was called.
	 * @param dtime The elapsed time since the last compute_points call.
	 * @return If true, the loader recommends that a new point set should be loaded.
	 */
	virtual bool should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) = 0;
	
	/**
	 * Get human-readable name of loader.
	 */
	virtual std::string loader_name() const;
	virtual loader_type get_loader_type() const = 0;
	
	virtual double get_setting(const std::string&) const { throw std::logic_error("No configuration for loader"); }
	virtual void set_setting(const std::string&, double) { throw std::logic_error("No configuration for loader"); }
	
	virtual std::size_t memory_size() const { return 0; }
	virtual std::size_t rom_size() const { return 0; }
	virtual std::size_t number_of_points() const = 0;
};
	
}

#endif
