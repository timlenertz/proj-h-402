#ifndef DYPC_TREE_STRUCTURE_NODE_H_
#define DYPC_TREE_STRUCTURE_NODE_H_

#include "../../point.h"
#include <cassert>
#include <vector>

namespace dypc {


template<class Splitter, std::size_t Levels>
class tree_structure_node {
private:
	using node_points_information = typename Splitter::node_points_information;

	struct point_set {
		point* points = nullptr;
		std::size_t number_of_points = 0;
		
		void add_point_to_buffer(const point& pt, std::size_t leaf_capacity) {
			if(! points) points = new point [leaf_capacity];
			assert(number_of_points < leaf_capacity);
			points[number_of_points++] = pt;
		}
	};

	tree_structure_node* children_[Splitter::number_of_node_children];
	point_set point_sets_[Levels];
	node_points_information points_information_;
	
public:	
	tree_structure_node() {
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) children_[i] = nullptr;
	}
	
	~tree_structure_node() {
		if(! children_[0]) return;
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) delete children_[i];
	}
	
	const tree_structure_node& child(std::ptrdiff_t i) const { assert(i >= 0 && i < Splitter::number_of_node_children); return *children_[i]; }
	tree_structure_node& child(std::ptrdiff_t i) { assert(i >= 0 && i < Splitter::number_of_node_children); return *children_[i]; }
	
	const point* points_begin(std::ptrdiff_t lvl = 0) const { assert(lvl >= 0 && lvl < Levels); return point_sets_[lvl].points; }
	const point* points_end(std::ptrdiff_t lvl = 0) const { assert(lvl >= 0 && lvl < Levels); return point_sets_[lvl].points + point_sets_[lvl].number_of_points; }

	void add_higher_level_point(unsigned level, const point& pt, const cuboid& cube, unsigned depth, std::size_t leaf_capacity);
	void add_points_with_information(std::vector<point>& all_points, const cuboid& cub, unsigned depth, std::size_t leaf_capacity);
	std::size_t move_out_points(std::vector<point>&, unsigned level = 0);
		
	std::size_t number_of_nodes_in_branch() const {
		std::size_t n = 1;
		if(! is_leaf()) for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) n += children_[i]->number_of_nodes_in_branch();
		return n;
	}

	std::size_t number_of_points(std::ptrdiff_t lvl = 0) const { assert(lvl >= 0 && lvl < Levels); return point_sets_[lvl].number_of_points; }
	std::size_t size() const;
	
	bool is_leaf() const { return (children_[0] == nullptr); }
	const node_points_information& get_points_information() const { return points_information_; }
};


template<class Splitter, std::size_t Levels>
void tree_structure_node<Splitter, Levels>::add_higher_level_point(unsigned level, const point& pt, const cuboid& cub, unsigned depth, std::size_t leaf_capacity) {
	assert(level > 0);
	
	auto& set = point_sets_[level];
	if(! is_leaf()) {
		auto child_index = Splitter::node_child_for_point(pt, cub, points_information_, depth);
		auto child_cuboid = Splitter::node_child_cuboid(child_index, cub, points_information_, depth);
		children_[child_index]->add_higher_level_point(level, pt, child_cuboid, depth + 1, leaf_capacity);
	} else if(set.number_of_points < leaf_capacity) {
		assert(cub.in_range(pt));
		set.add_point_to_buffer(pt, leaf_capacity);
	} else {
		throw std::logic_error("Not enough capacity to add higher level point to tree node");
	}
}


template<class Splitter, std::size_t Levels>
void tree_structure_node<Splitter, Levels>::add_points_with_information(std::vector<point>& all_points, const cuboid& cub, unsigned depth, std::size_t leaf_capacity) {
	assert(is_leaf());
		
	points_information_ = Splitter::compute_node_points_information(all_points, cub, depth);
		
	if(all_points.size() <= leaf_capacity) {
		auto& set = point_sets_[0];
		for(const point& pt : all_points) { assert(cub.in_range(pt)); set.add_point_to_buffer(pt, leaf_capacity); }
		increment_progress(all_points.size());
		
	} else {
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) children_[i] = new tree_structure_node;
		
		std::vector<point> child_point_sets[Splitter::number_of_node_children];
		for(const point& pt : all_points) {
			std::ptrdiff_t i = Splitter::node_child_for_point(pt, cub, points_information_, depth);
			child_point_sets[i].push_back(pt);
		}
		
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) {
			cuboid child_cuboid = Splitter::node_child_cuboid(i, cub, points_information_, depth);
			auto& set = child_point_sets[i];
			child(i).add_points_with_information(set, child_cuboid, depth + 1, leaf_capacity);
			set.clear();
		}
	}
}


template<class Splitter, std::size_t Levels>
std::size_t tree_structure_node<Splitter, Levels>::move_out_points(std::vector<point>& output_points, unsigned lvl) {
	auto& set = this->point_sets_[lvl];
	point* pts_ptr = output_points.data() + output_points.size();
	
	if(is_leaf()) {
		if(! set.points) return 0;
		const point* points_end = set.points + set.number_of_points;
		for(const point* pt = set.points; pt < points_end; ++pt) output_points.push_back(*pt);
		delete[] set.points;
	} else {
		std::size_t count = 0;
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) count += children_[i]->move_out_points(output_points, lvl);
		set.number_of_points = count;
	}
	
	set.points = pts_ptr;
	return set.number_of_points;
}


template<class Splitter, std::size_t Levels>
std::size_t tree_structure_node<Splitter, Levels>::size() const {
	std::size_t sz = sizeof(tree_structure_node);
	if(! is_leaf()) {
		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) sz += children_[i]->size();
	}
	return sz;
}




}

#endif
