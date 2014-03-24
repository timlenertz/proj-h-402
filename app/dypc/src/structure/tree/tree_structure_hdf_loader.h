#ifndef DYPC_TREE_STRUCTURE_HDF_LOADER_H_
#define DYPC_TREE_STRUCTURE_HDF_LOADER_H_

#include "tree_structure_loader.h"
#include "tree_structure_hdf_file.h"
#include "tree_structure.h"
#include <utility>
#include <memory>
#include <string>
#include <stack>
#include <deque>
#include <cassert>
#include <stdexcept>


namespace dypc {

template<class Structure>
class tree_structure_hdf_loader : public tree_structure_loader {
private:	
	using hdf_node = typename tree_structure_hdf_file<Structure>::hdf_node;

	struct path_entry {
		const hdf_node& node;
		std::ptrdiff_t node_index;
	};

	tree_structure_hdf_file<Structure> file_;
	
	std::vector<path_entry> position_path_;

	std::unique_ptr<hdf_node[]> nodes_;
	std::size_t number_of_nodes_ = 0;
	
	std::size_t extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const hdf_node& nd, unsigned depth, std::ptrdiff_t exclude_child_index = -1) const;

public:	
	explicit tree_structure_hdf_loader(const std::string& file);
	
	std::string loader_name() const override { return Structure::structure_name() + " HDF Ordered Loader"; }
	
protected:
	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) override;
	
	std::size_t memory_size_() const override {
		return number_of_nodes_ * sizeof(hdf_node);
	}
	
	std::size_t file_size_() const override {
		return file_.get_file_size();
	}
	
	std::size_t total_points_() const override {
		return file_.get_number_of_points();
	}
};


template<class Structure>
tree_structure_hdf_loader<Structure>::tree_structure_hdf_loader(const std::string& filename) : file_(filename) {	
	number_of_nodes_ = file_.get_number_of_nodes();
	nodes_.reset(new hdf_node [number_of_nodes_]);
	file_.read_nodes(nodes_.get(), number_of_nodes_);
	
	position_path_.push_back({ nodes_[0], 0 });
}


template<class Structure>
std::size_t tree_structure_hdf_loader<Structure>::extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
	bool outside_root = false;
	while(!outside_root && !position_path_.back().node.node_cuboid().in_range(req.position)) {
		if(position_path_.size() > 1) position_path_.pop_back();
		else outside_root = true;
	}
	if(! outside_root) while(! position_path_.back().node.is_leaf()) {
		const auto& e = position_path_.back();
		std::ptrdiff_t i = e.node.child_index_for_point(nodes_.get(), req.position);
		if(i != hdf_node::no_child_index) position_path_.push_back({ e.node.child_node(nodes_.get(), i), i });
		else break;
	}
	
	std::cout << position_path_.size() << std::endl;

	std::size_t c = 0;
	std::ptrdiff_t previous_i = -1;
	unsigned depth = position_path_.size();
	for(auto it = position_path_.rbegin(); c < capacity && it != position_path_.rend(); ++it) {
		c += extract_node_points_(points + c, capacity - c, req, it->node, --depth, previous_i);
		previous_i = it->node_index;
	}
	
	return c;
}



template<class Structure>
std::size_t tree_structure_hdf_loader<Structure>::extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const hdf_node& nd, unsigned depth, std::ptrdiff_t exclude_child_index) const {
	auto action = action_for_node_(nd.node_cuboid(), nd.data_length[0], nd.is_leaf(), req, Structure::levels);
		
	if(action == action_skip) {
		return 0;
		
	} else if(action == action_split) {
		struct child_entry { std::ptrdiff_t i; cuboid cub; };
		child_entry children[Structure::number_of_node_children];
		std::ptrdiff_t children_count = 0;
		
		for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) {
			if(nd.has_child(i) && (i != exclude_child_index)) children[children_count++] = { i, nd.child_node(nodes_.get(), i).node_cuboid() };
		}
		
		std::sort(children, children + children_count, [&](const child_entry& a, const child_entry& b)->bool {
			return (cuboid_distance_(req.position, a.cub) < cuboid_distance_(req.position, b.cub));
		});
		
		std::size_t c = 0;
		for(const child_entry* e = children; e != children + children_count; ++e) {
			c += extract_node_points_(points + c, capacity - c, req, nd.child_node(nodes_.get(), e->i), depth + 1);
		}
		return c;
		
	} else {
		hsize_t lvl = action;
		if(lvl >= Structure::levels) lvl = Structure::levels - 1;

		hsize_t read_number = nd.data_length[lvl];
		if(read_number > capacity) read_number = capacity;		
		
		file_.read_points(points, read_number, lvl, nd.data_start[lvl]);

		return read_number;
	}
}


}

#endif
