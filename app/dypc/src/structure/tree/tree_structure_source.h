#ifndef DYPC_TREE_STRUCTURE_SOURCE_H_
#define DYPC_TREE_STRUCTURE_SOURCE_H_

#include "../../geometry/cuboid.h"
#include "../../point.h"

namespace dypc {

/**
 * Source from which data of a tree structure is accessed by loader.
 * This is an abstract base class. Subclasses for HDF and the in-memore object are implemented.
 */
class tree_structure_source {
private:
	const std::size_t levels_; ///< Downsampling levels of the structure.
	const std::size_t number_of_children_; ///< Number of children per node in the tree structure.

protected:
	tree_structure_source(std::size_t lvls, std::size_t nchl) : levels_(lvls), number_of_children_(nchl) { }

public:
	/**
	 * Virtual node in the tree structure.
	 * Polymorphic object implemented in subclasses by which the loader traverses the tree.
	 * Represents a node in the tree. Does not have the same constraints as the structure's tree_structure_node.
	 * The tree structure can be more heterogenous (e.g. mix of different spliitters), and not all child nodes
	 * need to be defined.
	 */
	class node {
	public:
		static constexpr std::ptrdiff_t no_child_index = -1; ///< Placeholder value.
	
		virtual std::size_t number_of_points(std::ptrdiff_t lvl = 0) const = 0; ///< Get number of points in the nude.
		virtual std::size_t extract_points(point_buffer_t buffer, std::size_t capacity, std::ptrdiff_t lvl = 0) const = 0; ///< Extract points from node.
		
		virtual bool is_leaf() const = 0; ///< Check whether node is a leaf.
		virtual bool has_child(std::ptrdiff_t i) const = 0; ///< Check whether node has given child.
		virtual const node& child(std::ptrdiff_t i) const = 0; ///< Get child node.
		
		virtual std::ptrdiff_t child_for_point(glm::vec3 pt) const = 0; ///< Get child node index for given position. Returns \a no_child_index when none.
		virtual cuboid node_cuboid() const = 0; ///< Get cuboid for this node.
	};
	
	std::size_t levels() const { return levels_; } ///< Get number of downsampling levels.
	std::size_t number_of_node_children() const { return number_of_children_; } ///< Get number of children per node.
	
	virtual const node& root_node() const = 0; ///< Get polymorphic root node object.
	virtual std::size_t number_of_nodes() const = 0; ///< Get total number of nodes.
	std::size_t number_of_points() const { return this->root_node().number_of_points(); } ///< Get total number of points.
	
	virtual std::size_t memory_size() const = 0; ///< Get structure's size in RAM.
	virtual std::size_t rom_size() const = 0; ///< Get structure's size in ROM. 0 if loading from memory source.
};

}

#endif
