#ifndef DYPC_TREE_STRUCTURE_MEMORY_OCCLUDING_LOADER_H_
#define DYPC_TREE_STRUCTURE_MEMORY_OCCLUDING_LOADER_H_

#include "tree_structure_loader.h"
#include "tree_structure.h"
#include "../../canvas/projection_canvas.h"
#include <utility>
#include <vector>
#include <stack>
#include <cassert>
#include <algorithm>
#include <array>
#include <string>
#include <iostream>
#include <cmath>
#include <memory>

namespace dypc {

template<class Structure>
class tree_structure_memory_occluding_loader : public tree_structure_loader {
private:
	struct path_entry {
		const typename Structure::node& node;
		std::ptrdiff_t node_index;
		cuboid node_cuboid;
	};

	std::unique_ptr<const Structure> structure_;
	std::vector<path_entry> position_path_;
	projection_canvas<bool, 400, 300> shadow_canvas_;

	std::size_t extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const typename Structure::node& nd, const cuboid& cub, unsigned depth, std::ptrdiff_t exclude_child_index = -1);


public:
	explicit tree_structure_memory_occluding_loader(const Structure* s) : structure_(s) {
		position_path_.push_back({ structure_->root_node(), 0, structure_->root_cuboid() });
	}
		
	std::string loader_name() const override { return Structure::structure_name() + " Memory Occluding Loader"; }
	
protected:
	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) override;
	
	std::size_t memory_size_() const override { return structure_->size() + position_path_.size()*sizeof(path_entry); }
	std::size_t file_size_() const override { return 0; }
	std::size_t total_points_() const override { return structure_->number_of_points(); }
};



template<class Structure>
std::size_t tree_structure_memory_occluding_loader<Structure>::extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
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
	
	shadow_canvas_.set_matrix(req.view_projection_matrix);
	shadow_canvas_.clear(false);
	
	std::size_t c = 0;
	std::ptrdiff_t previous_i = -1;
	unsigned depth = position_path_.size();
	for(auto it = position_path_.rbegin(); c < capacity && it != position_path_.rend(); ++it) {
		c += extract_node_points_(points + c, capacity - c, req, it->node, it->node_cuboid, --depth, previous_i);
		previous_i = it->node_index;
	}
	
	//shadow_canvas_.print();
	
	return c;
}



template<class Structure>
std::size_t tree_structure_memory_occluding_loader<Structure>::extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const typename Structure::node& nd, const cuboid& cub, unsigned depth, std::ptrdiff_t exclude_child_index) {
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
			glm::vec3 p(pt->x, pt->y, pt->z);
			float distance = glm::distance(req.position, p);
			if(shadow_canvas_.in_range(p) && shadow_canvas_[p]) continue;
									
			*(points++) = *pt;
			++c;
		}
		
		for(const triangle& tri : cub.hull_triangles()) {
			shadow_canvas_.fill_triangle(true, tri);
		}
		
		return c;
	}
}

}

#endif