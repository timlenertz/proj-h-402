#ifndef DYPC_TREE_STRUCTURE_SIMPLE_LOADER_H_
#define DYPC_TREE_STRUCTURE_SIMPLE_LOADER_H_

#include "tree_structure_loader.h"

namespace dypc {

class tree_structure_simple_loader : public tree_structure_loader {
public:		
	std::string loader_name() const override { return "Tree Structure Simple Loader"; }

private:
	std::size_t extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const tree_structure_source::node& nd) const;
	
protected:
	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) override {
		return extract_node_points_(points, capacity, req, source_->root_node());
	}
};

}

#endif
