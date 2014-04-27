#ifndef DYPC_MODEL_H_
#define DYPC_MODEL_H_

#include <cstddef>
#include <iterator>
#include <utility>
#include <memory>
#include <glm/glm.hpp>
#include "../point.h"
#include "../geometry/cuboid.h"

#include <iostream>

namespace dypc {

/**
 * Model consisting of point cloud
 * The model object does not store the point cloud data in memory, but provides an interface for loading it from its source. This may for example be a PLY file (@see ply_model), or a generator which outputs random points that form a given shape (@see torus_model).
 * Upon request, the bounds of the point cloud are computed (@see compute_bounds_). This may involve scanning through the entire file.
 * 
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
		virtual std::size_t read(point* buffer, std::size_t n) = 0;
		virtual bool eof() = 0;
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
	
	std::size_t number_of_points() const { return number_of_points_; } ///< Get number of points in model
	std::size_t size() const { return number_of_points(); } ///< Get number of points in model
	float x_minimum() { return get_minimum().x; } ///< Get minimal X coordinate
	float x_maximum() { return get_maximum().x; } ///< Get maximal X coordinate
	float x_range() { return get_maximum().x - get_minimum().x; } ///< Get X length
	float y_minimum() { return get_minimum().y; } ///< Get minimal Y coordinate
	float y_maximum() { return get_maximum().y; } ///< Get maximal Y coordinate
	float y_range() { return get_maximum().y - get_minimum().y; } ///< Get Y length
	float z_minimum() { return get_minimum().z; } ///< Get minimal Z coordinate
	float z_maximum() { return get_maximum().z; } ///< Get maximal Z coordinate
	float z_range() { return get_maximum().z - get_minimum().z; } ///< Get Z length
	
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
	static constexpr std::size_t maximal_chunk_size_ = 1024;

	std::unique_ptr<handle> handle_;
	point* buffer_ = nullptr;
	std::size_t chunk_size_ = 0;
	std::ptrdiff_t point_ = 0;
	
	void read_next_chunk_();
	
public:
	iterator() = default;
	iterator(const iterator& it);
	iterator(iterator&& it);
	iterator& operator=(const iterator& it);
	iterator& operator=(iterator&& it);
	~iterator();
	explicit iterator(std::unique_ptr<handle>&& hd);

	bool operator==(const iterator& it) const;
	bool operator!=(const iterator& it) const;

	iterator& operator++();
	
	point& operator*() {		
		return buffer_[point_];
	}
	
	point* operator->() {
		return & buffer_[point_];
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
