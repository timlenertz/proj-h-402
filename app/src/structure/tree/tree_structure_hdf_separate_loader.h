#ifndef DYPC_TREE_STRUCTURE_HDF_SEPARATE_LOADER_H_
#define DYPC_TREE_STRUCTURE_HDF_SEPARATE_LOADER_H_

#include "tree_structure_loader.h"
#include "tree_structure_hdf_loader_base.h"
#include "tree_structure.h"
#include <utility>
#include <memory>
#include <string>
#include <stack>
#include <deque>


namespace dypc {

template<class Structure>
class tree_structure_hdf_separate_loader : public tree_structure_loader, public tree_structure_hdf_loader_base {
private:
	struct hdf_node {
		std::uint32_t data_start[Structure::levels];
		std::uint32_t data_length[Structure::levels];
		glm::vec3 cuboid_origin;
		glm::vec3 cuboid_sides;
		std::uint32_t children[Structure::number_of_node_children];
		
		bool is_leaf() const { return (children[0] == 0); }
		cuboid node_cuboid() const { return cuboid(cuboid_origin, cuboid_sides); }
	};
	
	static const H5::CompType node_type_;

	H5::H5File file_;
	H5::DataSet points_data_set_[Structure::levels];
	H5::DataSpace points_data_space_[Structure::levels];

	std::unique_ptr<hdf_node[]> nodes_;
	std::size_t number_of_nodes_ = 0;
	
	static std::string points_set_name_(std::ptrdiff_t lvl) {
		return std::string("points") + std::to_string(lvl);
	}

public:
	static void write(const std::string& file, const Structure&);
	static H5::CompType initialize_node_type();	
	
	explicit tree_structure_hdf_separate_loader(const std::string& file);
	
protected:
	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) override;
	
	
	std::size_t memory_size_() const override {
		return number_of_nodes_ * sizeof(hdf_node);
	}
	
	std::size_t file_size_() const override {
		return file_.getFileSize();
	}
	
	std::size_t total_points_() const override {
		hsize_t dims[2];
		points_data_set_[0].getSpace().getSimpleExtentDims(dims);
		return dims[0];
	}
};


template<class Structure>
const H5::CompType tree_structure_hdf_separate_loader<Structure>::node_type_ = tree_structure_hdf_separate_loader<Structure>::initialize_node_type();


template<class Structure>
H5::CompType tree_structure_hdf_separate_loader<Structure>::initialize_node_type() {
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
tree_structure_hdf_separate_loader<Structure>::tree_structure_hdf_separate_loader(const std::string& filename) : file_(filename, 0) {
	auto nodes_set = file_.openDataSet("nodes");
	auto nodes_space = nodes_set.getSpace();
	
	number_of_nodes_ = nodes_space.getSimpleExtentNpoints();
	nodes_.reset(new hdf_node [number_of_nodes_]);
	nodes_set.read((void*) nodes_.get(), node_type_);
	
	for(std::ptrdiff_t lvl = 0; lvl < Structure::levels; ++lvl) {
		points_data_set_[lvl] = file_.openDataSet(points_set_name_(lvl));
		points_data_space_[lvl] = points_data_set_[lvl].getSpace();		
	}
}
	


template<class Structure>
std::size_t tree_structure_hdf_separate_loader<Structure>::extract_points_(point_buffer_t buf, std::size_t capacity, const loader::request_t& req) {
	std::size_t total = 0;
	std::size_t remaining = capacity;

	std::stack<const hdf_node*> stack_;
	stack_.push(nodes_.get());
	
	while(! stack_.empty() && total < capacity) {
		const hdf_node& node = * stack_.top();
		stack_.pop();
		
		std::ptrdiff_t action = action_for_node_(node.node_cuboid(), node.data_length[0], node.is_leaf(), req, Structure::levels);
		if(action == action_split) {
			for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) stack_.push(nodes_.get() + node.children[i]);
		} else if(action >= 0) {
			hsize_t lvl = action;
			if(lvl >= Structure::levels) lvl = Structure::levels - 1;
			
			hsize_t read_number = node.data_length[lvl];
			if(read_number > remaining) read_number = remaining;

			hsize_t count[] = { read_number };
			hsize_t start[] = { node.data_start[lvl] };
			points_data_space_[lvl].selectHyperslab(H5S_SELECT_SET, count, start);

			hsize_t dims[] = { read_number };
			H5::DataSpace mem_space(1, dims);
			
			points_data_set_[lvl].read(buf, point_type_, mem_space, points_data_space_[lvl]);
			buf += read_number;
			total += read_number;
			remaining -= read_number;
		}
	}
	
	return total;
}


template<class Structure>
void tree_structure_hdf_separate_loader<Structure>::write(const std::string& filename, const Structure& s) {
	H5::H5File file(filename, H5F_ACC_TRUNC);
	
	H5::DataSpace points_space[Structure::levels];
	H5::DataSet points_set[Structure::levels];
	
	progress("Writing points to HDF...", Structure::levels, 1, [&]() {
	for(std::ptrdiff_t lvl = 0; lvl < Structure::levels; ++lvl) {
		hsize_t points_dims[] = { s.number_of_points(lvl) };
		points_space[lvl] = H5::DataSpace(1, points_dims);
		points_set[lvl] = file.createDataSet(points_set_name_(lvl), point_type_, points_space[lvl]);
		
		const auto& all_points = s.points_at_level(lvl);
		points_set[lvl].write((const void*) all_points.data(), point_type_, points_space[lvl], points_space[lvl]);
		
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
			if(e.node.number_of_points(lvl)) nd.data_start[lvl] = e.node.points_begin(lvl) - s.points_at_level(lvl).data();
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



}

#endif
