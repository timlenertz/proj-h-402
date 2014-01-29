#ifndef DYPC_TREE_STRUCTURE_LOADER_H_
#define DYPC_TREE_STRUCTURE_LOADER_H_

#include "tree_structure.h"
#include "../structure_loader.h"

namespace dypc {
	
class tree_structure_loader : public structure_loader {
private:
	float downsampling_start_distance_ = 20;
	float downsampling_step_distance_ = 10;
	std::size_t minimal_number_of_points_for_split_ = 1000;

protected:
	static constexpr std::ptrdiff_t action_skip = -2;
	static constexpr std::ptrdiff_t action_split = -1;
	
	std::ptrdiff_t action_for_node_(const cuboid&, std::size_t number_of_points, bool is_leaf, const loader::request_t&, std::size_t levels = 1) const;
	
public:
	void set_downsampling_start_distance(float d) { downsampling_start_distance_ = d; }
	void set_downsampling_step_distance(float d) { downsampling_step_distance_ = d; }
	void set_minimal_number_of_points_for_split(std::size_t n) { minimal_number_of_points_for_split_ = n; }
	
	::wxWindow* create_panel(::wxWindow* parent) override;
};
	
}

#endif
