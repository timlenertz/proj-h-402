#ifndef DYPC_TREE_STRUCTURE_MEMORY_SOURCE_H_
#define DYPC_TREE_STRUCTURE_MEMORY_SOURCE_H_

#include "tree_structure_source.h"
#include "../structure.h"
#include <unordered_map>
#include <utility>

namespace dypc {

/**
 * Tree structure source that reads from an in-memory tree structure.
 * Reads from tree_structure and tree_structure_node.
 * @tparam The tree_structure class.
 */
template<class Structure>
class tree_structure_memory_source : public tree_structure_source {
public:
	class node;

private:
	using structure_node = typename Structure::node;
	using map_t = std::unordered_map<const structure_node*, node>;

	std::unique_ptr<const Structure> structure_;
	node* root_node_;
	map_t map_;
	
	void add_node_to_map_(const structure_node& nd, const cuboid& cub, unsigned depth);
	
public:
	explicit tree_structure_memory_source(const Structure*);
		
	const node& root_node() const override { return *root_node_; }
	std::size_t number_of_nodes() const override { return structure_->number_of_nodes(); }
	std::size_t memory_size() const override { return structure_->size() + (map_.size() * sizeof(typename map_t::value_type)); }
	std::size_t rom_size() const override { return 0; }
};


template<class Structure>
class tree_structure_memory_source<Structure>::node : public tree_structure_source::node {
private:
	const structure_node& node_;
	const map_t& map_;
	cuboid cuboid_;
	unsigned depth_;

public:
	node(const map_t& mp, const structure_node& nd, const cuboid& cub, unsigned depth) : map_(mp), node_(nd), cuboid_(cub), depth_(depth) { }

	std::size_t number_of_points(std::ptrdiff_t lvl = 0) const override { return node_.number_of_points(); }
	
	std::size_t extract_points(point_buffer_t buffer, std::size_t capacity, std::ptrdiff_t lvl = 0) const override {
		auto pt_begin = node_.points_begin(lvl);
		auto pt_end = node_.points_end(lvl);
		std::size_t n = 0;
		for(auto it = pt_begin; (it != pt_end) && capacity; ++it, --capacity, ++n) *(buffer++) = *it;
		return n;
	}

	bool is_leaf() const override { return node_.is_leaf(); }
	bool has_child(std::ptrdiff_t i) const override { return ! is_leaf(); }
	const node& child(std::ptrdiff_t i) const override { return map_.at(& node_.child(i)); }
	
	std::ptrdiff_t child_for_point(glm::vec3 pt) const override;
	cuboid node_cuboid() const override { return cuboid_; }
};


template<class Structure>
tree_structure_memory_source<Structure>::tree_structure_memory_source(const Structure* str) :
tree_structure_source(Structure::levels, Structure::number_of_node_children), structure_(str) {
	auto& root_structure_node = str->root_node();
	add_node_to_map_(root_structure_node, str->root_cuboid(), 0);
	root_node_ = & map_.at(& root_structure_node);
}


template<class Structure>
void tree_structure_memory_source<Structure>::add_node_to_map_(const structure_node& nd, const cuboid& cub, unsigned depth) {
	map_.emplace(std::piecewise_construct, std::forward_as_tuple(&nd), std::forward_as_tuple(map_, nd, cub, depth));
	if(! nd.is_leaf()) for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) {
		cuboid child_cub = Structure::splitter::node_child_cuboid(i, cub, nd.get_points_information(), depth);
		add_node_to_map_(nd.child(i), child_cub, depth + 1);
	}
}


template<class Structure>
std::ptrdiff_t tree_structure_memory_source<Structure>::node::child_for_point(glm::vec3 pt) const {
	return Structure::splitter::node_child_for_point(pt, cuboid_, node_.get_points_information(), depth_);
}

}

#endif
