#ifndef DYPC_TREE_STRUCTURE_LOADER_H_
#define DYPC_TREE_STRUCTURE_LOADER_H_

#include "tree_structure.h"
#include "tree_structure_source.h"
#include "../../loader/downsampling_loader.h"
#include "../../enums.h"
#include <memory>
#include <utility>

namespace dypc {

/**
 * Loader that loader points from tree structure.
 * This is an abstract base class. The two variants \e simple and \e ordered exist, and
 * they receive points from a polymorphic tree_structure_source. This can in turn be a
 * memory source that reads out of tree_structure_node objects, of a HDF source.
 */
class tree_structure_loader : public downsampling_loader {	
private:
	std::size_t minimal_number_of_points_for_split_ = 1000; ///< Minimal number of points for node to be split.
	cuboid_distance_mode downsampling_node_distance_ = cuboid_distance_mode::center; ///< Point-to-cuboid distance setting.
	float additional_split_distance_difference_ = 25; ///< Minimal min-max distance difference to enforce additional split.

	/**
	 * Compute a point-to-cuboid distance.
	 * @param position The point.
	 * @param cub The cuboid.
	 * @param min_dist Minimal distance. This function expects it to be calculated by caller.
	 * @param max_dist Maxomal distance. This function expects it to be calculated by caller.
	 * @param type Distance type.
	 */
	float cuboid_distance_(glm::vec3 position, const cuboid& cub, float min_dist, float max_dist, cuboid_distance_mode type) const;

protected:
	static constexpr std::ptrdiff_t action_skip = -2; ///< Instead of a downsampling level, this value means ignore node.
	static constexpr std::ptrdiff_t action_split = -1; ///< Instead of a downsampling level, this value means descent into node's children.
	
	std::unique_ptr<const tree_structure_source> source_; ///< The tree structure source.
	
	/**
	 * Action to take for given node.
	 * Common behavoir for all tree structure loaders.
	 * @param cub Cuboid of current node.
	 * @param number_of_points Number of points in node.
	 * @param is_leaf Whether node is a leaf.
	 * @param req The loader request with camera position etc.
	 * @param levels Available downsampling levels.
	 * @return Either downsampling level at which the node should be outputted, or \a action_skip or \a action_split.
	 */
	std::ptrdiff_t action_for_node_(const cuboid&, std::size_t number_of_points, bool is_leaf, const loader::request_t& req, std::size_t levels = 1) const;
	
	/**
	 * Compute a point-to-cuboid distance.
	 * @param position The point.
	 * @param cub The cuboid.
	 * @param type Distance type.
	 */
	float cuboid_distance_(glm::vec3 position, const cuboid& cub, cuboid_distance_mode type) const;
	
	
	/**
	 * Compute a point-to-cuboid distance with current point-to-square distance type.
	 * @param position The point.
	 * @param cub The cuboid.
	 */
	float cuboid_distance_(glm::vec3 position, const cuboid& cub) const;
	
	virtual void updated_source_() { } ///< Notified subclass that source was switched.
		
public:
	void set_minimal_number_of_points_for_split(std::size_t n) { minimal_number_of_points_for_split_ = n; }
	void set_downsampling_node_distance(cuboid_distance_mode d) { downsampling_node_distance_ = d; }
	void set_additional_split_distance_difference(float d) { additional_split_distance_difference_ = d; }
	
	double get_setting(const std::string&) const override;
	void set_setting(const std::string&, double) override;
	
	void take_source(const tree_structure_source* src) { source_.reset(src); updated_source_(); } ///< Assign source. Takes ownership of pointer.
	void delete_source() { source_.release(); updated_source_(); } ///< Deletes source.
	
	loader_type get_loader_type() const override { return loader_type::tree; }
	
	std::size_t memory_size() const override { return source_->memory_size(); }
	std::size_t rom_size() const override { return source_->rom_size(); }
	std::size_t number_of_points() const override { return source_->number_of_points(); }
};
	
}

#endif
