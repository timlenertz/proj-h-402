#ifndef DYPC_TREE_STRUCTURE_HDF_FILE_H_
#define DYPC_TREE_STRUCTURE_HDF_FILE_H_

#include <H5Cpp.h>
#include "../../../geometry/cuboid.h"
#include <string>
#include <cstring>
#include <memory>
#include <cstdint>
#include <vector>

namespace dypc {

/**
 * 
 */
template<std::size_t Levels, std::size_t NumberOfChildren>
class tree_structure_hdf_file {
public:
	struct hdf_node {
		std::uint32_t data_start[Levels];
		std::uint32_t data_length[Levels];
		glm::vec3 cuboid_origin;
		glm::vec3 cuboid_sides;
		std::uint32_t children[NumberOfChildren];
		
		static constexpr std::ptrdiff_t no_child_index = -1;
		
		bool is_leaf() const {
			for(auto child : children) if(child) return false;
			return true;
		}
		
		cuboid node_cuboid() const { return cuboid(cuboid_origin, cuboid_sides); }
		bool has_child(std::ptrdiff_t i) const { return children[i]; }
		const hdf_node& child_node(const hdf_node* begin, std::ptrdiff_t i) const { assert(has_child(i)); return *(begin + children[i]); }
		std::ptrdiff_t child_index_for_point(const hdf_node* begin, glm::vec3 pt) const;
	};
	
	~tree_structure_hdf_file() {
		file_.flush(H5F_SCOPE_GLOBAL);
	}
	

private:
	static std::string points_set_name_(std::ptrdiff_t lvl) {
		return std::string("points") + std::to_string(lvl);
	}
	
	H5::H5File file_;
	H5::DataSet points_data_set_[Levels];
	H5::DataSet nodes_data_set_;
	
	static const H5::CompType point_type_;
	static const H5::CompType node_type_;

	static constexpr std::size_t maximal_chunk_size_ = 1000000;
	static constexpr hsize_t points_data_set_chunk_size_ = 1000000;
	
	template<class Iterator> void write_(Iterator el_begin, Iterator el_end, const H5::DataType& typ, const H5::DataSet&, hsize_t offset);
	template<class Inserter> void read_insert_(Inserter ins, hsize_t n, const H5::DataType& typ, const H5::DataSet&, hsize_t offset = 0) const;
	
	template<class Element> void write_(Element* el_begin, Element* el_end, const H5::DataType& typ, const H5::DataSet&, hsize_t offset);
	template<class Element> void read_(Element* ins, hsize_t n, const H5::DataType& typ, const H5::DataSet&, hsize_t offset = 0) const;
	
	void initialize_nodes_(hsize_t n);
	
public:
	static H5::CompType initialize_point_type();
	static H5::CompType initialize_node_type();

	tree_structure_hdf_file(const std::string& filename);
	tree_structure_hdf_file(const std::string& filename, hsize_t max_points);

	std::size_t get_file_size() const { return file_.getFileSize(); }

	hsize_t get_number_of_points(std::ptrdiff_t lvl = 0) const {
		hsize_t dims, maxdims;
		points_data_set_[lvl].getSpace().getSimpleExtentDims(&dims, &maxdims);
		return dims;
	}
	
	void set_number_of_points(hsize_t n, std::ptrdiff_t lvl = 0) {
		points_data_set_[lvl].extend(&n);
	}
	
	hsize_t get_number_of_nodes() const {
		hsize_t dims, maxdims;
		nodes_data_set_.getSpace().getSimpleExtentDims(&dims, &maxdims);
		return dims;
	}
	
	template<class Iterator> void write_points(Iterator pt_begin, Iterator pt_end, std::ptrdiff_t lvl, hsize_t offset = 0) {
		set_number_of_points(offset + (pt_end - pt_begin), lvl);
		write_(pt_begin, pt_end, point_type_, points_data_set_[lvl], offset);
	}
	void write_points(typename std::vector<point>::const_iterator pt_begin, typename std::vector<point>::const_iterator pt_end, std::ptrdiff_t lvl, hsize_t offset = 0) {
		write_points(&(*pt_begin), &(*pt_end), lvl, offset);
	}
	void write_points(typename std::vector<point>::iterator pt_begin, typename std::vector<point>::iterator pt_end, std::ptrdiff_t lvl, hsize_t offset = 0) {
		write_points(&(*pt_begin), &(*pt_end), lvl, offset);
	}
	template<class Inserter> void read_points(Inserter ins, hsize_t n, std::ptrdiff_t lvl, hsize_t offset = 0) const {
		read_insert_<Inserter>(ins, n, point_type_, points_data_set_[lvl], offset);
	}
	void read_points(point* buf, hsize_t n, std::ptrdiff_t lvl, hsize_t offset = 0) const {
		read_<point>(buf, n, point_type_, points_data_set_[lvl], offset);
	}
	
	template<class Iterator> void write_nodes(Iterator nd_begin, Iterator nd_end) {
		initialize_nodes_(nd_end - nd_begin);
		write_(nd_begin, nd_end, node_type_, nodes_data_set_, 0);
	}
	void write_nodes(typename std::vector<hdf_node>::const_iterator pt_begin, typename std::vector<hdf_node>::const_iterator pt_end) {
		write_nodes(&(*pt_begin), &(*pt_end));
	}
	void write_nodes(typename std::vector<hdf_node>::iterator pt_begin, typename std::vector<hdf_node>::iterator pt_end) {
		write_nodes(&(*pt_begin), &(*pt_end));
	}
	template<class Inserter> void read_nodes(Inserter ins, hsize_t n, hsize_t offset = 0) const {
		read_<Inserter>(ins, n, node_type_, nodes_data_set_, offset);
	}
	void read_nodes(hdf_node* buf, hsize_t n, hsize_t offset = 0) const {
		read_<hdf_node>(buf, n, node_type_, nodes_data_set_, offset);
	}
	void read_node(hdf_node* nd, hsize_t position) const {
		read_nodes(nd, 1, position);
	}
};



template<std::size_t Levels, std::size_t NumberOfChildren>
std::ptrdiff_t tree_structure_hdf_file<Levels, NumberOfChildren>::hdf_node::child_index_for_point(const hdf_node* begin, glm::vec3 pt) const {
	assert(node_cuboid().in_range(pt));
	for(std::ptrdiff_t i = 0; i < NumberOfChildren; ++i) {
		if(! has_child(i)) continue;
		const auto& child = child_node(begin, i);
		if(child.node_cuboid().in_range(pt)) return i;
	}
	return no_child_index;
}



template<std::size_t Levels, std::size_t NumberOfChildren>
const H5::CompType tree_structure_hdf_file<Levels, NumberOfChildren>::point_type_ = tree_structure_hdf_file<Levels, NumberOfChildren>::initialize_point_type();


template<std::size_t Levels, std::size_t NumberOfChildren>
const H5::CompType tree_structure_hdf_file<Levels, NumberOfChildren>::node_type_ = tree_structure_hdf_file<Levels, NumberOfChildren>::initialize_node_type();


template<std::size_t Levels, std::size_t NumberOfChildren>
H5::CompType tree_structure_hdf_file<Levels, NumberOfChildren>::initialize_point_type() {
	H5::CompType t(sizeof(point));
	t.insertMember("x", HOFFSET(point, x), H5::PredType::NATIVE_FLOAT);
	t.insertMember("y", HOFFSET(point, y), H5::PredType::NATIVE_FLOAT);
	t.insertMember("z", HOFFSET(point, z), H5::PredType::NATIVE_FLOAT);
	t.insertMember("r", HOFFSET(point, r), H5::PredType::NATIVE_UCHAR);
	t.insertMember("g", HOFFSET(point, g), H5::PredType::NATIVE_UCHAR);
	t.insertMember("b", HOFFSET(point, b), H5::PredType::NATIVE_UCHAR);
	return t;
}


template<std::size_t Levels, std::size_t NumberOfChildren>
H5::CompType tree_structure_hdf_file<Levels, NumberOfChildren>::initialize_node_type() {
	H5::CompType t(sizeof(hdf_node));
	hsize_t three_dims[] = { 3 };
	hsize_t levels_dims[] = { Levels };
	hsize_t child_dims[] = { NumberOfChildren };
	t.insertMember("data_start", HOFFSET(hdf_node, data_start), H5::ArrayType(H5::PredType::NATIVE_UINT32, 1, levels_dims));
	t.insertMember("data_length", HOFFSET(hdf_node, data_length), H5::ArrayType(H5::PredType::NATIVE_UINT32, 1, levels_dims));
	t.insertMember("cuboid_origin", HOFFSET(hdf_node, cuboid_origin), H5::ArrayType(H5::PredType::NATIVE_FLOAT, 1, three_dims));
	t.insertMember("cuboid_sides", HOFFSET(hdf_node, cuboid_sides), H5::ArrayType(H5::PredType::NATIVE_FLOAT, 1, three_dims));
	t.insertMember("children", HOFFSET(hdf_node, children), H5::ArrayType(H5::PredType::NATIVE_UINT32, 1, child_dims));
	return t;
}


template<std::size_t Levels, std::size_t NumberOfChildren>
tree_structure_hdf_file<Levels, NumberOfChildren>::tree_structure_hdf_file(const std::string& filename) {
	file_.openFile(filename, H5F_ACC_RDONLY);
	nodes_data_set_ = file_.openDataSet("nodes");
	for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) points_data_set_[lvl] = file_.openDataSet(points_set_name_(lvl));
}



template<std::size_t Levels, std::size_t NumberOfChildren>
tree_structure_hdf_file<Levels, NumberOfChildren>::tree_structure_hdf_file(const std::string& filename, hsize_t max_points) {
	file_ = H5::H5File(filename, H5F_ACC_TRUNC);
	for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) {
		auto& set = points_data_set_[lvl];

		H5::DSetCreatPropList prop;
		hsize_t zero = 0, chunk_size = points_data_set_chunk_size_;
		prop.setChunk(1, &chunk_size);
		
		set = file_.createDataSet(points_set_name_(lvl), point_type_, H5::DataSpace(1, &zero, &max_points), prop);
	}
}


template<std::size_t Levels, std::size_t NumberOfChildren>
void tree_structure_hdf_file<Levels, NumberOfChildren>::initialize_nodes_(hsize_t n) {
	nodes_data_set_ = file_.createDataSet("nodes", node_type_, H5::DataSpace(1, &n));
}


template<std::size_t Levels, std::size_t NumberOfChildren> template<class Iterator>
void tree_structure_hdf_file<Levels, NumberOfChildren>::write_(Iterator begin, Iterator end, const H5::DataType& typ, const H5::DataSet& set, hsize_t offset) {
	using element_t = typename Iterator::value_type;
	
	hsize_t total = end - begin;
	hsize_t remaining = total;
	
	H5::DataSpace data_space = set.getSpace();
	
	Iterator it = begin;
	while(remaining) {
		element_t chunk[maximal_chunk_size_];
		
		hsize_t chunk_size = maximal_chunk_size_;
		if(remaining < chunk_size) chunk_size = remaining;
		Iterator chunk_end_it = it + chunk_size;
		
		element_t* chunk_el = chunk;
		while(it != chunk_end_it) *(chunk_el++) = *(it++);
		
		H5::DataSpace mem_space(1, &chunk_size);
		data_space.selectHyperslab(H5S_SELECT_SET, &chunk_size, &offset);
		set.write((const void*) chunk, typ, mem_space, data_space);
		
		remaining -= chunk_size;
		offset += chunk_size;
	}
}



template<std::size_t Levels, std::size_t NumberOfChildren> template<class Inserter>
void tree_structure_hdf_file<Levels, NumberOfChildren>::read_insert_(Inserter ins, hsize_t n, const H5::DataType& typ, const H5::DataSet& set, hsize_t offset) const {	
	using element_t = typename Inserter::value_type;
	
	hsize_t remaining = n;
	
	H5::DataSpace data_space = set.getSpace();
	
	while(remaining) {
		element_t chunk[maximal_chunk_size_];
		
		hsize_t chunk_size = maximal_chunk_size_;
		if(remaining < chunk_size) chunk_size = remaining;
		
		H5::DataSpace mem_space(1, &chunk_size);
		data_space.selectHyperslab(H5S_SELECT_SET, &chunk_size, &offset);
		set.read((void*) chunk, typ, mem_space, data_space);
		
		std::size_t chunk_remaining = chunk_size;
		element_t* chunk_el = chunk;
		while(chunk_remaining--) ins = *(chunk_el++);		
		
		remaining -= chunk_size;
		offset += chunk_size;
	}
}



template<std::size_t Levels, std::size_t NumberOfChildren> template<class Element>
void tree_structure_hdf_file<Levels, NumberOfChildren>::write_(Element* el_begin, Element* el_end, const H5::DataType& typ, const H5::DataSet& set, hsize_t offset) {
	hsize_t n = el_end - el_begin;
	if(n == 0) return;
	H5::DataSpace mem_space(1, &n);
	H5::DataSpace data_space = set.getSpace();
	data_space.selectHyperslab(H5S_SELECT_SET, &n, &offset);
	set.write((const void*)el_begin, typ, mem_space, data_space);
}



template<std::size_t Levels, std::size_t NumberOfChildren> template<class Element>
void tree_structure_hdf_file<Levels, NumberOfChildren>::read_(Element* buf, hsize_t n, const H5::DataType& typ, const H5::DataSet& set, hsize_t offset) const {
	H5::DataSpace mem_space(1, &n);
	H5::DataSpace data_space = set.getSpace();
	data_space.selectHyperslab(H5S_SELECT_SET, &n, &offset);
	set.read((void*) buf, typ, mem_space, data_space);
}

}

#endif
