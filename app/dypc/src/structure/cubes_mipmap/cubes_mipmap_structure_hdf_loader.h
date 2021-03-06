#ifndef DYPC_CUBES_MIPMAP_STRUCTURE_HDF_LOADER_H_
#define DYPC_CUBES_MIPMAP_STRUCTURE_HDF_LOADER_H_

#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <H5Cpp.h>
#include "cubes_mipmap_structure_loader.h"
#include "../../point.h"
#include "../../geometry/cuboid.h"

#include <cstdint>

namespace dypc {
	
class cubes_mipmap_structure;

class cubes_mipmap_structure_hdf_loader : public cubes_mipmap_structure_loader {	
private:
	class cube_entry {
	public:
		hsize_t data_start;
		hsize_t data_length;
		cuboid cube;
		
		cube_entry(hsize_t start, hsize_t len, const cuboid& c) :
			data_start(start), data_length(len), cube(c) { }
	};
	
	struct hdf_cube {
		std::int32_t x;
		std::int32_t y;
		std::int32_t z;
		std::uint32_t data_start;
		std::uint32_t data_length;
	};
	
	static H5::CompType point_type_;
	static H5::CompType cube_type_;
	
	static H5::CompType initialize_point_type_();
	static H5::CompType initialize_cube_type_();	

	std::vector<cube_entry> cube_entries_;
	H5::H5File file_;
	H5::DataSpace points_data_space_;
	H5::DataSet points_data_set_;

	std::uint32_t mipmap_levels_;

protected:
	std::size_t compute_downsampled_points_(point_buffer_t points, std::size_t capacity, const loader::request_t&);

public:
	static void write(const std::string& file, const cubes_mipmap_structure&);
	
	explicit cubes_mipmap_structure_hdf_loader(const std::string& file);
	
	std::string loader_name() const override { return "Cubes Mipmap Structure HDF Loader"; }

	std::size_t memory_size() const override;
	std::size_t rom_size() const override;
	std::size_t number_of_points() const override;
};

}

#endif
