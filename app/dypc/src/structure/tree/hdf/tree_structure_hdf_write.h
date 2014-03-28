#ifndef DYPC_TREE_STRUCTURE_HDF_WRITE_H_
#define DYPC_TREE_STRUCTURE_HDF_WRITE_H_

#include "../tree_structure.h"
#include "tree_structure_hdf_file.h"
#include <string>
#include <memory>

namespace dypc {
/*
template<class Splitter, std::size_t Levels, class PointsContainer>
void write_to_hdf(const std::string& filename, const tree_structure<Splitter, Levels, PointsContainer>& s) {
	using Structure = tree_structure<Splitter, Levels, PointsContainer>;
	tree_structure_hdf_file<Structure> file(filename, s.number_of_nodes(), s.number_of_points());	
	
	progress_for(0, Structure::levels, "Writing points to HDF...", [&](std::ptrdiff_t lvl) {
		const auto& pts = s.points_at_level(lvl);
		file.write_points(pts.begin(), pts.end(), lvl);
	});


	using structure_node_t = typename Structure::node;
	struct entry {
		const structure_node_t& node;
		cuboid node_cuboid;
		unsigned depth;
	};
	
	hsize_t nodes_position = 0;
	
	std::stack<entry> stack_;
	stack_.push({ s.root_node(), s.root_cuboid(), 0 });
	
	progress(s.number_of_nodes(), "Writing nodes to HDF...", [&](progress_handle& pr) {
		using hdf_node = typename tree_structure_hdf_file<Structure>::hdf_node;
		std::unique_ptr<hdf_node[]> nodes_(new hdf_node[s.number_of_nodes()]);
		hdf_node* current = nodes_.get();
		
		while(! stack_.empty()) {
			auto e = stack_.top();
			stack_.pop();
		
			auto node_number_of_points = e.node.number_of_points();
	
			hdf_node& nd = *(current++);
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
			
			++nodes_position;
			pr.increment();
		}
		file.write_nodes(nodes_.get(), current);
	});
}
*/

}

#endif
