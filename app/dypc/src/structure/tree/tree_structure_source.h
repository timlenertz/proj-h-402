#ifndef DYPC_TREE_STRUCTURE_SOURCE_H_
#define DYPC_TREE_STRUCTURE_SOURCE_H_

#include "../../cuboid.h"
#include "../../point.h"

namespace dypc {

class tree_structure_source {
private:
	const std::size_t levels_;
	const std::size_t number_of_children_;

protected:
	tree_structure_source(std::size_t lvls, std::size_t nchl) : levels_(lvls), number_of_children_(nchl) { }

public:
	class node {
	public:
		virtual std::size_t number_of_points(std::ptrdiff_t lvl = 0) const = 0;
		virtual std::size_t extract_points(point_buffer_t buffer, std::size_t capacity, std::ptrdiff_t lvl = 0) const = 0;
		
		virtual bool is_leaf() const = 0;
		virtual bool has_child(std::ptrdiff_t i) const = 0;
		virtual const node& child(std::ptrdiff_t i) const = 0;
		
		virtual cuboid node_cuboid() const = 0;
	};
	
	std::size_t levels() const { return levels_; }
	std::size_t number_of_node_children() const { return number_of_children_; }
	
	virtual const node& root_node() const = 0;
	virtual std::size_t number_of_nodes() const = 0;
	std::size_t number_of_points() const { return this->root_node().number_of_points(); }
	
	virtual std::size_t memory_size() const = 0;
	virtual std::size_t rom_size() const = 0;
};

}

#endif
