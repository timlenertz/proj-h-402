#ifndef DYPC_TREE_STRUCTURE_HDF_SIMPLE_LOADER_H_
#define DYPC_TREE_STRUCTURE_HDF_SIMPLE_LOADER_H_
/*
#include "tree_structure_loader.h"
#include "tree_structure.h"
#include <utility>
#include <memory>
#include <string>
#include <stack>
#include <deque>


namespace dypc {

template<class Structure>
class tree_structure_hdf_simple_loader : public tree_structure_loader, public tree_structure_hdf_loader_base<Structure> {
private:
	using typename tree_structure_hdf_loader_base<Structure>::hdf_node;
	using tree_structure_hdf_loader_base<Structure>::node_type_;
	using tree_structure_hdf_loader_base<Structure>::point_type_;
	using tree_structure_hdf_loader_base<Structure>::points_set_name_;

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
	
	explicit tree_structure_hdf_simple_loader(const std::string& file);
	
	std::string loader_name() const override { return Structure::structure_name() + " HDF Simple Loader"; }
	
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
tree_structure_hdf_simple_loader<Structure>::tree_structure_hdf_simple_loader(const std::string& filename) : file_(filename, 0) {
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
std::size_t tree_structure_hdf_simple_loader<Structure>::extract_points_(point_buffer_t buf, std::size_t capacity, const loader::request_t& req) {
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


}
*/
#endif
