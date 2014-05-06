#ifndef DYPC_TREE_STRUCTURE_PIECEWISE_H_
#define DYPC_TREE_STRUCTURE_PIECEWISE_H_

#include "tree_structure.h"
#include "kdtree_half/kdtree_half_structure_splitter.h"
#include <vector>
#include <cmath>

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
	
	void load_piece(const piece_node&);
	
	std::size_t total_number_of_points_upper_bound(std::ptrdiff_t lvl) const;
};


template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
class tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::piece_node {
private:
	piece_node* children_[PiecesSplitter::number_of_node_children];
	const cuboid cuboid_;
	const std::size_t depth_ = 0;
	std::size_t number_of_points_ = 0;
	std::ptrdiff_t data_offset_ = -1;
	
	void delete_children_();

public:
	piece_node(const cuboid& cub, std::size_t depth) : cuboid_(cub), depth_(depth) {
		for(auto& child : children_) child = nullptr;
	}
	
	~piece_node() {
		delete_children_();
	}
	
	piece_node(const piece_node&) = delete;
	piece_node& operator=(const piece_node&) = delete;
	
	const cuboid& get_cuboid() const { return cuboid_; }
	std::size_t get_depth() const { return depth_; }
	std::size_t get_number_of_points() const { return number_of_points_; }
	std::ptrdiff_t get_data_offset() const { return data_offset_; }
	
	bool has_child(std::ptrdiff_t i) const { assert(i >= 0 && i < PiecesSplitter::number_of_node_children); return (children_[i] != nullptr); }
	piece_node& child(std::ptrdiff_t i) { assert(has_child(i)); return *children_[i]; }
	const piece_node& child(std::ptrdiff_t i) const { assert(has_child(i)); return *children_[i]; }
	
	bool is_leaf() const {
		for(auto child : children_) if(child) return false;
		return true;
	}
	
	bool contains_point(glm::vec3 pt) const {
		return cuboid_.in_range(pt);
	}
	
	void initialize_tree(std::size_t max_depth);
	
	bool count_point(glm::vec3 pt, std::ptrdiff_t maxnum);
	
	std::ptrdiff_t make_pieces(std::ptrdiff_t maxnum, std::ptrdiff_t offset = 0);
};




template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::tree_structure_piecewise(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, std::ptrdiff_t maxnum) :
super(leaf_cap, dmin, damount, dmode, mod, super::no_load),
root_piece_node_(mod.enclosing_cuboid(), 0) {
	maxnum = 70000;
	
	std::size_t max_depth = expected_maximal_pieces_depth_;
	bool repeat = true;
	while(repeat) {
		repeat = false;
		root_piece_node_.initialize_tree(max_depth);
		progress(mod.number_of_points(), "Counting points in child pieces (max depth " + std::to_string(max_depth) + ")...", [&](progress_handle& pr) {
			for(const point& pt : mod) {
				pr.increment();
				bool ok = root_piece_node_.count_point(pt, maxnum);
				if(! ok) { repeat = true; ++max_depth; break; }
			}
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
std::size_t tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::total_number_of_points_upper_bound(std::ptrdiff_t lvl) const {
	std::size_t n = super::total_number_of_points();
	float ratio = std::pow(super::mipmap_factor_, lvl);
	return ratio * n;
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
	delete_children_();
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
	assert(contains_point(pt));
	assert(data_offset_ == -1);
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
std::ptrdiff_t tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::piece_node::make_pieces(std::ptrdiff_t maxnum, std::ptrdiff_t offset) {
	assert(!is_leaf() || number_of_points_ <= maxnum);
	data_offset_ = offset;
	if(number_of_points_ <= maxnum) {
		delete_children_();
		return offset + number_of_points_;
	} else {
		for(auto child : children_) if(child) offset = child->make_pieces(maxnum, offset);
		return offset;
	}
}


}

#endif
