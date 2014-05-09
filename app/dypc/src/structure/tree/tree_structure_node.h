#ifndef DYPC_TREE_STRUCTURE_NODE_H_
#define DYPC_TREE_STRUCTURE_NODE_H_

#include "../../point.h"
#include <cassert>

#include "../../debug.h"
#include <array>
#include <utility>

namespace dypc {

/**
 * Node of tree structure.
 * @tparam Splitter Tree structure splitter which defines how space is subdivided into cuboids.
 * @tparam Levels Mipmap levels of downsampling to generate.
 * @tparam PointsContainer Container used to store arrays of points.
 */
template<class Splitter, std::size_t Levels, class PointsContainer>
class tree_structure_node {
private:
	using node_points_information = typename Splitter::node_points_information; ///< Type of node information.
	using point_iterator = typename PointsContainer::const_iterator; ///< Iterator for node points.

	/**
	 * Structure to represent point set of the node.
	 * Compact so as to minimize size of node object. Takes 3 forms during execution of add_root_node_points:
	 * 1. Pointer to an internal allocated buffer
	 * 2. Offset in final ordered points array
	 * 3. Iterator in final ordered points array
	 */
	struct point_set {
		union {
			point* points_buffer; ///< Pointer to internal allocated buffer.
			std::ptrdiff_t points_offset; ///< Offset in ordered points array.
			point_iterator points_iterator; ///< Iterator to ordered points array.
		};
		std::size_t number_of_points = 0; ///< Number of points in node.
		
		point_set() {
			// Start at stage 1, with no allocated buffer
			points_buffer = nullptr;
		}
		
		/**
		 * Add point to internal buffer.
		 * For use at stage 1.
		 * @param pt The point to add.
		 * @param leaf_capacity Capacity, becomes size of internal buffer.
		 */
		void add_point_to_buffer(const point& pt, std::size_t leaf_capacity) {
			// Allocate buffer if not yet done, and add the point
			if(! points_buffer) points_buffer = new point [leaf_capacity];
			assert(number_of_points < leaf_capacity);
			points_buffer[number_of_points++] = pt;
		}
	};

	std::array<tree_structure_node*, Splitter::number_of_node_children> children_; ///< Child nodes, either all set or all null.
	std::array<point_set, Levels> point_sets_; ///< Point sets for different downsampling levels.
	node_points_information points_information_; ///< Node information.
	
	
	
	/**
	 * Recursively add non-downsampled points to tree nodes and build tree.
	 * Adds points into internal buffers.
	 * @see add_root_node_points
	 * @param all_points Unordered full points set to add.
	 * @param cub Cuboid of this node.
	 * @param depth Depth of this node.
	 * @param leaf_capacity The leaf capacity.
	 * @param pr Process handle to track progress of adding points.
	 */
	void add_points_with_information_(PointsContainer& all_points, const cuboid& cub, unsigned depth, std::size_t leaf_capacity, progress_handle& pr);

	/**
	 * Add of downsampled level into tree.
	 * Adds point into internal buffer of a leaf. Unlike add_points_with_information_, it does not further split the tree,
	 * as downsampled point sets contain fewer points. However, it may occur that more points fall into one leaf than before
	 * downsampling. False is returned if trying to add too many points.
	 * @see add_root_node_points
	 * @param lvl Downsampling level, must be greater than 0.
	 * @param pt Point to add.
	 * @param cub Cuboid of this node.
	 * @param depth Depth of this node.
	 * @param leaf_capacity The leaf_capacity.
	 * @return True on success, false if beyond leaf capacity.
	 */
	bool add_higher_level_point_(std::ptrdiff_t lvl, const point& pt, const cuboid& cub, unsigned depth, std::size_t leaf_capacity);
	
	/**
	 * Recursively move points from interal buffers into output array.
	 * Also determines number of points in non-leaf nodes (point_set.number_of_points). \a output_points will contain the points from the nodes, concatenated in depth-first order. Also deallocates internal buffers, and stores corresponding offset in ¹a output_points instead.
	 * @see add_root_node_points.
	 * @param output_points Output points array.
	 * @param lvl Downsampling point set level.
	 */
	std::size_t move_out_points_(PointsContainer& output_points, unsigned lvl);
	
	/**
	 * Recursively replaces the stored offsets from \a output_points by iterators.
	 * @see add_root_node_points
	 * @param output_points Output points array.
	 * @param lvl Downsampling point set level.
	 */
	void finalize_move_out_(const PointsContainer& output_points, unsigned lvl);

	
public:
	/**
	 * Create empty tree structure node.
	 */
	tree_structure_node() {
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) children_[i] = nullptr;
	}
	
	~tree_structure_node() {
		clear();
	}
	
	/**
	 * Recursively deletes node and its children.
	 */
	void clear();
		
	tree_structure_node(const tree_structure_node&) = delete; ///< Disallow copy construction.
	
	/**
	 * Move construct tree structure node.
	 * Invalidates this object.
	 */
	tree_structure_node(tree_structure_node&&);
	
	const tree_structure_node& child(std::ptrdiff_t i) const { assert(i >= 0 && i < Splitter::number_of_node_children && children_[i]); return *children_[i]; } ///< Get child node.
	tree_structure_node& child(std::ptrdiff_t i) { assert(i >= 0 && i < Splitter::number_of_node_children && children_[i]); return *children_[i]; } ///< Get child node.
	
	point_iterator points_begin(std::ptrdiff_t lvl = 0) const { assert(lvl >= 0 && lvl < Levels); return point_sets_[lvl].points_iterator; } ///< Get iterator to start of point set. Undefined when points at \a lvl are unloaded.
	point_iterator points_end(std::ptrdiff_t lvl = 0) const { assert(lvl >= 0 && lvl < Levels); return point_sets_[lvl].points_iterator + point_sets_[lvl].number_of_points; } ///< Get iterator to end of points set. Undefined when points at \a lvl are unloaded.

	/**
	 * Add points into node.
	 * Must be called only on root node of tree, and only once for each level. Must be called for level 0 (not downsampled) prior to higher levels.
	 * @param lvl Downsampling level, 0 means no downsampling.
	 * @param all_points Unordered array of points to add to node. Is cleared during process, in order to save memory.
	 * @param output_points Output array that will store the points in tree nodes order.
	 * @param cub Cuboid for this node.
	 * @param leaf_capacity Leaf nodes capacity for the tree.
	 * @param pr Progress handle to track progress during filling process.
	 */
	void add_root_node_points(std::ptrdiff_t lvl, PointsContainer& all_points, PointsContainer& output_points, const cuboid& cub, std::size_t leaf_capacity, progress_handle& pr);
		
	/**
	 * Get number of nodes in tree starting from this node.
	 */
	std::size_t number_of_nodes_in_branch() const {
		std::size_t n = 1;
		if(! is_leaf()) for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) n += children_[i]->number_of_nodes_in_branch();
		return n;
	}

	std::size_t number_of_points(std::ptrdiff_t lvl = 0) const { assert(lvl >= 0 && lvl < Levels); return point_sets_[lvl].number_of_points; } ///< Get number of points in node for given downsampling level. Remains set after unloading level \a lvl.
	std::size_t size() const; ///< Get approximate memory usage of node.
	
	bool is_empty() const { return (number_of_points() == 0); } ///< Check if the node is empty.
	
	bool is_leaf() const { return (children_[0] == nullptr); } ///< Check if the node is a leaf.
	const node_points_information& get_points_information() const { return points_information_; } ///< Get node points information.
};



template<class Splitter, std::size_t Levels, class PointsContainer>
tree_structure_node<Splitter, Levels, PointsContainer>::tree_structure_node(tree_structure_node&& n) : children_(n.children_), point_sets_(n.point_sets_), points_information_(n.points_information_) {
	n.children_.fill(nullptr);
	n.point_sets_.fill(point_set());
}


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure_node<Splitter, Levels, PointsContainer>::clear() {
	if(! children_[0]) return;
	for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) delete children_[i];
}


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure_node<Splitter, Levels, PointsContainer>::add_root_node_points(std::ptrdiff_t lvl, PointsContainer& output_points, PointsContainer& all_points, const cuboid& cub, std::size_t leaf_capacity, progress_handle& pr) {
	assert(lvl >= 0 && lvl < Levels);
	assert(number_of_points(lvl) == 0); // Must be called once only for each level
	if(lvl == 0) {
		// Adding non-downsampled points. Must always be done first. Since this point set is the largest in size, it is used to determine node information and recursively split the tree
		
		// Stage 1: Generate node points information from point set, split node if necessary (if number of points larger than leaf capacity), and add points into internal buffers of node and child nodes.
		add_points_with_information_(all_points, cub, 0, leaf_capacity, pr);
	} else {
		assert(! is_empty()); // Must already have been called for level 0
		// Adding downsampled points. The tree structure and node informations have already been generated before.
		for(const point& pt : all_points) add_higher_level_point_(lvl, pt, cub, 0, leaf_capacity);
	}
	
	// Clear all_points to free memory
	all_points.clear(); all_points.shrink_to_fit();
	
	// Stage 2: All points have been added into node and children's internal buffers. The output points array is going to be ordered at a concatenation of the tree's node points in depth-first order. That way, points belonging to one node (at any depth) are always in one interval.
	// Internal buffers are deallocated, and instead the offset in the output array is stored in nodes.
	move_out_points_(output_points, lvl);
	
	// Stage 3: All points have been put into output array. Since the output array will no longer be changed, it is now safe to store iterators to it (For deque, the iterators would otherwise be invalidated when the array is changed). The final stage replaces the stored offsets by iterators, so that faster access is possible.
	finalize_move_out_(output_points, lvl);
}


template<class Splitter, std::size_t Levels, class PointsContainer>
bool tree_structure_node<Splitter, Levels, PointsContainer>::add_higher_level_point_(std::ptrdiff_t lvl, const point& pt, const cuboid& cub, unsigned depth, std::size_t leaf_capacity) {
	assert(lvl > 0);
	
	auto& set = point_sets_[lvl];
	if(! is_leaf()) {
		// This is not a leaf; add point to appropriate child
		auto child_index = Splitter::node_child_for_point(pt, cub, points_information_, depth);
		auto child_cuboid = Splitter::node_child_cuboid(child_index, cub, points_information_, depth);
		return children_[child_index]->add_higher_level_point_(lvl, pt, child_cuboid, depth + 1, leaf_capacity);
	} else if(set.number_of_points < leaf_capacity) {
		// This is a leaf; add point to internal buffer
		assert(cub.in_range(pt));
		set.add_point_to_buffer(pt, leaf_capacity);
		return true;
	} else {
		// Error: Tried to add more points into node than there were for non-downsampled point set
		return false;
	}
}


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure_node<Splitter, Levels, PointsContainer>::add_points_with_information_(PointsContainer& all_points, const cuboid& cub, unsigned depth, std::size_t leaf_capacity, progress_handle& pr) {
	assert(is_leaf());
	
	// Compute node points information. E.g. for kdTree, this is the split plane calculated from the median of a coordinate of the points
	points_information_ = Splitter::compute_node_points_information(all_points.begin(), all_points.end(), cub, depth);
		
	if(all_points.size() <= leaf_capacity) {
		// The points to add fit into leaf capacity; add them all to internal buffer of this node
		auto& set = point_sets_[0];
		for(const point& pt : all_points) { assert(cub.in_range(pt)); set.add_point_to_buffer(pt, leaf_capacity); }
		pr.increment(all_points.size());
		
	} else {
		// There are more points than would fit into one leaf.
		// Generate child nodes
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) children_[i] = new tree_structure_node;
		
		// Distribute points into subsets for the child nodes
		PointsContainer child_point_sets[Splitter::number_of_node_children];
		for(const point& pt : all_points) {
			std::ptrdiff_t i = Splitter::node_child_for_point(pt, cub, points_information_, depth);
			child_point_sets[i].push_back(pt);
		}
		
		// Recursively call with child node point sets
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) {
			cuboid child_cuboid = Splitter::node_child_cuboid(i, cub, points_information_, depth);
			auto& set = child_point_sets[i];
			child(i).add_points_with_information_(set, child_cuboid, depth + 1, leaf_capacity, pr);
			set.clear(); set.shrink_to_fit(); // Free it now to save some memory
		}
	}
}


template<class Splitter, std::size_t Levels, class PointsContainer>
std::size_t tree_structure_node<Splitter, Levels, PointsContainer>::move_out_points_(PointsContainer& output_points, unsigned lvl) {
	// Do depth-first descent of tree, and copy points from internal buffers to output_points.
	
	auto& set = point_sets_[lvl];
	
	std::ptrdiff_t output_offset = output_points.size(); // Offset of first point that will be added from this node
	
	if(is_leaf()) {
		if(! set.points_buffer) return 0;
		// Copy points from internal buffer
		const point* points_end = set.points_buffer + set.number_of_points;
		for(const point* pt = set.points_buffer; pt < points_end; ++pt) output_points.push_back(*pt);
		// Free internal buffer
		delete[] set.points_buffer;
	} else {
		// Recursive call
		std::size_t count = 0;
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) count += children_[i]->move_out_points_(output_points, lvl);
		set.number_of_points = count; // Sum of points in this node
	}
	
	// Save offset
	set.points_offset = output_offset;
	return set.number_of_points;
}


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure_node<Splitter, Levels, PointsContainer>::finalize_move_out_(const PointsContainer& output_points, unsigned lvl) {
	auto& set = this->point_sets_[lvl];
	// Replace offset by iterator
	set.points_iterator = output_points.begin() + set.points_offset;
	if(! is_leaf()) {
		// Recursive call into child nodes
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) children_[i]->finalize_move_out_(output_points, lvl);
	}
}


template<class Splitter, std::size_t Levels, class PointsContainer>
std::size_t tree_structure_node<Splitter, Levels, PointsContainer>::size() const {
	std::size_t sz = sizeof(tree_structure_node);
	if(! is_leaf()) {
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) sz += children_[i]->size();
	}
	return sz;
}




}

#endif
