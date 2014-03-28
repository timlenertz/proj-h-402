#ifndef DYPC_TREE_STRUCTURE_ORDERED_LOADER_H_
#define DYPC_TREE_STRUCTURE_ORDERED_LOADER_H_

#include "tree_structure_loader.h"
#include <vector>

namespace dypc {

class tree_structure_ordered_loader : public tree_structure_loader {
protected:
	using source_node = tree_structure_source::node;

private:
	std::vector<const source_node*> position_path_;
	
public:	
	std::string loader_name() const override { return "Tree Structure Ordered Loader"; }

protected:
	virtual std::size_t extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const tree_structure_source::node& nd, const source_node* skip = nullptr) const;
	
	void updated_source_() override;
	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) override;
};


}

#endif
