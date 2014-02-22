#ifndef DYPC_TREE_STRUCTURE_PIECEWISE_HDF_WRITE_H_
#define DYPC_TREE_STRUCTURE_PIECEWISE_HDF_WRITE_H_

#include "tree_structure_piecewise.h"
#include "tree_structure_hdf_file.h"
#include <string>
#include <vector>
#include <functional>
#include <array>

namespace dypc {


template<class Splitter, std::size_t Levels, class PointsContainer>
void write_to_hdf_piecewise(const std::string& filename, tree_structure_piecewise<Splitter, Levels, PointsContainer>& s) {
	using Structure = tree_structure_piecewise<Splitter, Levels, PointsContainer>;
	using hdf_node = typename tree_structure_hdf_file<Structure>::hdf_node;
	using structure_node = typename tree_structure_piecewise<Splitter, Levels, PointsContainer>::node;
	using piece_node = typename tree_structure_piecewise<Splitter, Levels, PointsContainer>::piece_node;
	using piece = typename tree_structure_piecewise<Splitter, Levels, PointsContainer>::piece;	
	
	std::vector<hdf_node> hdf_nodes;
	
	std::array<std::ptrdiff_t, Splitter::number_of_node_children> data_output_offsets, data_tree_offsets;
	data_output_offsets.fill(0); data_tree_offsets.fill(0);
	
	tree_structure_hdf_file<Structure> file(filename, s.total_number_of_nodes_upper_bound(), s.total_number_of_points());
	
	std::function<void(const structure_node&, const cuboid&, unsigned)> add_structure_node = [&](const structure_node& nd, const cuboid& cub, unsigned depth)->void {
		if(nd.is_empty()) return;

		auto old_data_offsets = data_tree_offsets;
		
		hdf_nodes.push_back(hdf_node());
		hdf_node& hn = hdf_nodes.back();

		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) hn.children[i] = 0;
		hn.cuboid_origin = cub.origin();
		hn.cuboid_sides = cub.side_lengths();
		
		if(nd.is_leaf()) {
			for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) data_tree_offsets[lvl] += nd.number_of_points(lvl);
		} else {
			for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) {
				typename Splitter::node_points_information no_info;
				cuboid child_cub = Splitter::node_child_cuboid(i, cub, no_info, depth);
				auto old_nodes_count = hdf_nodes.size();
				add_structure_node(nd.child(i), child_cub, depth + 1);
				hn.children[i] = old_nodes_count;
			}
		}
		
		for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) {
			hn.data_start[lvl] = old_data_offsets[lvl];
			hn.data_length[lvl] = data_tree_offsets[lvl] - old_data_offsets[lvl];
		}
	};
	
	
	
	auto add_piece = [&](const piece& p, unsigned depth)->void {
		s.load_piece(p);
		
		//progress("Writing points to HDF...", Levels, 1, [&]() {
		for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) {
			const auto& pts = s.points_at_level(lvl);
			file.write_points(pts.begin(), pts.end(), lvl, data_output_offsets[lvl]);
			data_output_offsets[lvl] += pts.size();		
			increment_progress();
		}
		//});
		
		add_structure_node(s.root_node(), s.root_cuboid(), depth); 
	};
	
	
	
	std::function<void(const piece_node&, const cuboid&, unsigned)> add_piece_node = [&](const piece_node& nd, const cuboid& cub, unsigned depth)->void {		
		auto old_data_offsets = data_tree_offsets;
		
		if(nd.is_leaf()) {			
			const piece& p = nd.get_piece();
			add_piece(p, depth);
		} else {
			hdf_nodes.push_back(hdf_node());
			hdf_node& hn = hdf_nodes.back();
			
			hn.cuboid_origin = cub.origin();
			hn.cuboid_sides = cub.side_lengths();
			
			for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) {
				if(nd.has_child(i)) {
					typename Splitter::node_points_information no_info;
					cuboid child_cub = Splitter::node_child_cuboid(i, cub, no_info, depth);
					auto old_nodes_count = hdf_nodes.size();
					add_piece_node(nd.child(i), child_cub, depth + 1);
					hn.children[i] = old_nodes_count;
					if(depth == 0) std::cout << i << ": " << old_nodes_count << std::endl;
				} else {
					hn.children[i] = 0;
				}
			}
			
			for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) {
				hn.data_start[lvl] = old_data_offsets[lvl];
				hn.data_length[lvl] = data_tree_offsets[lvl] - old_data_offsets[lvl];
			}
		}
		

	};
	
	add_piece_node(s.root_piece_node(), s.root_piece_cuboid(), 0);
		
	//file.write_nodes(hdf_nodes.begin(), hdf_nodes.end());
}


}

#endif
