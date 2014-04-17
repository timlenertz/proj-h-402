#ifndef DYPC_MODEL_H_
#define DYPC_MODEL_H_

#include <cstddef>
#include <iterator>
#include <glm/glm.hpp>
#include "../point.h"
#include "../cuboid.h"

namespace dypc {

class model {
private:
	bool has_bounds_() const {
		return (minimum_ != maximum_);
	}
		
protected:
	std::size_t number_of_points_ = 0;
	mutable glm::vec3 minimum_;
	mutable glm::vec3 maximum_;

	virtual void compute_bounds_() { }
	
public:
	class iterator;
	
	iterator begin();
	iterator end();
	
	glm::vec3 get_minimum() {
		if(! has_bounds_()) this->compute_bounds_();
		return minimum_;
	}
	
	glm::vec3 get_maximum() {
		if(! has_bounds_()) this->compute_bounds_();
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
	
	cuboid bounding_cuboid(float ep = 0.0) {
		float two_ep = ep + ep;
		return cuboid(
			glm::vec3(x_minimum() - ep, y_minimum() - ep, z_minimum() - ep),
			glm::vec3(x_range() + two_ep, y_range() + two_ep, z_range() + two_ep)
		);
	}
	
	cuboid enclosing_cuboid() {
		return bounding_cuboid(0.001);
	}
	
	virtual ~model() { }
	
	virtual void rewind() = 0;
	virtual bool next_point(point&) = 0;
};


class model::iterator : public std::iterator<std::input_iterator_tag, point> {
private:
	model* model_;
	point point_;
	
public:
	iterator() : model_(nullptr) { }

	explicit iterator(model& mod) : model_(&mod) {
		model_->rewind();
		operator++();
	} 

	bool operator==(const iterator& it) const { return model_ == it.model_; }
	bool operator!=(const iterator& it) const { return model_ != it.model_; }

	iterator& operator++() {
		bool ok = model_->next_point(point_);
		if(! ok) model_ = nullptr;
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
	return iterator(*this);
}

inline model::iterator model::end() {
	return iterator();
}

}

#endif
