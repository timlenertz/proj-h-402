#include "tree_structure_ordered_loader.h"
#include <memory>
#include <algorithm>

namespace dypc {

void tree_structure_ordered_loader::updated_source_() {
	position_path_.clear();
	if(source_) position_path_.push_back(& source_->root_node());
}

std::size_t tree_structure_ordered_loader::extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const tree_structure_source::node& nd, const source_node* skip) const {
	const std::size_t levels = source_->levels();
	const std::size_t number_of_node_children = source_->number_of_node_children();
	
	auto action = action_for_node_(nd.node_cuboid(), nd.number_of_points(), nd.is_leaf(), req, levels);
	
	if(action == action_skip) {
		return 0;
		
	} else if(action == action_split) {
		std::vector<const source_node*> children;
		for(std::ptrdiff_t i = 0; i < number_of_node_children; ++i) if(nd.has_child(i)) {
			const source_node* child = & nd.child(i);
			if(child != skip) children.push_back(child);
		}
		std::sort(children.begin(), children.end(), [&](const source_node* a, const source_node* b) {
			return (cuboid_distance_(req.position, a->node_cuboid()) < cuboid_distance_(req.position, b->node_cuboid()));
		});
		
		std::size_t c = 0;
		for(const source_node* nd : children) c += this->extract_node_points_(points + c, capacity - c, req, *nd);
		return c;
		
	} else {
		std::ptrdiff_t lvl = action;
		if(lvl >= levels) lvl = levels - 1;
		
		return nd.extract_points(points, capacity, lvl);
	}
}

	

std::size_t tree_structure_ordered_loader::extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
	bool outside_root = false;
	while(!outside_root && !position_path_.back()->node_cuboid().in_range(req.position)) {
		if(position_path_.size() > 1) position_path_.pop_back();
		else outside_root = true;
	}
	if(! outside_root) while(! position_path_.back()->is_leaf()) {
		const auto& nd = *position_path_.back();
		assert(nd.node_cuboid().in_range(req.position));
		std::ptrdiff_t i = nd.child_for_point(req.position);
		if(i == tree_structure_source::node::no_child_index) break;
		
		position_path_.push_back(& nd.child(i));
	}
	
	std::size_t c = 0;
	const source_node* previous = nullptr;
	for(auto it = position_path_.rbegin(); c < capacity && it != position_path_.rend(); ++it) {
		c += this->extract_node_points_(points + c, capacity - c, req, **it, previous);
		previous = *it;
	}
	
	return c;
}

}
