#ifndef DYPC_TREE_STRUCTURE_PIECEWISE_H_
#define DYPC_TREE_STRUCTURE_PIECEWISE_H_

#include "tree_structure.h"
#include <vector>
#include <stack>
#include <iostream>

namespace dypc {

template<class Splitter, std::size_t Levels, class PointsContainer = std::deque<point>>
class tree_structure_piecewise : public tree_structure<Splitter, Levels, PointsContainer> {
private:
	using super = tree_structure<Splitter, Levels, PointsContainer>;
	class piece;

public:
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
		
		bool has_child(std::ptrdiff_t i) {
			assert(! is_leaf() && i >= 0 && i < Splitter::number_of_node_children);
			return children_[i];
		}
		piece_node& child(std::ptrdiff_t i) {
			if(has_child(i)) return *children_[i];
			else return *(children_[i] = new piece_node);
		}
		void delete_child(std::ptrdiff_t i) { assert(has_child(i)); delete children_[i]; }
		const piece_node& child(std::ptrdiff_t i) const { assert(has_child(i)); return *children_[i]; }
	};

private:
	static constexpr std::ptrdiff_t maximal_number_of_points_per_piece = 500000;

	struct piece {
		cuboid root_cuboid;
		std::size_t number_of_points;
		std::ptrdiff_t offset;
		unsigned depth;
		piece_node& node;
	};
	
	model& model_;
	cuboid model_root_cuboid_;
	std::vector<piece> pieces_;
	piece_node root_piece_node_;
	
public:
	tree_structure_piecewise(std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod);
	
	void load_piece(const piece&);
	std::size_t number_of_pieces() const { return pieces_.size(); }
};



template<class Splitter, std::size_t Levels, class PointsContainer>
tree_structure_piecewise<Splitter, Levels, PointsContainer>::tree_structure_piecewise(std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod) : super(leaf_cap, mmfac, dmode, dmax), model_(mod), model_root_cuboid_(Splitter::root_cuboid(mod)) {	
	std::stack<piece> pieces_stack;
	pieces_stack.push({ model_root_cuboid_, mod.number_of_points(), 0, 0, root_piece_node_ });
	
	while(! pieces_stack.empty()) {
		piece p = pieces_stack.top();
		pieces_stack.pop();
				
		if(p.number_of_points > maximal_number_of_points_per_piece) {
			std::size_t children_number_of_points[Splitter::number_of_node_children];
			for(auto& np : children_number_of_points) np = 0; 
			
			typename Splitter::node_points_information no_info;
			
			for(const point& pt : mod) {
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
	
	load_piece(pieces_[1]);
}


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure_piecewise<Splitter, Levels, PointsContainer>::load_piece(const piece& p) {
	super::load_model_(model_, p.root_cuboid, p.depth);
}

}

#endif
