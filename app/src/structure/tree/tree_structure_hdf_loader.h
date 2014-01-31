#ifndef DYPC_TREE_STRUCTURE_HDF_LOADER_H_
#define DYPC_TREE_STRUCTURE_HDF_LOADER_H_

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
class tree_structure_hdf_loader : public tree_structure_loader, public tree_structure_hdf_loader_base {
private:
	struct hdf_node {
		std::uint32_t data_start;
		std::uint32_t data_length;
		glm::vec3 cuboid_origin;
		glm::vec3 cuboid_sides;
		std::uint32_t children[Structure::number_of_node_children];
		
		bool is_leaf() const { return (children[0] == 0); }
		cuboid node_cuboid() const { return cuboid(cuboid_origin, cuboid_sides); }
	};
	
	static const H5::CompType node_type_;
	
	static H5::CompType initialize_node_type_();	

	H5::H5File file_;
	H5::DataSet points_data_set_;
	H5::DataSpace points_data_space_;

	std::unique_ptr<hdf_node[]> nodes_;
	std::size_t number_of_nodes_ = 0;

public:
	static void write(const std::string& file, const Structure&);
	
	explicit tree_structure_hdf_loader(const std::string& file);
	
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
		points_data_set_.getSpace().getSimpleExtentDims(dims);
		return dims[0];
	}
};


template<class Structure>
const H5::CompType tree_structure_hdf_loader<Structure>::node_type_ = tree_structure_hdf_loader<Structure>::initialize_node_type_();


template<class Structure>
H5::CompType tree_structure_hdf_loader<Structure>::initialize_node_type_() {
	H5::CompType t(sizeof(hdf_node));
	hsize_t three_dims[] = { 3 };
	hsize_t child_dims[] = { Structure::number_of_node_children };
	t.insertMember("data_start", HOFFSET(hdf_node, data_start), H5::PredType::NATIVE_UINT32);
	t.insertMember("data_length", HOFFSET(hdf_node, data_length), H5::PredType::NATIVE_UINT32);
	t.insertMember("cuboid_origin", HOFFSET(hdf_node, cuboid_origin), H5::ArrayType(H5::PredType::NATIVE_FLOAT, 1, three_dims));
	t.insertMember("cuboid_sides", HOFFSET(hdf_node, cuboid_sides), H5::ArrayType(H5::PredType::NATIVE_FLOAT, 1, three_dims));
	t.insertMember("children", HOFFSET(hdf_node, children), H5::ArrayType(H5::PredType::NATIVE_UINT32, 1, child_dims));
	return t;
}


template<class Structure>
tree_structure_hdf_loader<Structure>::tree_structure_hdf_loader(const std::string& filename) : file_(filename, 0) {
	auto nodes_set = file_.openDataSet("nodes");
	auto nodes_space = nodes_set.getSpace();
	
	number_of_nodes_ = nodes_space.getSimpleExtentNpoints();
	nodes_.reset(new hdf_node [number_of_nodes_]);
	nodes_set.read((void*) nodes_.get(), node_type_);
	
	points_data_set_ = file_.openDataSet("points");
	points_data_space_ = points_data_set_.getSpace();
}
	


template<class Structure>
std::size_t tree_structure_hdf_loader<Structure>::extract_points_(point_buffer_t buf, std::size_t capacity, const loader::request_t& req) {
	std::size_t total = 0;
	std::size_t remaining = capacity;

	std::stack<const hdf_node*> stack_;
	stack_.push(nodes_.get());
	
	while(! stack_.empty()) {
		const hdf_node& node = * stack_.top();
		stack_.pop();
		
		std::ptrdiff_t action = action_for_node_(node.node_cuboid(), node.data_length, node.is_leaf(), req, Structure::levels);
		if(action == action_split) {
			for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) stack_.push(nodes_.get() + node.children[i]);
		} else if(action >= 0) {
			hsize_t lvl = action;
			if(lvl >= Structure::levels) lvl = Structure::levels - 1;

			hsize_t count[] = { node.data_length, 1 };
			hsize_t start[] = { node.data_start, lvl };
			points_data_space_.selectHyperslab(H5S_SELECT_SET, count, start);

			hsize_t dims[] = { node.data_length };
			H5::DataSpace mem_space(1, dims);

			point* mem_buf = new point [node.data_length];
			const point* mem_buf_end = mem_buf + node.data_length;
			points_data_set_.read(mem_buf, point_type_, mem_space, points_data_space_);

			for(const point* pt = mem_buf; (pt != mem_buf_end) && remaining; ++pt) {
				if(! *pt) continue;
				*(buf++) = *pt;
				++total;
				--remaining;
			}
			
			delete[] mem_buf;
		}
	}
	
	return total;
}


template<class Structure>
void tree_structure_hdf_loader<Structure>::write(const std::string& filename, const Structure& s) {
	H5::H5File file(filename, H5F_ACC_TRUNC);
	
	hsize_t points_dims[] = { s.number_of_points(), Structure::levels };
	H5::DataSpace points_space(2, points_dims);
	H5::DataSet points_set = file.createDataSet("points", point_type_, points_space);

	hsize_t nodes_dims[] = { s.number_of_nodes() };
	H5::DataSpace nodes_space(1, nodes_dims);
	H5::DataSet nodes_set = file.createDataSet("nodes", node_type_, nodes_space);

	using structure_node_t = typename Structure::node;
	struct entry {
		const structure_node_t& node;
		cuboid node_cuboid;
	};
	
	hsize_t points_position = 0;
	hsize_t nodes_position = 0;
	
	std::stack<entry> stack_;
	stack_.push({ s.root_node(), s.root_cuboid() });
	
	progress("Writing to HDF...", s.number_of_nodes(), 1000, [&]() {
	while(! stack_.empty()) {
		auto e = stack_.top();
		stack_.pop();
	
		auto node_number_of_points = e.node.number_of_points();

		hdf_node nd;
		nd.data_start = points_position;
		nd.data_length = node_number_of_points;
		nd.cuboid_origin = e.node_cuboid.origin();
		nd.cuboid_sides = e.node_cuboid.side_lengths();

		if(e.node.is_leaf()) {
			for(hsize_t lvl = 0; lvl < Structure::levels; ++lvl) {
				const point* level_points = e.node.points_begin(lvl);
				const point* level_points_end = e.node.points_end(lvl);
				hsize_t level_number_of_points = level_points_end - level_points;
				
				hsize_t mem_dims[] = { level_number_of_points };
				H5::DataSpace mem_space(1, mem_dims);
				
				hsize_t count[2] = { level_number_of_points, 1 };
				hsize_t start[2] = { points_position, lvl };
				points_space.selectHyperslab(H5S_SELECT_SET, count, start);
				points_set.write((const void*) level_points, point_type_, mem_space, points_space);
			}
			for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) nd.children[i] = 0;
			points_position += node_number_of_points;
			
		} else {
			std::size_t child_node_offset = 0;
			for(std::ptrdiff_t i = 0; i < Structure::number_of_node_children; ++i) {				
				nd.children[i] = nodes_position + 1 + child_node_offset;
				child_node_offset += e.node.child(i).number_of_nodes_in_branch();
			}
			for(std::ptrdiff_t i = Structure::number_of_node_children - 1; i >= 0; --i) {
				auto child_cuboid = Structure::splitter::node_child_cuboid(i, e.node_cuboid);
				stack_.push({ e.node.child(i), child_cuboid });
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
