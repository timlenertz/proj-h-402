#include "model.h"
#include "../progress.h"
#include <cstring>

namespace dypc {

cuboid model::bounding_cuboid(float ep) {
	float two_ep = ep + ep;
	
	return cuboid(
		glm::vec3(x_minimum() - ep, y_minimum() - ep, z_minimum() - ep),
		glm::vec3(x_range() + two_ep, y_range() + two_ep, z_range() + two_ep)
	);
}

void model::find_bounds_() {
	auto it = begin();
	auto it_end = end();
	
	// Use first point's coordinates as initial min/max
	minimum_ = maximum_ = *it;
	
	progress_foreach(it, it_end, number_of_points(), "Finding bounds of model", [&](const point& pt) {
		if(pt.x < minimum_[0]) minimum_[0] = pt.x;
		else if(pt.x > maximum_[0]) maximum_[0] = pt.x;
		
		if(pt.y < minimum_[1]) minimum_[1] = pt.y;
		else if(pt.y > maximum_[1]) maximum_[1] = pt.y;

		if(pt.z < minimum_[2]) minimum_[2] = pt.z;
		else if(pt.z > maximum_[2]) maximum_[2] = pt.z;
	});
}


model::iterator::iterator(std::unique_ptr<handle>&& hd) :
handle_(std::move(hd)), buffer_(new point[maximal_chunk_size_]), point_(0) {
	read_next_chunk_();
}


model::iterator::~iterator() {
	if(buffer_) delete[] buffer_;
}


model::iterator::iterator(const iterator& it) {
	operator=(it);
}


model::iterator::iterator(iterator&& it) :
handle_(it.handle_.release()), buffer_(it.buffer_), chunk_size_(it.chunk_size_), point_(it.point_) {
	it.buffer_ = nullptr;
}


model::iterator& model::iterator::operator=(const iterator& it) {
	if(it.handle_) handle_ = std::move(it.handle_->clone()); // it is not end; copy handle
	else handle_.release(); // it is end iterator

	if(it.buffer_) {
		if(! buffer_) buffer_ = new point[maximal_chunk_size_];
		std::memcpy(buffer_, it.buffer_, it.chunk_size_ * sizeof(point));
		chunk_size_ = it.chunk_size_;
	} else {
		if(buffer_) delete[] buffer_;
		buffer_ = nullptr;
	}

	return *this;
}


model::iterator& model::iterator::operator=(iterator&& it) {
	handle_ = std::move(it.handle_);
	point_ = it.point_;
	chunk_size_ = it.chunk_size_;
	buffer_ = it.buffer_;
	it.buffer_ = nullptr;
	return *this;
}


void model::iterator::read_next_chunk_() {
	assert(handle_); assert(buffer_);
	chunk_size_ = handle_->read(buffer_, maximal_chunk_size_);
	point_ = 0;
}


model::iterator& model::iterator::operator++() {
	assert(handle_);
	++point_;
	if(point_ >= chunk_size_) {		
		// reached end of current chunk
		if(! handle_->eof()) read_next_chunk_(); // handle not yet at eof, read next chunk
		else handle_.release(); // handle at eof (+ enf of chunk) --> close handle, iterator will be == to end iterator
	}
	return *this;
}


bool model::iterator::operator==(const iterator& it) const {
	// handle_ is unique_ptr --> two iterators never have same handle_
	// handle_ = nullptr only for end iterator
	return handle_ == it.handle_;
}


bool model::iterator::operator!=(const iterator& it) const {
	return handle_ != it.handle_;
}

	
}
