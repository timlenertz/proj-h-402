#ifndef DYPC_TREE_STRUCTURE_H_
#define DYPC_TREE_STRUCTURE_H_

#include "../structure.h"
#include "../../cuboid.h"
#include "../../point.h"
#include "../../progress.h"
#include "../../model/model.h"
#include "tree_structure_node.h"
#include <vector>
#include <cassert>

namespace dypc {

template<class Splitter, std::size_t Levels = 1>
class tree_structure : public structure {
public:
	using splitter = Splitter;
	using node = tree_structure_node<Splitter, Levels>;
	static constexpr std::size_t levels = Levels;
	static constexpr std::size_t number_of_node_children = Splitter::number_of_node_children;

protected:
	const std::size_t leaf_capacity_;
	node root_;
	cuboid root_cuboid_;
	std::vector<point> all_points_[Levels];

public:
	tree_structure(std::size_t leaf_capacity, model& mod);
	
	std::size_t number_of_points(std::ptrdiff_t lvl = 0) const { assert(lvl >= 0 && lvl < levels); return all_points_[lvl].size(); }
	std::size_t number_of_nodes() const { return root_.number_of_nodes_in_branch(); }
	std::size_t size() const;
	
	const node& root_node() const { return root_; }
	const cuboid& root_cuboid() const { return root_cuboid_; }
	
	const std::vector<point>& points_at_level(unsigned lvl = 0) const
		{ assert(lvl < Levels); return all_points_[lvl]; }
};




template<class Splitter, std::size_t Levels>
tree_structure<Splitter, Levels>::tree_structure(std::size_t leaf_capacity, model& mod) :
leaf_capacity_(leaf_capacity), root_cuboid_(Splitter::root_coboid(mod)) {
	progress("Building tree structure...", mod.number_of_points(), 250000, [&]() {
		for(const auto& pt : mod) {
			root_.add_point(pt, root_cuboid_, leaf_capacity_);
			increment_progress();
		}
	});
	all_points_[0].reserve(mod.number_of_points());
	root_.move_out_points(all_points_[0]);
}


template<class Splitter, std::size_t Levels>
std::size_t tree_structure<Splitter, Levels>::size() const {
	std::size_t sz = sizeof(tree_structure) + root_.size();
	for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) sz += all_points_[lvl].size() * sizeof(point);
	return sz;
}

}

#endif
