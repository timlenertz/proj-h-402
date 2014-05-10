#ifndef DYPC_TREE_STRUCTURE_HDF_SOURCE_H_
#define DYPC_TREE_STRUCTURE_HDF_SOURCE_H_

#include "../tree_structure_source.h"
#include "tree_structure_hdf_file.h"
#include <memory>
#include <vector>
#include <string>

namespace dypc {

/**
 * Tree structure source that reads HDF file.
 */
template<std::size_t Levels, std::size_t NumberOfChildren>
class tree_structure_hdf_source : public tree_structure_source {
public:
	class node;
	
private:
	using file_t = tree_structure_hdf_file<Levels, NumberOfChildren>;
	using hdf_node = typename file_t::hdf_node;

	file_t file_;
	std::vector<node> nodes_;

public:	
	explicit tree_structure_hdf_source(const std::string& filepath);
		
	const node& root_node() const override { return nodes_[0]; }
	std::size_t number_of_nodes() const override { return nodes_.size(); }
	std::size_t memory_size() const override { return nodes_.size() * sizeof(node); }
	std::size_t rom_size() const override { return file_.get_file_size(); }
};


template<std::size_t Levels, std::size_t NumberOfChildren>
class tree_structure_hdf_source<Levels, NumberOfChildren>::node : public tree_structure_source::node {
private:
	tree_structure_hdf_source& source_;
	const hdf_node node_;

public:
	node(tree_structure_hdf_source& src, const hdf_node& nd) : source_(src), node_(nd) { }

	tree_structure_hdf_source& source() const { return source_; }

	std::size_t number_of_points(std::ptrdiff_t lvl = 0) const override { return node_.data_length[lvl]; }
	
	std::size_t extract_points(point_buffer_t buffer, std::size_t capacity, std::ptrdiff_t lvl = 0) const override {
		std::size_t n = number_of_points(lvl);
		if(n > capacity) n = capacity;
		source_.file_.read_points(buffer, n, lvl, node_.data_start[lvl]);
		return n;
	}

	bool is_leaf() const override { return node_.is_leaf(); }
	bool has_child(std::ptrdiff_t i) const override { return node_.has_child(i); }
	const node& child(std::ptrdiff_t i) const override { assert(has_child(i)); return source_.nodes_[node_.children[i]]; }
	
	std::ptrdiff_t child_for_point(glm::vec3 pt) const override;
	cuboid node_cuboid() const override { return node_.node_cuboid(); }
};


template<std::size_t Levels, std::size_t NumberOfChildren>
tree_structure_hdf_source<Levels, NumberOfChildren>::tree_structure_hdf_source(const std::string& filepath) :
tree_structure_source(Levels, NumberOfChildren), file_(filepath) {
	std::size_t number_of_nodes = file_.get_number_of_nodes();
	std::unique_ptr<hdf_node[]> hdf_nodes(new hdf_node [number_of_nodes]);
	file_.read_nodes(hdf_nodes.get(), number_of_nodes);
	
	const hdf_node* end = hdf_nodes.get() + number_of_nodes;
	for(const hdf_node* it = hdf_nodes.get(); it != end; ++it) nodes_.emplace_back(*this, *it);
}


template<std::size_t Levels, std::size_t NumberOfChildren>
std::ptrdiff_t tree_structure_hdf_source<Levels, NumberOfChildren>::node::child_for_point(glm::vec3 pt) const {
	for(std::ptrdiff_t i = 0; i < NumberOfChildren; ++i) {
		if(has_child(i) && child(i).node_cuboid().in_range(pt)) return i;
	}
	return no_child_index;
}




}

#endif
