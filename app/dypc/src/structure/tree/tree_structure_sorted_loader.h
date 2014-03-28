#ifndef DYPC_TREE_STRUCTURE_SORTED_LOADER_H_
#define DYPC_TREE_STRUCTURE_SORTED_LOADER_H_

#include "tree_structure_ordered_loader.h"
#include "../../canvas/projection_canvas.h"
#include <vector>

namespace dypc {

class tree_structure_sorted_loader : public tree_structure_ordered_loader {	
public:	
	std::string loader_name() const override { return "Tree Structure Sorting Loader"; }

protected:
	std::size_t extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const tree_structure_source::node& nd, const source_node* skip = nullptr) const override;

	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) override;
};


}

#endif
