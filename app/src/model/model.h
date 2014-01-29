#ifndef DYPC_MODEL_H_
#define DYPC_MODEL_H_

#include <cstddef>
#include <iterator>
#include <glm/glm.hpp>
#include "../point.h"

namespace dypc {

class model {
protected:
	std::size_t number_of_points_ = 0;
	glm::vec3 minimum_;
	glm::vec3 maximum_;
	
public:
	class iterator;
	
	iterator begin();
	iterator end();
	
	std::size_t number_of_points() const { return number_of_points_; }
	float x_minimum() const { return minimum_.x; }
	float x_maximum() const { return maximum_.x; }
	float x_range() const { return maximum_.x - minimum_.x; }
	float y_minimum() const { return minimum_.y; }
	float y_maximum() const { return maximum_.y; }
	float y_range() const { return maximum_.y - minimum_.y; }
	float z_minimum() const { return minimum_.z; }
	float z_maximum() const { return maximum_.z; }
	float z_range() const { return maximum_.z - minimum_.z; }
	
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
