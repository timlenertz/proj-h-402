#ifndef DYPC_LOADER_H_
#define DYPC_LOADER_H_

#include "../point.h"
#include "../geometry/frustum.h"
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
 * Loader which outputs set of points to be rendered.
 * The various subclasses load points from the a structure (@see structure_loader) stored in memory or in a file, or directly from the model (@see direct_model_loader). The set of points to load is based on the request (@see loader::request_t). It may apply filtering such as frustum culling, downsampling and others.
 * A two-function interface for getting/setting the various settings of the loaders is provided. (@see get_setting @see set_setting).
 */
class loader {
private:
	static constexpr float minimal_update_distance_ = 1.0;

public:
	/**
	 * Request for the loader.
	 * Indicates the current position, orientation, etc. of the camera in the point cloud, and used by loader to determine set of points to be loaded.
	 */
	struct request_t {
		request_t() = default;
		
		/**
		 * Generate request with given values.
		 * Sets the data members. The view frustum is deduced from the matrix.
		 */
		request_t(const glm::vec3& pos, const glm::vec3& vel, const glm::quat& ori, const glm::mat4& mat) :
			position(pos), velocity(vel), orientation(ori), view_projection_matrix(mat), view_frustum(mat) { }
		
		
		glm::vec3 position; ///< Position of the camera in the point cloud.
		glm::vec3 velocity; ///< Velocity at which the camera is currently moving through the point cloud.
		glm::quat orientation; ///< Direction the camera is looking at, expressed as quaternion.
		glm::mat4 view_projection_matrix; ///< Current View-projection matrix.
		
		/**
		 * Current view frustum.
		 * Automatically generated from matrix by constructor.
		 */
		frustum view_frustum;
	};
	
	virtual ~loader() { }

	/**
	 * Load points into a buffer.
	 * Time-critical function.
	 * @param request The request_t object based on which to select point set.
	 * @param points Buffer to write points into.
	 * @param count On output, number of points written into buffer.
	 * @param capacity Maximal number of points that may be loaded and written to buffer.
	 */
	virtual void compute_points(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) = 0;
	
	/**
	 * Asks the loader whether a new point set should be loaded.
	 * For instance when the camera position or orientation has changed. Loading can always be forced by calling compute_points.
	 * @param request The current request.
	 * @param previous The previous request for which compute_points was called.
	 * @param dtime The elapsed time since the last compute_points call.
	 * @return If true, the loader recommends that a new point set should be loaded.
	 */
	virtual bool should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime);
	
	/**
	 * Get human-readable name of loader.
	 */
	virtual std::string loader_name() const;
	
	/**
	 * Get type of loader.
	 */
	virtual loader_type get_loader_type() const = 0;
	
	/**
	 * Get a string-identified setting of the loader.
	 * Provides interface for parameterizing the loader.
	 * @param setting Name of setting to change.
	 * @return Current value of setting, casted to double
	 */
	virtual double get_setting(const std::string& setting) const;
	
	/**
	 * Change a string-identified setting of the loader.
	 * Provides interface for parameterizing the loader.
	 * @param setting Name of setting to change
	 * @param value New value for setting, casted to double
	 */
	virtual void set_setting(const std::string& setting, double value);
	
	/**
	 * Get amount of RAM memory used by the loader.
	 * For loaders that use structure that stores model in memory (@see structure), returns size of structure. Value is approximate and does not need to correspond precisely to amount of allocated memory.
	 * @return Memory used, in byte.
	 */
	virtual std::size_t memory_size() const { return 0; }
	
	/**
	 * Get amount of secondary storage ROM memory used by loader.
	 * For loaders that read from a file, corresponds to file size.
	 * @return Memory used, in byte.
	 */
	virtual std::size_t rom_size() const { return 0; }
	
	/**
	 * Get total number of points in model.
	 * @return Number of points.
	 */
	virtual std::size_t number_of_points() const = 0;
};
	
}

#endif
