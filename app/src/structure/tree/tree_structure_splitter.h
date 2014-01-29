#ifndef DYPC_TREE_STRUCTURE_SPLITTER_H_
#define DYPC_TREE_STRUCTURE_SPLITTER_H_

#include "../../model/model.h"

namespace dypc {
	
class tree_structure_splitter {
public:
	static cuboid root_coboid(const model& mod) {
		return cuboid(
			glm::vec3(mod.x_minimum(), mod.y_minimum(), mod.z_minimum()),
			glm::vec3(mod.x_range(), mod.y_range(), mod.z_range())
		);
	}
};

}

#endif
