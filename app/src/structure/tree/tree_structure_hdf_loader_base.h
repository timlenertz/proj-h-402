#ifndef DYPC_TREE_STRUCTURE_HDF_LOADER_BASE_H_
#define DYPC_TREE_STRUCTURE_HDF_LOADER_BASE_H_

#include <H5Cpp.h>
#include "tree_structure.h"
#include "../../cuboid.h"
#include <string>
#include <cstring>
#include <memory>

namespace dypc {


template<class Structure>
class tree_structure_hdf_loader_base {
protected:
	struct hdf_node {
		std::uint32_t data_start[Structure::levels];
		std::uint32_t data_length[Structure::levels];
		glm::vec3 cuboid_origin;
		glm::vec3 cuboid_sides;
		std::uint32_t children[Structure::number_of_node_children];
		
		bool is_leaf() const { return (children[0] == 0); }
		cuboid node_cuboid() const { return cuboid(cuboid_origin, cuboid_sides); }
		const hdf_node& child_node(const hdf_node* begin, std::ptrdiff_t i) const { return *(begin + children[i]); }
		std::ptrdiff_t child_index_for_point(const hdf_node* begin, glm::vec3 pt) const;
	};

	static std::string points_set_name_(std::ptrdiff_t lvl) {
		return std::string("points") + std::to_string(lvl);
	}
	
	static const H5::CompType point_type_;
	static const H5::CompType node_type_;
	
	template<class Iterator> static void write_points_(Iterator pt_begin, Iterator pt_end, H5::DataSet& data_set);
	static void write_points_(const point* pt_begin, const point* pt_end, H5::DataSet& data_set);

		
public:
	static H5::CompType initialize_point_type();
	static H5::CompType initialize_node_type();
	
	static void write(const std::string& filename, const Structure& s);
};



template<class Structure>
std::ptrdiff_t tree_structure_hdf_loader_base<Structure>::hdf_node::child_index_for_point(const hdf_node* begin, glm::vec3 pt) const {
	assert(node_cuboid().in_range(pt));
	for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) {
		const auto& child = child_node(begin, i);
		if(child.node_cuboid().in_range(pt)) return i;
	}
	throw std::logic_error("No child node containing the point");
}



template<class Structure>
const H5::CompType tree_structure_hdf_loader_base<Structure>::point_type_ = tree_structure_hdf_loader_base<Structure>::initialize_point_type();


template<class Structure>
const H5::CompType tree_structure_hdf_loader_base<Structure>::node_type_ = tree_structure_hdf_loader_base<Structure>::initialize_node_type();


template<class Structure>
H5::CompType tree_structure_hdf_loader_base<Structure>::initialize_point_type() {
	H5::CompType t(sizeof(point));
	t.insertMember("x", HOFFSET(point, x), H5::PredType::NATIVE_FLOAT);
	t.insertMember("y", HOFFSET(point, y), H5::PredType::NATIVE_FLOAT);
	t.insertMember("z", HOFFSET(point, z), H5::PredType::NATIVE_FLOAT);
	t.insertMember("r", HOFFSET(point, r), H5::PredType::NATIVE_UCHAR);
	t.insertMember("g", HOFFSET(point, g), H5::PredType::NATIVE_UCHAR);
	t.insertMember("b", HOFFSET(point, b), H5::PredType::NATIVE_UCHAR);
	return t;
}


template<class Structure>
H5::CompType tree_structure_hdf_loader_base<Structure>::initialize_node_type() {
	H5::CompType t(sizeof(hdf_node));
	hsize_t three_dims[] = { 3 };
	hsize_t levels_dims[] = { Structure::levels };
	hsize_t child_dims[] = { Structure::number_of_node_children };
	t.insertMember("data_start", HOFFSET(hdf_node, data_start), H5::ArrayType(H5::PredType::NATIVE_UINT32, 1, levels_dims));
	t.insertMember("data_length", HOFFSET(hdf_node, data_length), H5::ArrayType(H5::PredType::NATIVE_UINT32, 1, levels_dims));
	t.insertMember("cuboid_origin", HOFFSET(hdf_node, cuboid_origin), H5::ArrayType(H5::PredType::NATIVE_FLOAT, 1, three_dims));
	t.insertMember("cuboid_sides", HOFFSET(hdf_node, cuboid_sides), H5::ArrayType(H5::PredType::NATIVE_FLOAT, 1, three_dims));
	t.insertMember("children", HOFFSET(hdf_node, children), H5::ArrayType(H5::PredType::NATIVE_UINT32, 1, child_dims));
	return t;
}



template<class Structure>
void tree_structure_hdf_loader_base<Structure>::write(const std::string& filename, const Structure& s) {
	H5::H5File file(filename, H5F_ACC_TRUNC);
	
	H5::DataSpace points_space[Structure::levels];
	H5::DataSet points_set[Structure::levels];
	
	progress("Writing points to HDF...", Structure::levels, 1, [&]() {
	for(std::ptrdiff_t lvl = 0; lvl < Structure::levels; ++lvl) {
		hsize_t points_dims[] = { s.number_of_points(lvl) };
		points_space[lvl] = H5::DataSpace(1, points_dims);
		points_set[lvl] = file.createDataSet(points_set_name_(lvl), point_type_, points_space[lvl]);
		
		const auto& all_points = s.points_at_level(lvl);
		write_points_(all_points.begin(), all_points.end(), points_set[lvl]);
		
		increment_progress();
	}
	});

	hsize_t nodes_dims[] = { s.number_of_nodes() };
	H5::DataSpace nodes_space(1, nodes_dims);
	H5::DataSet nodes_set = file.createDataSet("nodes", node_type_, nodes_space);

	using structure_node_t = typename Structure::node;
	struct entry {
		const structure_node_t& node;
		cuboid node_cuboid;
		unsigned depth;
	};
	
	hsize_t nodes_position = 0;
	
	std::stack<entry> stack_;
	stack_.push({ s.root_node(), s.root_cuboid(), 0 });
		
	progress("Writing nodes to HDF...", s.number_of_nodes(), 1000, [&]() {
	while(! stack_.empty()) {
		auto e = stack_.top();
		stack_.pop();
	
		auto node_number_of_points = e.node.number_of_points();

		hdf_node nd;
		nd.cuboid_origin = e.node_cuboid.origin();
		nd.cuboid_sides = e.node_cuboid.side_lengths();
		
		for(std::ptrdiff_t lvl = 0; lvl < Structure::levels; ++lvl) {
			if(e.node.number_of_points(lvl)) nd.data_start[lvl] = s.node_points_offset(e.node, lvl);
			else nd.data_start[lvl] = 0;
			nd.data_length[lvl] = e.node.number_of_points(lvl);
		}
		
		if(e.node.is_leaf()) {
			for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) nd.children[i] = 0;
			
		} else {
			std::size_t child_node_offset = 0;
			for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) {				
				nd.children[i] = nodes_position + 1 + child_node_offset;
				child_node_offset += e.node.child(i).number_of_nodes_in_branch();
			}
			for(std::ptrdiff_t i = Structure::number_of_node_children - 1; i >= 0; --i) {
				auto child_cuboid = Structure::splitter::node_child_cuboid(i, e.node_cuboid, e.node.get_points_information(), e.depth);
				stack_.push({ e.node.child(i), child_cuboid, e.depth + 1 });
			}
		}

		nodes_space.selectElements(H5S_SELECT_SET, 1, &nodes_position);
		nodes_set.write((const void*) &nd, node_type_, H5::DataSpace(H5S_SCALAR), nodes_space);
		++nodes_position;
		
		increment_progress();
	}
	});
}



template<class Structure> template<class Iterator>
void tree_structure_hdf_loader_base<Structure>::write_points_(Iterator pt_begin, Iterator pt_end, H5::DataSet& data_set) {
	const std::size_t maximal_chunk_size = 1000;
	
	hsize_t total = pt_end - pt_begin;
	hsize_t remaining = total;
	hsize_t offset = 0;
	
	H5::DataSpace data_space = data_set.getSpace();
	
	Iterator pt = pt_begin;
	while(remaining) {
		point chunk[maximal_chunk_size];
		
		hsize_t chunk_size = maximal_chunk_size;
		if(remaining < chunk_size) chunk_size = remaining;
		Iterator pt_chunk_end = pt + chunk_size;
		
		point* chunk_pt = chunk;
		while(pt != pt_chunk_end) *(chunk_pt++) = *(pt++);
		
		H5::DataSpace mem_space(1, &chunk_size);
		data_space.selectHyperslab(H5S_SELECT_SET, &chunk_size, &offset);
		data_set.write((const void*) chunk, point_type_, mem_space, data_space);
		
		remaining -= chunk_size;
		pt += chunk_size;
	}
}


template<class Structure>
void tree_structure_hdf_loader_base<Structure>::write_points_(const point* pt_begin, const point* pt_end, H5::DataSet& data_set) {
	hsize_t total = pt_end - pt_begin;
	H5::DataSpace mem_space(1, &total);
	data_set.write((const void*) pt_begin, point_type_, mem_space, data_set.getSpace());
}



}

#endif
