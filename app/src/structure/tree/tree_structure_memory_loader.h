#ifndef DYPC_TREE_STRUCTURE_MEMORY_LOADER_H_
#define DYPC_TREE_STRUCTURE_MEMORY_LOADER_H_

#include "tree_structure_loader.h"
#include "tree_structure.h"
#include <utility>
#include <stack>
#include <cassert>

namespace dypc {

template<class Structure>
class tree_structure_memory_loader : public tree_structure_loader {
private:
	struct path_entry {
		const typename Structure::node& node;
		cuboid node_cuboid;
	};

	Structure structure_;
	std::stack<path_entry> path_;

public:
	template<class... Args> tree_structure_memory_loader(Args&&... args) : structure_(std::forward<Args>(args)...) {
		path_.push({ structure_.root_node(), structure_.root_cuboid() });
	}
	
	std::size_t extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const typename Structure::node& nd, const cuboid& cub, unsigned depth) const;
	
protected:
	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) override;
	
	std::size_t memory_size_() const override { return structure_.size() + path_.size()*sizeof(path_entry); }
	std::size_t file_size_() const override { return 0; }
	std::size_t total_points_() const override { return structure_.number_of_points(); }
};



template<class Structure>
std::size_t tree_structure_memory_loader<Structure>::extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
	/*
	bool outside_root = false;
	while(! path_.top().node_cuboid.in_range(req.position)) {
		if(path_.size() > 1) path_.pop();
		else outside_root = true;
	}
	if(! outside_root) while(! path_.top().node.is_leaf()) {
		const auto& e = path_.top();
		assert(e.node_cuboid.in_range(req.position));
		std::ptrdiff_t i = Structure::splitter::node_child_for_point(req.position, e.node_cuboid, e.node.get_points_information(), path_.size() - 1);
		
		const auto& child_node = e.node.child(i);
		auto child_cuboid = Structure::splitter::node_child_cuboid(i, e.node_cuboid, e.node.get_points_information(), path_.size() - 1);
		
		path_.push({ child_node, child_cuboid });
	}
	*/
	return extract_node_points_(points, capacity, req, structure_.root_node(), structure_.root_cuboid(), 0);
}



template<class Structure>
std::size_t tree_structure_memory_loader<Structure>::extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const typename Structure::node& nd, const cuboid& cub, unsigned depth) const {
	auto action = action_for_node_(cub, nd.number_of_points(), nd.is_leaf(), req, Structure::levels);
	
	if(action == action_skip) {
		return 0;
		
	} else if(action == action_split) {
		std::size_t c = 0;
		for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) {
			auto child_cuboid = Structure::splitter::node_child_cuboid(i, cub, nd.get_points_information(), depth);
			c += extract_node_points_(points + c, capacity - c, req, nd.child(i), child_cuboid, depth + 1);
		}
		return c;
		
	} else {
		std::ptrdiff_t lvl = action;
		if(lvl >= Structure::levels) lvl = Structure::levels - 1;
		
		if(! nd.number_of_points(0)) return 0;

		const point* pts = nd.points_begin(lvl);
		const point* pts_end = nd.points_end(lvl);
		std::size_t c = 0;
		for(const point* pt = pts; pt != pts_end && c < capacity; ++pt) {
			*(++points) = *pt; ++c;
		}
		return c;
	}
}

}

#endif
