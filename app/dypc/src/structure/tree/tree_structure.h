#ifndef DYPC_TREE_STRUCTURE_H_
#define DYPC_TREE_STRUCTURE_H_

#include "../mipmap_structure.h"
#include "../../enums.h"
#include "../../geometry/cuboid.h"
#include "../../point.h"
#include "../../progress.h"
#include "../../model/model.h"
#include "../../downsampling.h"
#include "tree_structure_node.h"
#include <cassert>
#include <map>

#include <deque>
#include <vector>

namespace dypc {

/**
 * Tree structure where the model is recursively subdivided into cuboid regions.
 * @tparam Splitter Tree structure splitter which defines how space is subdivided into cuboids.
 * @tparam Levels Mipmap levels of downsampling to generate.
 * @tparam PointsContainer Container used to store arrays of points.
 */
template<class Splitter, std::size_t Levels, class PointsContainer = std::deque<point>>
class tree_structure : public mipmap_structure {
public:
	using splitter = Splitter; ///< Splitter type for this tree structure class.
	using node = tree_structure_node<Splitter, Levels, PointsContainer>; ///< Node type for this tree structure class.
	static constexpr std::size_t levels = Levels; ///< Number of levels for this tree structure class.
	static constexpr std::size_t number_of_node_children = Splitter::number_of_node_children; ///< Number of children per node for this tree structure class.
	
protected:
	struct no_load_t {};
	static constexpr no_load_t no_load = no_load_t();

	const std::size_t leaf_capacity_; ///< Maximal number of points per node.

	node root_; ///< The root node.
	cuboid root_cuboid_; ///< The cuboid of the root node, encloses entire model.
	PointsContainer all_points_[Levels]; ///< The ordered point sets for the different levels.
	
	/**
	 * Create stub tree structure.
	 * Does not load create the tree or load any points from model
	 * @param leaf_cap Maximal number of points per node.
	 * @param dmin Downsampling minimum output number of points.
	 * @param damount Downsampling amount.
	 * @param dmode Downsampling mode.
	 * @param mod The model, must exist during lifetime of tree structure.
	 * @param no_load Marker.
	 */
	tree_structure(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, no_load_t no_load) : mipmap_structure(Levels, dmin, damount, dmode, mod), leaf_capacity_(leaf_cap) { }
	
	/**
	 * Unload model.
	 * Unloads points and destroys tree, and frees used memory
	 */
	void unload_();
	
	/**
	 * Load model.
	 * Load points from model and create tree nodes. Can be restricted to cuboid region of model (for piecewise tree @see tree_structure_piecewise).
	 * @param cub Cuboid within which to load.
	 */
	void load_(const cuboid& cub);

public:
	/**
	 * Create tree structure.
	 * Creates tree and loads all points. Optionally also loads all downsampled points.
	 * @param leaf_cap Maximal number of points per node.
	 * @param dmin Downsampling minimum output number of points.
	 * @param damount Downsampling amount.
	 * @param dmode Downsampling mode.
	 * @param mod The model, must exist during lifetime of tree structure.
	 * @param load_all_downsampled If true, also load all levels of downsampled points.
	 */
	tree_structure(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, bool load_all_downsampled = true);
	
	/**
	 * Get number of points for given level.
	 * Returns 0 when points at that level are not loader
	 * @param lvl Mipmap level, 0 for not downsampled.
	 */
	std::size_t number_of_points(std::ptrdiff_t lvl = 0) const { assert(lvl >= 0 && lvl < levels); return all_points_[lvl].size(); }
	
	/**
	 * Get total number of nodes in tree.
	 */
	std::size_t number_of_nodes() const { return root_.number_of_nodes_in_branch(); }
	
	/**
	 * Get approximate memory usage of structure.
	 */
	std::size_t size() const;
	
	/**
	 * Generate downsampled points for given level.
	 * @param lvl Mipmap level, must be larger than 0.
	 * @param previous_results Previous results information for use by downsampling algorithm.
	 */
	void load_downsampled_points(std::ptrdiff_t lvl, uniform_downsampling_previous_results_t& previous_results);
	
	/**
	 * Generate downsampled points for given level.
	 * @param lvl Mipmap level, must be larger than 0.
	 */
	void load_downsampled_points(std::ptrdiff_t lvl) {
		uniform_downsampling_previous_results_t previous_results;
		load_downsampled_points(lvl, previous_results);
	}
	
	/**
	 * Unload downsampled points for given level.
	 * Free used memory
	 * @param lvl Mipmap level, must be larger than 0.
	 */
	void unload_downsampled_points(std::ptrdiff_t lvl);
	
	const node& root_node() const { return root_; } ///< Get root node.
	const cuboid& root_cuboid() const { return root_cuboid_; } ///< Get cuboid of root node.
	
	/**
	 * Get ordered points array for given downsampling level.
	 * @param lvl Mipmap level, 0 for not downsampled.
	 */
	const PointsContainer& points_at_level(unsigned lvl = 0) const {
		assert(lvl < Levels); return all_points_[lvl];
	}

	/**
	 * Get offset for given node in ordered points array.
	 * Points in array returned by points_at_level starting at the returned offset are points of that node.
	 * @param nd Node of this tree structure.
	 * @param lvl Mipmap level, 0 for not downsampled.
	 */
	std::ptrdiff_t node_points_offset(const node& nd, unsigned lvl = 0) const {
		return nd.points_begin(lvl) - all_points_[lvl].begin();
	}
};


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure<Splitter, Levels, PointsContainer>::unload_() {
	root_ = node();
	for(auto& pts : all_points_) {
		pts.clear();
		pts.shrink_to_fit(); // Make sure memory is freed
	}
}


template<class Splitter, std::size_t Levels, class PointsContainer>
tree_structure<Splitter, Levels, PointsContainer>::tree_structure(std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, bool load_all_downsampled) :
mipmap_structure(Levels, dmin, damount, dmode, mod), leaf_capacity_(leaf_cap) {
	load_(mod.enclosing_cuboid());
	if(load_all_downsampled) for(std::ptrdiff_t lvl = 1; lvl < Levels; ++lvl) load_downsampled_points(lvl);
}


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure<Splitter, Levels, PointsContainer>::load_downsampled_points(std::ptrdiff_t lvl, uniform_downsampling_previous_results_t& previous_results) {
	assert(lvl >= 1 && lvl < Levels);

	PointsContainer downsampled; // Will hold unordered downsampled points
	const auto& original_points = all_points_[0];
	auto& level_points = all_points_[lvl]; // Will hold ordered downsampled points
	
	// Call downsampling algorithm
	downsample_points_(original_points.begin(), original_points.end(), lvl, root_cuboid_.area(), downsampled, previous_results);
	
	// Add points into root node
	progress(all_points_[0].size(), "Adding downsampled points, level " + std::to_string(lvl) + "...", [&](progress_handle& pr) {
		root_.add_root_node_points(lvl, level_points, downsampled, root_cuboid_, leaf_capacity_, pr);
	});
}

template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure<Splitter, Levels, PointsContainer>::unload_downsampled_points(std::ptrdiff_t lvl) {
	assert(lvl >= 1 && lvl < Levels);
	all_points_[lvl].clear();
	all_points_[lvl].shrink_to_fit(); // Make sure memory is freed
}


template<class Splitter, std::size_t Levels, class PointsContainer>
void tree_structure<Splitter, Levels, PointsContainer>::load_(const cuboid& cub) {	
	unload_(); // Unloads existing data, if any.
	
	// Cuboid of root node: Cuboid of model points to load, adjusted to fit tree structure
	// e.g. For Octree, make it enclosing cube
	root_cuboid_ = Splitter::adjust_root_cuboid(cub);
	
	// Will hold unordered array of all points to add
	PointsContainer all_points_unordered;
	
	// Collect points from model that are in cuboid
	progress_foreach(model_, "Collecting points from model...", [&](const point& pt) {
		if(cub.in_range(pt)) all_points_unordered.push_back(pt);
	});

	// Add points and build tree
	progress(all_points_unordered.size(), "Adding points and building tree...", [&](progress_handle& pr) {
		root_.add_root_node_points(0, all_points_[0], all_points_unordered, root_cuboid_, leaf_capacity_, pr);
	});
}



template<class Splitter, std::size_t Levels, class PointsContainer>
std::size_t tree_structure<Splitter, Levels, PointsContainer>::size() const {
	std::size_t sz = sizeof(tree_structure) + root_.size();
	for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) sz += all_points_[lvl].size() * sizeof(point);
	return sz;
}

}

#endif
