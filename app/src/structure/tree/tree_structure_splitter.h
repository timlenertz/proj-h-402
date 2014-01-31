#ifndef DYPC_TREE_STRUCTURE_SPLITTER_H_
#define DYPC_TREE_STRUCTURE_SPLITTER_H_

#include "../../model/model.h"
#include "../../cuboid.h"
#include "../../point.h"
#include <stdexcept>

namespace dypc {

class tree_structure_splitter {
public:
	static constexpr std::size_t number_of_node_children = 0;

	static cuboid root_cuboid(const model& mod) {
		return cuboid(
			glm::vec3(mod.x_minimum(), mod.y_minimum(), mod.z_minimum()),
			glm::vec3(mod.x_range(), mod.y_range(), mod.z_range())
		);
	}
	
	static std::ptrdiff_t node_child_for_point(const point& pt, const cuboid& cub) { throw std::logic_error("Not implemented"); }
	static cuboid node_child_cuboid(const std::ptrdiff_t i, const cuboid& cub) { throw std::logic_error("Not implemented"); }
};

}

#endif
