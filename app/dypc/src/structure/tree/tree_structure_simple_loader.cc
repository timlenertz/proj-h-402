#include "tree_structure_simple_loader.h"

namespace dypc {

std::size_t tree_structure_simple_loader::extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const tree_structure_source::node& nd) const {
	const std::size_t levels = source_->levels();
	const std::size_t number_of_node_children = source_->number_of_node_children();
	
	auto action = action_for_node_(nd.node_cuboid(), nd.number_of_points(), nd.is_leaf(), req, levels);
	
	if(action == action_skip) {
		return 0;
		
	} else if(action == action_split) {
		std::size_t c = 0;
		for(std::ptrdiff_t i = 0; i < number_of_node_children; ++i) if(nd.has_child(i)) c += extract_node_points_(points + c, capacity - c, req, nd.child(i));
		return c;
		
	} else {
		std::ptrdiff_t lvl = action;
		if(lvl >= levels) lvl = levels - 1;
		
		return nd.extract_points(points, capacity, lvl);
	}
}

}
