#ifndef DYPC_TREE_STRUCTURE_HDF_LOADER_BASE_H_
#define DYPC_TREE_STRUCTURE_HDF_LOADER_BASE_H_

#include <H5Cpp.h>

namespace dypc {

class tree_structure_hdf_loader_base {
protected:
	static const H5::CompType point_type_;
	
public:
	static H5::CompType initialize_point_type();
};

}

#endif
