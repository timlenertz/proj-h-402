#ifndef DYPC_TREE_STRUCTURE_PIECEWISE_HDF_WRITE_H_
#define DYPC_TREE_STRUCTURE_PIECEWISE_HDF_WRITE_H_

#include "../tree_structure_piecewise.h"
#include "tree_structure_hdf_file.h"
#include "../../../progress.h"
#include <string>
#include <vector>
#include <functional>
#include <array>
#include <atomic>
#include <thread>

namespace dypc {

/**
 * Write tree structure to HDF file.
 * Uses piecewise tree structure: The entire structure (all pieces) will get written to HDF, but only
 * one piece is loaded into memory at a time. Limits memory usage, and allows for huge models to be
 * written into the file.
 * @tparam Splitter Splitter that defined the tree structure.
 * @tparam Levels Number of mipmap levels.
 * @tparam PointsCountainer Container used to hold arrays (std::vector, std::deque)
 * @param filename Path of HDF file
 * @param s The piecewise tree structure.
 */
template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
void write_to_hdf(const std::string& filename, tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>& s) {
	using Structure = tree_structure_piecewise<Splitter, Levels, PointsContainer>;
	using file_t = tree_structure_hdf_file<Levels, Splitter::number_of_node_children>;
	using hdf_node = typename file_t::hdf_node;
	using structure_node = typename tree_structure_piecewise<Splitter, Levels, PointsContainer>::node;
	using piece_node = typename tree_structure_piecewise<Splitter, Levels, PointsContainer>::piece_node;
	
	std::vector<hdf_node> hdf_nodes;
	
	std::array<std::ptrdiff_t, Levels> data_output_offsets, data_tree_offsets;
	data_output_offsets.fill(0); data_tree_offsets.fill(0);
		
	file_t file(filename, s.total_number_of_points());
	
	progress_handle* root_progress_handle = nullptr;
	
	std::function<bool(const structure_node&, const cuboid&, unsigned)> add_structure_node = [&](const structure_node& nd, const cuboid& cub, unsigned depth)->bool {		
		if(nd.number_of_points() == 0) return false;
		
		auto old_data_offsets = data_tree_offsets;
		auto old_nodes_count = hdf_nodes.size();
		
		hdf_nodes.push_back(hdf_node());
		auto hn = hdf_nodes.begin() + old_nodes_count;

		for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) hn->children[i] = 0;
		hn->cuboid_origin = cub.origin;
		hn->cuboid_sides = cub.side_lengths();
		
		if(nd.is_leaf()) {
			for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) data_tree_offsets[lvl] += nd.number_of_points(lvl);
		} else {
			for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) {
				typename Splitter::node_points_information no_info;
				cuboid child_cub = Splitter::node_child_cuboid(i, cub, no_info, depth);
				auto previous_nodes_count = hdf_nodes.size();
				bool has_child_points = add_structure_node(nd.child(i), child_cub, depth + 1);
				hn = hdf_nodes.begin() + old_nodes_count;
				if(has_child_points) hn->children[i] = previous_nodes_count;
			}
		}
		
		for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) {
			hn->data_start[lvl] = old_data_offsets[lvl];
			hn->data_length[lvl] = data_tree_offsets[lvl] - old_data_offsets[lvl];
		}
		
		return true;
	};
	
	
	std::function<void(const piece_node&)> add_piece_node = [&](const piece_node& nd) {
		if(nd.is_leaf()) {
			s.load_piece(nd);
			
			auto n = s.number_of_points();
			if(n == 0) return;
			
			root_progress_handle->increment(n);
			
			const auto& pts = s.points_at_level(0);
			file.write_points(pts.begin(), pts.end(), 0, data_output_offsets[0]);
			data_output_offsets[0] += pts.size();		
	
			for(std::ptrdiff_t lvl = 1; lvl < Levels; ++lvl) {
				s.load_downsampled_points(lvl);
				const auto& pts = s.points_at_level(lvl);
				file.write_points(pts.begin(), pts.end(), lvl, data_output_offsets[lvl]);
				data_output_offsets[lvl] += pts.size();		
				s.unload_downsampled_points(lvl);
			}
			
			add_structure_node(s.root_node(), s.root_cuboid(), nd.get_depth()); 

		} else {
			auto old_data_offsets = data_tree_offsets;
			
			auto old_nodes_count = hdf_nodes.size();
			hdf_nodes.push_back(hdf_node());
			auto hn = hdf_nodes.begin() + old_nodes_count;
			
			const cuboid& cub = nd.get_cuboid();
			hn->cuboid_origin = cub.origin;
			hn->cuboid_sides = cub.side_lengths();
			for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) hn->children[i] = 0;

			for(std::ptrdiff_t i = 0; i < PiecesSplitter::number_of_node_children; ++i) {
				if(! nd.has_child(i)) continue;

				auto previous_nodes_count = hdf_nodes.size();
				add_piece_node(nd.child(i));
				hn = hdf_nodes.begin() + old_nodes_count;
				hn->children[i] = previous_nodes_count;
			}
			
			for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) {
				hn->data_start[lvl] = old_data_offsets[lvl];
				hn->data_length[lvl] = data_tree_offsets[lvl] - old_data_offsets[lvl];
			}
		}
	};
	
	progress(s.total_number_of_points(), "Writing piecewise tree structure points to HDF...", [&](progress_handle& pr) {
		root_progress_handle = &pr;
		add_piece_node(s.root_piece_node());
	});
		
	file.write_nodes(hdf_nodes.begin(), hdf_nodes.end());
}


/*
template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
void write_to_hdf_parallel(const std::string& filename, tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>& s, std::size_t threads = 2) {
	using Structure = tree_structure_piecewise<Splitter, Levels, PointsContainer>;
	using file_t = tree_structure_hdf_file<Levels, Splitter::number_of_node_children>;
	using hdf_node = typename file_t::hdf_node;
	using structure_node = typename tree_structure_piecewise<Splitter, Levels, PointsContainer>::node;
	using piece_node = typename tree_structure_piecewise<Splitter, Levels, PointsContainer>::piece_node;
	using piece = typename tree_structure_piecewise<Splitter, Levels, PointsContainer>::piece;	
			
	file_t file(filename, s.total_number_of_points());
	
	using point_data_offsets_t = std::array<std::ptrdiff_t, Levels>;
	
	struct add_piece_job {
		const piece& the_piece;
		point_data_offsets_t offsets;
		std::vector<hdf_node> added_nodes;
		point_data_offsets_t actual_lengths;
		
		add_piece_job(const piece& p, const point_data_offsets_t&) :
		the_piece(p), offsets(off) {
			actual_lengths.fill(0);
		}
	};
	
	std::vector<add_piece_job> jobs;
	std::stack<add_piece_job*> scheduled_jobs;
	
	

	std::function<void(const piece_node&, const cuboid&, unsigned)> add_piece_node = [&](const piece_node& nd, const cuboid& cub, unsigned depth) {				
		if(nd.is_leaf()) {
			const piece& p = nd.get_piece();
			add_piece(p, depth);
		} else {			
			auto old_nodes_count = hdf_nodes.size();
			hdf_nodes.push_back(hdf_node());
			auto hn = hdf_nodes.begin() + old_nodes_count;
			
			hn->cuboid_origin = cub.origin();
			hn->cuboid_sides = cub.side_lengths();
			for(auto& child : hn->children) child = 0;

			typename PiecesSplitter::node_points_information no_info;				
			for(std::ptrdiff_t i = 0; i < PiecesSplitter::number_of_node_children; ++i) {
				if(! nd.has_child(i)) continue;
				
				cuboid child_cub = PiecesSplitter::node_child_cuboid(i, cub, no_info, depth);
				auto previous_nodes_count = hdf_nodes.size();
				add_piece_node(nd.child(i), child_cub, depth + 1);
				hn = hdf_nodes.begin() + old_nodes_count;
				hn->children[i] = previous_nodes_count;
			}
			
			for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) {
				hn->data_start[lvl] = old_data_offsets[lvl];
				hn->data_length[lvl] = data_tree_offsets[lvl] - old_data_offsets[lvl];
			}
		}
	};


	std::stack<piece_node> stack;
	point_data_offsets_t point_offsets; point_offsets.fill(0);
	stack.push(s.root_piece_node());
	while(! stack.empty()) {
		auto top = stack.top();
		stack.pop();
		if(top.is_leaf()) {
			jobs.emplace_back(top.get_piece(), point_offsets);
			
		} else {
			for(std::ptrdiff_t i = PiecesSplitter::number_of_node_children - 1; i >= 0; --i)
				if(top.has_child(i)) stack.push(top.child(i));
		}
	}


	progress(s.total_number_of_points(), "Writing piecewise tree structure points to HDF...", [&](progress_handle& pr) {
		add_piece_node(s.root_piece_node(), s.root_piece_cuboid(), 0);
	});
		
	file.write_nodes(hdf_nodes.begin(), hdf_nodes.end());

}
*/

}

#endif
