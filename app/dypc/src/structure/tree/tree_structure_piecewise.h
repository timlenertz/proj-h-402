#ifndef DYPC_TREE_STRUCTURE_PIECEWISE_H_
#define DYPC_TREE_STRUCTURE_PIECEWISE_H_

#include "tree_structure.h"
#include <vector>
#include <stack>
#include <iostream>

namespace dypc {

template<class Splitter, std::size_t Levels, class PointsContainer = std::vector<point>>
class tree_structure_piecewise : public tree_structure<Splitter, Levels, PointsContainer> {
private:
	using super = tree_structure<Splitter, Levels, PointsContainer>;

public:
	struct piece;

	class piece_node {
	private:
		const piece* piece_ = nullptr;
		piece_node* children_[Splitter::number_of_node_children];

	public:
		piece_node() { for(auto& c : children_) c = nullptr; }
		explicit piece_node(const piece& p) : piece_(&p) { }
		~piece_node() { if(! is_leaf()) for(piece_node* c : children_) if(c) delete c; }
	
		bool is_leaf() const { return piece_; }
		void set_piece(const piece& p) { piece_ = &p; }
		const piece& get_piece() const { assert(is_leaf()); return *piece_; }
		
		bool has_child(std::ptrdiff_t i) const {
			assert(! is_leaf() && i >= 0 && i < Splitter::number_of_node_children);
			return children_[i];
		}
		piece_node& child(std::ptrdiff_t i) {
			if(has_child(i)) return *children_[i];
			else return *(children_[i] = new piece_node);
		}
		const piece_node& child(std::ptrdiff_t i) const { assert(has_child(i)); return *children_[i]; }
		void delete_child(std::ptrdiff_t i) { assert(has_child(i)); delete children_[i]; }
	};

	struct piece {
		cuboid root_cuboid;
		std::size_t number_of_points;
		std::ptrdiff_t offset;
		unsigned depth;
		piece_node& node;
	};

private:
	const std::ptrdiff_t maximal_number_of_points_per_piece_;

	model& model_;
	cuboid model_root_cuboid_;
	std::vector<piece> pieces_;
	piece_node root_piece_node_;
	
public:
	tree_structure_piecewise(std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod);
	tree_structure_piecewise(std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod, std::ptrdiff_t maxnum);
	
	const piece_node& root_piece_node() const { return root_piece_node_; }
	const cuboid& root_piece_cuboid() const { return model_root_cuboid_; }
	
	void load_piece(const piece&);
	std::size_t number_of_pieces() const { return pieces_.size(); }
	
	std::size_t total_number_of_nodes_upper_bound() const;
	std::size_t total_number_of_points() const { return model_.number_of_points(); }
};



template<class Splitter, std::size_t Levels, class PointsContainer>
tree_structure_piecewise<Splitter, Levels, PointsContainer>::tree_structure_piecewise(std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod) : super(leaf_cap, mmfac, dmode, dmax, mod), model_(mod), model_root_cuboid_(Splitter::root_cuboid(mod)), maximal_number_of_points_per_piece_(0) {
	std::cout << "?!" << std::endl;
}



template<class Splitter, std::size_t Levels, class PointsContainer>
tree_structure_piecewise<Splitter, Levels, PointsContainer>::tree_structure_piecewise(std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod, std::ptrdiff_t maxnum) : super(leaf_cap, mmfac, dmode, dmax), model_(mod), model_root_cuboid_(Splitter::root_cuboid(mod)), maximal_number_of_points_per_piece_(maxnum) {	
	std::cout << maxnum << std::endl;
	
	std::stack<piece> pieces_stack;
	pieces_stack.push({ model_root_cuboid_, mod.number_of_points(), 0, 0, root_piece_node_ });
	
	while(! pieces_stack.empty()) {
		piece p = pieces_stack.top();
		pieces_stack.pop();
						
		if(maximal_number_of_points_per_piece_ && (p.number_of_points > maximal_number_of_points_per_piece_) && !p.node.is_leaf()) {
			std::size_t children_number_of_points[Splitter::number_of_node_children];
			for(auto& np : children_number_of_points) np = 0; 
			
			typename Splitter::node_points_information no_info;
			
			for(const point& pt : mod) {
				if(! p.root_cuboid.in_range(pt)) continue;
				std::ptrdiff_t i = Splitter::node_child_for_point(pt, p.root_cuboid, no_info, p.depth);
				++children_number_of_points[i];
			}
			
			std::ptrdiff_t offset = 0;
			for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) {
				std::size_t n = children_number_of_points[i];
				if(n == 0) continue;
								
				cuboid cub = Splitter::node_child_cuboid(i, p.root_cuboid, no_info, p.depth);
				pieces_stack.push({ cub, n, offset, p.depth + 1, p.node.child(i) });
				offset += n;
			}
			
		} else {
			pieces_.push_back(p);
			piece& copy = pieces_.back();
			copy.node.set_piece(copy);
		}
	}
	
	for(const auto& p : pieces_) std::cout << "piece " << p.offset << " nb:" << p.number_of_points << " depth: " << p.depth << std::endl;
}


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure_piecewise<Splitter, Levels, PointsContainer>::load_piece(const piece& p) {
	super::load_model_(model_, p.root_cuboid, p.depth);
}



template<class Splitter, std::size_t Levels, class PointsContainer>
std::size_t tree_structure_piecewise<Splitter, Levels, PointsContainer>::total_number_of_nodes_upper_bound() const {
	std::size_t level_nodes = total_number_of_points() / super::leaf_capacity_;
	std::size_t total = level_nodes;
	while(level_nodes > 1) {
		level_nodes = (level_nodes + 1) / Splitter::number_of_node_children;
		total += level_nodes;
	}
	return total;
}


}

#endif
