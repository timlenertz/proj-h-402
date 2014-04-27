#ifndef DYPC_MODEL_H_
#define DYPC_MODEL_H_

#include <cstddef>
#include <iterator>
#include <utility>
#include <memory>
#include <glm/glm.hpp>
#include "../point.h"
#include "../geometry/cuboid.h"

namespace dypc {

/**
 * Model consisting of full point cloud
 * The model object does not store the point cloud data in memory, but provides an interface for loading it from its source. This may for example be a PLY file (@see ply_model), or a generator which outputs random points that form a given shape (@see torus_model).
 * Upon request, the bounds of the point cloud are computed (@see compute_bounds_). This may involve scanning through the entire file.
 * The class provides an iterator interface (@see begin, @see end). The order of points depends on the source.
 */
class model {
private:
	bool has_bounds_() const {
		return (minimum_ != maximum_);
	}
	
	void find_bounds_();
		
protected:
	class handle {
	protected:
		handle() = default;
		handle(const handle&) = delete;
		handle& operator=(const handle&) = delete;
	public:
		virtual ~handle() { }
		virtual bool read(point&) = 0;
		virtual std::unique_ptr<handle> clone() = 0;
	};

	std::size_t number_of_points_ = 0;
	mutable glm::vec3 minimum_;
	mutable glm::vec3 maximum_;
	
	virtual std::unique_ptr<handle> make_handle_() = 0;
	
public:
	class iterator;

	iterator begin();
	iterator end();
	
	virtual ~model() { }
	
	/**
	 * Get minimal X, Y, Z coordinates
	 * Finds bounds first if not yet done.
	 */
	glm::vec3 get_minimum() {
		if(! has_bounds_()) this->find_bounds_();
		return minimum_;
	}
	
	/**
	 * Get maximal X, Y, Z coordinates
	 * Finds bounds first if not yet done.
	 */
	glm::vec3 get_maximum() {
		if(! has_bounds_()) this->find_bounds_();
		return maximum_;
	}
	
	std::size_t number_of_points() const { return number_of_points_; }
	std::size_t size() const { return number_of_points(); }
	float x_minimum() { return get_minimum().x; }
	float x_maximum() { return get_maximum().x; }
	float x_range() { return get_maximum().x - get_minimum().x; }
	float y_minimum() { return get_minimum().y; }
	float y_maximum() { return get_maximum().y; }
	float y_range() { return get_maximum().y - get_minimum().y; }
	float z_minimum() { return get_minimum().z; }
	float z_maximum() { return get_maximum().z; }
	float z_range() { return get_maximum().z - get_minimum().z; }
	
	/**
	 * Cuboid which bounds the model
	 * @param ep If non-zero, expands cuboid by this amount in all directions
	 */
	cuboid bounding_cuboid(float ep = 0.0);
	
	/**
	 * Cuboid which is guaranteed to contain all points of the model
	 * Slightly expanded version of bounding cuboid.
	 */
	cuboid enclosing_cuboid() {
		return bounding_cuboid(0.001);
	}
};


class model::iterator : public std::iterator<std::input_iterator_tag, point> {
private:
	std::unique_ptr<handle> handle_;
	point point_;
	
public:
	iterator() = default;
	
	iterator(const iterator& it) { *this = it; }
	
	iterator(iterator&& it) : handle_(it.handle_.release()), point_(it.point_) { }

	iterator& operator=(const iterator& it) {
		if(it.handle_) handle_ = std::move(it.handle_->clone());
		else handle_.release();
		point_ = it.point_;
		return *this;
	}
	
	iterator& operator=(iterator&& it) {
		handle_ = std::move(it.handle_);
		point_ = it.point_;
		return *this;
	}

	explicit iterator(std::unique_ptr<handle>&& hd) : handle_(std::move(hd)) {
		operator++();
	} 

	bool operator==(const iterator& it) const { return handle_ == it.handle_; }
	bool operator!=(const iterator& it) const { return handle_ != it.handle_; }

	iterator& operator++() {
		assert(handle_);
		bool ok = handle_->read(point_);
		if(! ok) handle_.release();
		return *this;
	}
	
	point& operator*() {
		return point_;
	}
	
	point* operator->() {
		return &point_;
	}
};

inline model::iterator model::begin() {
	return iterator(std::move(this->make_handle_()));
}

inline model::iterator model::end() {
	return iterator();
}

}

#endif
