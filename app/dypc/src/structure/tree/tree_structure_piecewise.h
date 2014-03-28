#ifndef DYPC_TREE_STRUCTURE_PIECEWISE_H_
#define DYPC_TREE_STRUCTURE_PIECEWISE_H_

#include "tree_structure.h"
#include <vector>
#include <stack>
#include <list>
#include <cmath>

namespace dypc {

template<class Splitter, std::size_t Levels, class PointsContainer = std::vector<point>, class PiecesSplitter = Splitter>
class tree_structure_piecewise : public tree_structure<Splitter, Levels, PointsContainer> {
	static_assert(Splitter::number_of_node_children >= PiecesSplitter::number_of_node_children, "Pieces splitter cannot have more children than nodes splitter");
	
private:
	using super = tree_structure<Splitter, Levels, PointsContainer>;

public:
	struct piece;

	class piece_node {
	private:
		const piece* piece_ = nullptr;
		piece_node* children_[PiecesSplitter::number_of_node_children];

	public:
		piece_node() { for(auto& c : children_) c = nullptr; }
		explicit piece_node(const piece& p) : piece_node(), piece_(&p) { }
		~piece_node() { if(! is_leaf()) for(piece_node* c : children_) if(c) delete c; }
	
		piece_node(const piece_node&) = delete;
		piece_node& operator=(const piece_node&) = delete;
		piece_node(piece_node&& nd) : piece_(nd.piece_) {
			for(std::ptrdiff_t i = 0; i < PiecesSplitter::number_of_node_children; ++i) {
				children_[i] = nd.children_[i];
				nd.children_[i] = nullptr;
			}
		}
		piece_node& operator=(piece_node&&) = delete;
	
		bool is_leaf() const { return piece_; }
		void set_piece(const piece& p) { piece_ = &p; }
		const piece& get_piece() const { assert(is_leaf()); return *piece_; }
		
		bool has_child(std::ptrdiff_t i) const {
			assert(! is_leaf() && i >= 0 && i < PiecesSplitter::number_of_node_children);
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
	std::list<piece> pieces_;
	piece_node root_piece_node_;
	
public:
	tree_structure_piecewise(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod);
	tree_structure_piecewise(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, std::ptrdiff_t maxnum);
	
	const piece_node& root_piece_node() const { return root_piece_node_; }
	const cuboid& root_piece_cuboid() const { return model_root_cuboid_; }
	
	void load_piece(const piece&);
	std::size_t number_of_pieces() const { return pieces_.size(); }
	
	std::size_t total_number_of_points() const { return model_.number_of_points(); }
	std::size_t total_number_of_points_upper_bound(std::ptrdiff_t lvl) const;
};



template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::tree_structure_piecewise(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod) : super(leaf_cap, dmin, damount, dmode, mod), model_(mod), model_root_cuboid_(Splitter::root_cuboid(mod)), maximal_number_of_points_per_piece_(0) { }



template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::tree_structure_piecewise(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, std::ptrdiff_t maxnum) : super(leaf_cap, dmin, damount, dmode, mod), model_(mod), model_root_cuboid_(Splitter::root_cuboid(mod)), maximal_number_of_points_per_piece_(maxnum) {		
	std::stack<piece> pieces_stack;
	pieces_stack.push({ model_root_cuboid_, mod.number_of_points(), 0, 0, root_piece_node_ });
	
	progress(mod.number_of_points(), "Determining tree structure pieces...", [&](progress_handle& pr) {

	while(! pieces_stack.empty()) {		
		piece p = pieces_stack.top();
		pieces_stack.pop();
						
		if(maximal_number_of_points_per_piece_ && (p.number_of_points > maximal_number_of_points_per_piece_) && !p.node.is_leaf()) {
			std::size_t children_number_of_points[PiecesSplitter::number_of_node_children];
			for(auto& np : children_number_of_points) np = 0; 
			
			typename PiecesSplitter::node_points_information no_info;
			
			progress_foreach(mod, "Counting points in child pieces", [&](const point& pt) {
				assert(model_root_cuboid_.in_range(pt));
				if(! p.root_cuboid.in_range(pt)) return;
				std::ptrdiff_t i = PiecesSplitter::node_child_for_point(pt, p.root_cuboid, no_info, p.depth);
				++children_number_of_points[i];
				
				//for(std::ptrdiff_t i= 0; i < PiecesSplitter::number_of_node_children; ++i) std::cout << "Child " << i << ": " << children_number_of_points[i] << std::endl;
				
				#ifndef NDEBUG
				for(std::ptrdiff_t j = 0; j < PiecesSplitter::number_of_node_children; ++j) {
					cuboid cub = PiecesSplitter::node_child_cuboid(j, p.root_cuboid, no_info, p.depth);
					assert(cub.in_range(pt) == (i == j));
				}
				#endif
			});
			
			std::ptrdiff_t offset = 0;
			for(std::ptrdiff_t i = 0; i < PiecesSplitter::number_of_node_children; ++i) {
				std::size_t n = children_number_of_points[i];
				if(n == 0) continue;
								
				cuboid cub = PiecesSplitter::node_child_cuboid(i, p.root_cuboid, no_info, p.depth);
				
				pieces_stack.push({ cub, n, offset, p.depth + 1, p.node.child(i) });
				offset += n;
			}
			
		} else {
			pieces_.push_back(p);
			piece& stored_piece = pieces_.back();
			stored_piece.node.set_piece(stored_piece);
				
			pr.increment(p.number_of_points);
		}
	}
	
	/*
	std::size_t total=0;
	std::cout << "pieces" << std::endl;
	for(const auto& p : pieces_) {
		total += p.number_of_points;
		std::cout << "piece " << p.offset << " nb:" << p.number_of_points << " depth: " << p.depth << " total: " << total << std::endl;
	}
	std::cout << "model total: " << model_.number_of_points() << std::endl;
	*/
	
	});
}


template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
void tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::load_piece(const piece& p) {
	super::load_model_(model_, p.root_cuboid, p.depth);
	assert(super::number_of_points() == p.number_of_points);
}


template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
std::size_t tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>::total_number_of_points_upper_bound(std::ptrdiff_t lvl) const {
	std::size_t n = total_number_of_points();
	float ratio = std::pow(super::mipmap_factor_, lvl);
	return ratio * n;
}


}

#endif
