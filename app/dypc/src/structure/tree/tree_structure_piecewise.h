#ifndef DYPC_TREE_STRUCTURE_PIECEWISE_H_
#define DYPC_TREE_STRUCTURE_PIECEWISE_H_

#include "tree_structure.h"
#include "kdtree_half/kdtree_half_structure_splitter.h"
#include <vector>
#include <cmath>
#include <iostream>

namespace dypc {

/**
 * Tree structure which is subdivided into pieces.
 * The model is first recursively subdivided into pieces using the splitter PiecesSplitter. (can be different than the tree splitter).
 * The leaves for this first tree have a (large) maximal capacity. Only one node of that tree is always loaded at a time,
 * and that piece is again recursively subdivided like a normal tree structure. The goal is to reduce memory usage and/or allow parallelism
 * during creation of structure file, by loading only one piece at a time.
 * The double tree structure allows for the structure to be stored into a file and as a single large tree.
 * @tparam Splitter Tree structure splitter which defines how the loaded piece is subdivided into cuboids.
 * @tparam Levels Mipmap levels of downsampling to generate.
 * @tparam PointsContainer Container used to store arrays of points.
 * @tparam PiecesSplitter Tree structure splitter which defines how the model is subdivided into piece nodes.
 */
template<class Splitter, std::size_t Levels, class PointsContainer = std::vector<point>, class PiecesSplitter = kdtree_half_structure_splitter>
class tree_structure_piecewise : public tree_structure<Splitter, Levels, PointsContainer> {
	static_assert(Splitter::number_of_node_children >= PiecesSplitter::number_of_node_children, "Pieces splitter cannot have more children than nodes splitter");
	
private:
	using super = tree_structure<Splitter, Levels, PointsContainer>;
	static constexpr std::size_t expected_maximal_pieces_depth_ = 3; ///< Expected maximal depth for pieces tree.

public:
	class piece_node;

private:
	piece_node root_piece_node_; ///< The root piece node.
	
public:
	/**
	 * Create tree structure in one single piece.
	 * Loads all downsampled point sets.
	 */
	tree_structure_piecewise(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod) : 
	super(leaf_cap, dmin, damount, dmode, mod, true), root_piece_node_(cuboid(), 0) { }
	
	/**
	 * Create tree structure, split into pieces of at most \a maxnum points.
	 * Initially no piece is loaded.
	 * @param maxnum Maximal number of points per piece.
	 */
	tree_structure_piecewise(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, std::ptrdiff_t maxnum);
	
	/**
	 * Get root node of pieces tree.
	 * @pre The piecewise tree structure 
	 */
	const piece_node& root_piece_node() const {
		assert(root_piece_node_.get_number_of_points());
		return root_piece_node_;
	}
	
	/**
	 * Load given piece in tree structure.
	 * Load the portion of the model contained within that piece's cuboid into the tree structure.
	 * This builds the tree and generated the downsampled sets, just like if a new tree_structure object were
	 * to be created for that point set.
	 * @param nd The piece node to load.
	 */
	void load_piece(const piece_node& nd);
	
	
	tree_structure<Splitter, Levels, PointsContainer> load_and_export_piece(const piece_node& nd) const;
	
	/**
	 * Get total number of points in structure.
	 * Sum of points in all pieces, for given downsampling level. Will give exact value, without loading pieces.
	 * @param lvl Downsampling level.
	 */
	std::size_t total_number_of_points(std::ptrdiff_t lvl = 0) const;
	
	/**
	 * Get number of points in piece.
	 * Will give exact value, without loading piece.
	 * @param lvl Downsampling level.
	 */
	std::size_t piece_number_of_points(const piece_node& nd, std::ptrdiff_t lvl = 0) const;
};


/**
 * Node in the pieces tree.
 */
template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
class tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::piece_node {
private:
	piece_node* children_[PiecesSplitter::number_of_node_children]; ///< Child nodes.
	const cuboid cuboid_; ///< Cuboid for this piece node.
	const std::size_t depth_ = 0; ///< Depth of this piece node.
	std::size_t number_of_points_ = 0; ///< Number of points belonging into this piece node.
	int id_ = -1; ///< ID for the piece in the finalized tree. -1 when not yet defined.
	
	void delete_children_(); ///< Delete any children.

public:
	/**
	 * Create child node with no children.
	 * @param cub Cuboid for this node.
	 * @param depth Depth of this node in the tree.
	 */
	piece_node(const cuboid& cub, std::size_t depth) : cuboid_(cub), depth_(depth) {
		for(auto& child : children_) child = nullptr;
	}
	
	~piece_node() {
		delete_children_();
	}
	
	piece_node(const piece_node&) = delete;
	piece_node& operator=(const piece_node&) = delete;
	
	const cuboid& get_cuboid() const { return cuboid_; } ///< Get cuboid of this node.
	std::size_t get_depth() const { return depth_; } ///< Get depth of this node.
	std::size_t get_number_of_points() const { return number_of_points_; } ///< Get number of points of this node.
	int get_id() const { assert(id_ != -1); return id_; }
	
	bool has_child(std::ptrdiff_t i) const { assert(i >= 0 && i < PiecesSplitter::number_of_node_children); return (children_[i] != nullptr); } ///< Check if a given child exists.
	piece_node& child(std::ptrdiff_t i) { assert(has_child(i)); return *children_[i]; } ///< Get child of node.
	const piece_node& child(std::ptrdiff_t i) const { assert(has_child(i)); return *children_[i]; } ///< Get child of node.

	bool contains_point(glm::vec3 pt) const { return cuboid_.in_range(pt); } ///< Check if node cuboid contains a given point.
	
	/**
	 * Check if node has no children.
	 */
	bool is_leaf() const {
		for(auto child : children_) if(child) return false;
		return true;
	}
	
	/**
	 * Initialize full tree down to given depth.
	 * Creates all child node with cuboids given by PieceSplitter, down to given maximal depth.
	 * Removes existing tree if any, and resets number of points to zero.
	 * @param max_depth Depth ot tree to create.
	 */
	void initialize_tree(std::size_t max_depth);
	
	/**
	 * Recursively increment point count.
	 * Increments the number of points in this node, and recursively of all child nodes that contain this point.
	 * (i.e. one branch in the tree). Returns false, if this puts the number of points of a leaf to more than \a maxnum.
	 * Can be called only after initialize_tree; i.e. when either all of no child nodes exist for and node.
	 * @param pt Point to count. Must be inside this node's cuboid.
	 * @param maxnum Maximal number of points that can be in a leaf.
	 * @return Returns false when too many points in a leaf.
	 */
	bool count_point(glm::vec3 pt, std::ptrdiff_t maxnum);
	
	/**
	 * Finalize tree and define data offsets.
	 * Removes as many nodes as possible from the tree, such that all child nodes in the remaining tree have a number of
	 * points less or equal to \a maxnum, yielding the final pieces tree.
	 * count_point must never have returned false
	 * @param maxnum 
	 */
	void make_pieces(std::ptrdiff_t maxnum, int& id_counter);
	
	void make_pieces(std::ptrdiff_t maxnum) {
		int id_counter = 0;
		make_pieces(maxnum, id_counter);
	}
};




template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::tree_structure_piecewise(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, std::ptrdiff_t maxnum) :
super(leaf_cap, dmin, damount, dmode, mod, true, super::no_load),
root_piece_node_(mod.enclosing_cuboid(), 0) {	
	std::size_t max_depth = expected_maximal_pieces_depth_;
	bool repeat = true;
	while(repeat) {
		repeat = false;
		root_piece_node_.initialize_tree(max_depth);
		progress_foreach(mod, "Counting points in child pieces (max depth " + std::to_string(max_depth) + ")...", [&](const point& pt) {
			if(repeat) return; // no easy way to exit right now
			bool ok = root_piece_node_.count_point(pt, maxnum);
			if(! ok) { repeat = true; ++max_depth; }
		});
	}

	root_piece_node_.make_pieces(maxnum);
}


template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
void tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::load_piece(const piece_node& p) {
	super::load_(p.get_cuboid());
	assert(super::number_of_points() == p.get_number_of_points());
}


template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
tree_structure<Splitter, Levels, PointsContainer> tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::load_and_export_piece(const piece_node& nd) const {
	return tree_structure<Splitter, Levels, PointsContainer>(
		super::leaf_capacity_,
		super::downsampling_minimum_,
		super::downsampling_amount_,
		super::downsampling_mode_,
		super::model_,
		nd.get_cuboid(),
		false,
		true
	);
}


template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
std::size_t tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::total_number_of_points(std::ptrdiff_t lvl) const {
	std::size_t n = super::total_number_of_points();
	return super::downsampling_number_of_points_exact(n, lvl);
}


template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
std::size_t tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::piece_number_of_points(const piece_node& nd, std::ptrdiff_t lvl) const {
	return super::downsampling_number_of_points_exact(nd.get_number_of_points(), lvl);
}


template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
void tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::piece_node::delete_children_() {
	for(auto& child : children_) if(child) {
		delete child;
		child = nullptr;
	}
}


template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
void tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::piece_node::initialize_tree(std::size_t max_depth) {
	delete_children_(); number_of_points_ = 0;
	if(max_depth == 0) return;
	typename PiecesSplitter::node_points_information no_info;
	for(std::ptrdiff_t i = 0; i < PiecesSplitter::number_of_node_children; ++i) {
		cuboid child_cub = PiecesSplitter::node_child_cuboid(i, cuboid_, no_info, depth_);
		children_[i] = new piece_node(child_cub, depth_ + 1);
		children_[i]->initialize_tree(max_depth - 1);
	}
}
	

template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
bool tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::piece_node::count_point(glm::vec3 pt, std::ptrdiff_t maxnum) {
	//assert(contains_point(pt));
	typename PiecesSplitter::node_points_information no_info;
	++number_of_points_;
	if(! children_[0]) {
		return (number_of_points_ <= maxnum);
	} else {
		auto i = PiecesSplitter::node_child_for_point(pt, cuboid_, no_info, depth_);
		return children_[i]->count_point(pt, maxnum);
	}
}


template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
void tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::piece_node::make_pieces(std::ptrdiff_t maxnum, int& id_counter) {	
	assert(!is_leaf() || number_of_points_ <= maxnum);
	if(number_of_points_ <= maxnum) {
		delete_children_();
		id_ = ++id_counter;
	} else {
		for(auto child : children_) if(child) child->make_pieces(maxnum, id_counter);
	}
}


}

#endif
