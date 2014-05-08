#ifndef DYPC_TREE_STRUCTURE_SPLITTER_H_
#define DYPC_TREE_STRUCTURE_SPLITTER_H_

#include "../../model/model.h"
#include "../../geometry/cuboid.h"
#include "../../point.h"
#include <vector>
#include <stdexcept>

namespace dypc {

/**
 * Splitter types that determines how a tree structure subdivides space.
 * All of the static members must be redefined in the subclass.
 */
class tree_structure_splitter {
public:
	tree_structure_splitter() = delete; // Cannot create instance

	static constexpr std::size_t number_of_node_children = 0; ///< Number of children each node has.

	struct node_points_information { }; ///< Information about points to be stored with node.

	/**
	 * Adjusts the cuboid for the root node.
	 * Takes cuboid enclosing model as input, so it can be adjusted for root node if necessary.
	 * For example Octree requires cubes. The resulting cuboid must include the input cuboid.
	 */
	static cuboid adjust_root_cuboid(const cuboid& cub) {
		return cub;
	}
	
	/**
	 * Get index of node child containing given point, for node.
	 * Must ensure that the cuboid node_child_cuboid for the returned index (and no others) always includes this point.
	 * Allows for a more efficient algorithm to be used, instead of calculating and testing all child cuboids.
	 * @param pt The point. Must be inside \a cub.
	 * @param cub Cuboid of this node.
	 * @param info Node points information for this node.
	 * @param depth Depth of this node.
	 */
	static std::ptrdiff_t node_child_for_point(const point& pt, const cuboid& cub, const node_points_information& info, unsigned depth) { throw std::logic_error("Not implemented"); }
	
	/**
	 * Get child node cuboid for node.
	 * @param i Child index.
	 * @param cub Cuboid of this node.
	 * @param info Node points information for this node.
	 * @param depth Depth of this node.
	 */
	static cuboid node_child_cuboid(const std::ptrdiff_t i, const cuboid& cub, const node_points_information& info, unsigned depth) { throw std::logic_error("Not implemented"); }
	
	
	/**
	 * Create node points information from point set.
	 * Iterates through the point set, in order to determine node points information. E.g. for Kdtree, this is
	 * the split plane at median.
	 * @param pt_begin Points begin iterator.
	 * @param pt_end Points end iterator.
	 * @param cub Cuboid of this node.
	 * @param depth Depth of this node.
	 */
	template<class Iterator>
	static node_points_information compute_node_points_information(Iterator pt_begin, Iterator pt_end, const cuboid& cub, unsigned depth) { return node_points_information(); }
};

}

#endif
