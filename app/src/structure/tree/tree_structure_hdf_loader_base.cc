#include "tree_structure_hdf_loader_base.h"
#include "../../point.h"

namespace dypc {

const H5::CompType tree_structure_hdf_loader_base::point_type_ = tree_structure_hdf_loader_base::initialize_point_type();

H5::CompType tree_structure_hdf_loader_base::initialize_point_type() {
	H5::CompType t(sizeof(point));
	t.insertMember("x", HOFFSET(point, x), H5::PredType::NATIVE_FLOAT);
	t.insertMember("y", HOFFSET(point, y), H5::PredType::NATIVE_FLOAT);
	t.insertMember("z", HOFFSET(point, z), H5::PredType::NATIVE_FLOAT);
	t.insertMember("r", HOFFSET(point, r), H5::PredType::NATIVE_UCHAR);
	t.insertMember("g", HOFFSET(point, g), H5::PredType::NATIVE_UCHAR);
	t.insertMember("b", HOFFSET(point, b), H5::PredType::NATIVE_UCHAR);
	return t;
}

}
