#include "cubes_structure_hdf_loader.h"
#include "cubes_structure.h"
#include "../../progress.h"
#include "../../downsampling.h"
#include <utility>
#include <memory>

namespace dypc {

H5::CompType cubes_structure_hdf_loader::point_type_ = cubes_structure_hdf_loader::initialize_point_type_();
H5::CompType cubes_structure_hdf_loader::cube_type_ = cubes_structure_hdf_loader::initialize_cube_type_();

H5::CompType cubes_structure_hdf_loader::initialize_point_type_() {
	H5::CompType t(sizeof(weighted_point));
	t.insertMember("x", HOFFSET(weighted_point, x), H5::PredType::NATIVE_FLOAT);
	t.insertMember("y", HOFFSET(weighted_point, y), H5::PredType::NATIVE_FLOAT);
	t.insertMember("z", HOFFSET(weighted_point, z), H5::PredType::NATIVE_FLOAT);
	t.insertMember("r", HOFFSET(weighted_point, r), H5::PredType::NATIVE_UCHAR);
	t.insertMember("g", HOFFSET(weighted_point, g), H5::PredType::NATIVE_UCHAR);
	t.insertMember("b", HOFFSET(weighted_point, b), H5::PredType::NATIVE_UCHAR);
	t.insertMember("weight", HOFFSET(weighted_point, weight), H5::PredType::NATIVE_FLOAT);
	return t;
}

H5::CompType cubes_structure_hdf_loader::initialize_cube_type_() {
	H5::CompType t(sizeof(hdf_cube));
	t.insertMember("x", HOFFSET(hdf_cube, x), H5::PredType::NATIVE_INT32);
	t.insertMember("z", HOFFSET(hdf_cube, y), H5::PredType::NATIVE_INT32);
	t.insertMember("y", HOFFSET(hdf_cube, z), H5::PredType::NATIVE_INT32);
	t.insertMember("data_start", HOFFSET(hdf_cube, data_start), H5::PredType::NATIVE_UINT32);
	t.insertMember("data_length", HOFFSET(hdf_cube, data_length), H5::PredType::NATIVE_UINT32);
	return t;
}


void cubes_structure_hdf_loader::write(const std::string& filename, const cubes_structure& s) {
	H5::H5File file(filename, H5F_ACC_TRUNC);
			
	std::vector<weighted_point> points_data;
	std::vector<hdf_cube> cubes_data;
	hsize_t cube_start = 0;

	for(const auto& p : s.cubes()) {
		auto idx = p.first;
		const cubes_structure::cube& c = p.second;
		
		points_data.insert(points_data.end(), c.weighted_points().begin(), c.weighted_points().end());
		auto len = c.number_of_points();
		
		hdf_cube hc;
		hc.x = std::get<0>(idx);
		hc.y = std::get<1>(idx);
		hc.z = std::get<2>(idx);
		hc.data_start = cube_start;
		hc.data_length = len;
		cubes_data.push_back(hc);
				
		cube_start += len;
	}
	
	hsize_t cubes_dims[] = { cubes_data.size() };
	H5::DataSpace cubes_space(1, cubes_dims);
	H5::DataSet cubes_set = file.createDataSet("cubes", cube_type_, cubes_space);
	cubes_set.write(cubes_data.data(), cube_type_);

	hsize_t points_dims[] = { points_data.size() };
	H5::DataSpace points_space(1, points_dims);
	H5::DataSet points_set = file.createDataSet("points", point_type_, points_space);
	points_set.write(points_data.data(), point_type_);
	
	H5::Attribute side_attr = cubes_set.createAttribute("side_length", H5::PredType::NATIVE_FLOAT, H5::DataSpace(H5S_SCALAR));
	float side_length = s.get_side_length();
	side_attr.write(H5::PredType::NATIVE_FLOAT, (const void*)&side_length);
}



cubes_structure_hdf_loader::cubes_structure_hdf_loader(const std::string& filename) : file_(filename, 0) {	
	auto cubes_set = file_.openDataSet("cubes");
	
	float side_length;
	auto side_attr = cubes_set.openAttribute("side_length");
	side_attr.read(H5::PredType::NATIVE_FLOAT, (void*)&side_length);
	
	auto cubes_space = cubes_set.getSpace();
	hsize_t cubes_count = cubes_space.getSimpleExtentNpoints();
	hdf_cube* cubes_data = new hdf_cube [cubes_count];
	const hdf_cube* cubes_data_end = cubes_data + cubes_count;
	cubes_set.read((void*)cubes_data, cube_type_);
	for(hdf_cube* cub = cubes_data; cub < cubes_data_end; ++cub) {
		cube_entries_.emplace_back(
			cub->data_start,
			cub->data_length,
			cube_from_index_(std::make_tuple(cub->x, cub->y, cub->z), side_length)
		);
	}
	delete[] cubes_data;
		
	points_data_set_ = file_.openDataSet("points");
	points_data_space_ = points_data_set_.getSpace();
	number_of_points_ = points_data_space_.getSimpleExtentNpoints();
}


std::size_t cubes_structure_hdf_loader::extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
	std::size_t remaining = capacity;
	std::size_t total = 0;
	point_buffer_t buf = points;	
	
	for(const auto& entry : cube_entries_) {
		const cuboid& cube = entry.cube;
		if(entry.data_length > remaining) break;
		
		if(frustum_culling_ && !req.view_frustum.contains_cuboid(cube)) continue;
		
		float distance = std::abs(glm::distance(req.position, cube.center()));
		float min_weight = 1.0 - downsampling_ratio_(distance, capacity, number_of_points_);

		points_data_space_.selectHyperslab(H5S_SELECT_SET, &entry.data_length, &entry.data_start);
		
		hsize_t dims[] = { entry.data_length };
		H5::DataSpace mem_space(1, dims);
		
		weighted_point* mem_buf = new weighted_point[entry.data_length];
		weighted_point* mem_buf_end = mem_buf + entry.data_length;
		points_data_set_.read(mem_buf, point_type_, mem_space, points_data_space_);
		
		for(weighted_point* it = mem_buf; it != mem_buf_end; ++it) {
			if(it->weight < min_weight) break;
			
			*(buf++) = *it;
			--remaining;
			++total;
		}
		
		delete[] mem_buf;
	}
	
	return total;
}

	
std::size_t cubes_structure_hdf_loader::memory_size() const {
	return cube_entries_.size() * sizeof(cube_entry);
}

std::size_t cubes_structure_hdf_loader::rom_size() const {
	return file_.getFileSize();
}

std::size_t cubes_structure_hdf_loader::number_of_points() const {
	return number_of_points_;;
}


}
