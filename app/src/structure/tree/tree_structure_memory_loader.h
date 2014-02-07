#ifndef DYPC_TREE_STRUCTURE_MEMORY_LOADER_H_
#define DYPC_TREE_STRUCTURE_MEMORY_LOADER_H_

#include "tree_structure_loader.h"
#include "tree_structure.h"
#include <utility>
#include <vector>
#include <stack>
#include <cassert>
#include <algorithm>
#include <array>
#include <string>

namespace dypc {

template<class Structure>
class tree_structure_memory_loader : public tree_structure_loader {
private:
	struct path_entry {
		const typename Structure::node& node;
		std::ptrdiff_t node_index;
		cuboid node_cuboid;
	};

	Structure structure_;
	std::vector<path_entry> position_path_;

	std::size_t extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const typename Structure::node& nd, const cuboid& cub, unsigned depth, std::ptrdiff_t exclude_child_index = -1) const;


public:
	template<class... Args> tree_structure_memory_loader(Args&&... args) : structure_(std::forward<Args>(args)...) {
		position_path_.push_back({ structure_.root_node(), 0, structure_.root_cuboid() });
	}
		
	std::string loader_name() const override { return Structure::structure_name() + " Memory Simple Loader"; }
	
protected:
	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) override;
	
	std::size_t memory_size_() const override { return structure_.size() + position_path_.size()*sizeof(path_entry); }
	std::size_t file_size_() const override { return 0; }
	std::size_t total_points_() const override { return structure_.number_of_points(); }
};



template<class Structure>
std::size_t tree_structure_memory_loader<Structure>::extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
	bool outside_root = false;
	while(!outside_root && !position_path_.back().node_cuboid.in_range(req.position)) {
		if(position_path_.size() > 1) position_path_.pop_back();
		else outside_root = true;
	}
	if(! outside_root) while(! position_path_.back().node.is_leaf()) {
		const auto& e = position_path_.back();
		assert(e.node_cuboid.in_range(req.position));
		std::ptrdiff_t i = Structure::splitter::node_child_for_point(req.position, e.node_cuboid, e.node.get_points_information(), position_path_.size() - 1);
		
		const auto& child_node = e.node.child(i);
		auto child_cuboid = Structure::splitter::node_child_cuboid(i, e.node_cuboid, e.node.get_points_information(), position_path_.size() - 1);
		
		position_path_.push_back({ child_node, i, child_cuboid });
	}
	
	std::size_t c = 0;
	std::ptrdiff_t previous_i = -1;
	unsigned depth = position_path_.size();
	for(auto it = position_path_.rbegin(); c < capacity && it != position_path_.rend(); ++it) {
		c += extract_node_points_(points + c, capacity - c, req, it->node, it->node_cuboid, --depth, previous_i);
		previous_i = it->node_index;
	}
	
	return c;
}



template<class Structure>
std::size_t tree_structure_memory_loader<Structure>::extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const typename Structure::node& nd, const cuboid& cub, unsigned depth, std::ptrdiff_t exclude_child_index) const {
	auto action = action_for_node_(cub, nd.number_of_points(), nd.is_leaf(), req, Structure::levels);
	
	if(action == action_skip) {
		return 0;
		
	} else if(action == action_split) {
		struct child_entry { std::ptrdiff_t i; cuboid cub; };
		std::array<child_entry, Structure::number_of_node_children> children;
		
		for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) {
			auto child_cuboid = Structure::splitter::node_child_cuboid(i, cub, nd.get_points_information(), depth);
			children[i] = { i, child_cuboid };
		}
		
		std::sort(children.begin(), children.end(), [&](const child_entry& a, const child_entry& b)->bool {
			return (cuboid_distance_(req.position, a.cub) < cuboid_distance_(req.position, b.cub));
		});
		
		std::size_t c = 0;
		for(const child_entry& e : children) {
			if(e.i == exclude_child_index) continue;
			c += extract_node_points_(points + c, capacity - c, req, nd.child(e.i), e.cub, depth + 1);
		}
		return c;
		
		
	} else {
		std::ptrdiff_t lvl = action;
		if(lvl >= Structure::levels) lvl = Structure::levels - 1;
		
		if(! nd.number_of_points(0)) return 0;

		auto pts = nd.points_begin(lvl);
		auto pts_end = nd.points_end(lvl);
		std::size_t c = 0;
		for(auto pt = pts; pt != pts_end && c < capacity; ++pt) {
			*(points++) = *pt;
			++c;
		}
		return c;
	}
}

}

#endif
