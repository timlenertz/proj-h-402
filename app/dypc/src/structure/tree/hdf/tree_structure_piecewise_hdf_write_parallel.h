#ifndef DYPC_TREE_STRUCTURE_PIECEWISE_HDF_WRITE_PARALLEL_H_
#define DYPC_TREE_STRUCTURE_PIECEWISE_HDF_WRITE_PARALLEL_H_

#include "../tree_structure_piecewise.h"
#include "tree_structure_piecewise_hdf_write.h"
#include "tree_structure_hdf_file.h"
#include "../../../progress.h"
#include <string>
#include <vector>
#include <functional>
#include <array>
#include <mutex>
#include <thread>
#include <atomic>

namespace dypc {

/**
 * Write tree structure to HDF file using parallel processing.
 * Uses piecewise tree structure: The entire structure (all pieces) will get written to HDF, but only
 * a few piece is loaded into memory at a time and processed simulteneously. Saves less memory than
 * the sequential version, but benefits from multiprocessing.
 * @tparam Splitter Splitter that defined the tree structure.
 * @tparam Levels Number of mipmap levels.
 * @tparam PointsCountainer Container used to hold arrays (std::vector, std::deque)
 * @param filename Path of HDF file
 * @param s The piecewise tree structure.
 * @param number_of_threads Number of simultaneous threads.
 */
template<class Splitter, std::size_t Levels, class PointsContainer, class PiecesSplitter>
void write_to_hdf_parallel(const std::string& filename, tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>& s, std::size_t number_of_threads = std::thread::hardware_concurrency()) {
	if(number_of_threads <= 1) return write_to_hdf(filename, s);
	
	// Some type definitions...
	using Structure = tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>;
	using single_piece_structure_t = tree_structure<Splitter, Levels, PointsContainer>;
	
	using file_t = tree_structure_hdf_file<Levels, Splitter::number_of_node_children>;
	using hdf_node = typename file_t::hdf_node;
	using structure_node = typename tree_structure_piecewise<Splitter, Levels, PointsContainer>::node; // Node in tree inside a piece
	using piece_node = typename tree_structure_piecewise<Splitter, Levels, PointsContainer>::piece_node; // Node in pieces tree
	
	using point_data_offsets_t = std::array<std::ptrdiff_t, Levels>; // Stores offsets in the L point sets
				
	file_t file(filename, s.total_number_of_points()); // The output file. Total number of points is known.
	std::mutex file_mutex; // Only one thread may write into file at a time.
	
	std::vector<hdf_node> all_hdf_nodes; // Will contain full nodes list for file. (Combined from pieces tree + trees in each piece)
	
	// Task to add nodes+points from one piece
	struct add_piece_task_base {
		explicit add_piece_task_base(const piece_node& nd) : node(nd) { }

		const piece_node& node; // The node of the piece
		std::ptrdiff_t parent_node_entry_offset; // Index of entry for its parent piece in all_hdf_nodes. -1 if none (--> 1 piece only)
		std::ptrdiff_t parent_node_child; // parent_node_entry_offset.child(parent_node_child) = node
		point_data_offsets_t point_data_offsets; // Offset in file's point data sets where points of this piece should be written
		std::vector<hdf_node> output_piece_nodes; // On task completion: Nodes inside tree.
		// The child offsets in output_piece_nodes local output_piece_nodes.
		// When adding to all_hdf_nodes, these indices, and the references from the pieces tree, need to be updated
	};
	// Adds taken flag, and needed copy constructor
	struct add_piece_task : public add_piece_task_base {
		std::atomic_bool taken = false; // Whether has task has already been taken by a thread or has already finished

		explicit add_piece_task(const piece_node& nd) : add_piece_task_base(nd), taken(false) { }
		add_piece_task(const add_piece_task& t2) : add_piece_task_base(t2), taken(t2.taken.load()) { }
	};
	
	std::vector<add_piece_task> scheduled_tasks; // Tasks to do
	
	
	// Function: Recursively add a node inside tree of one piece. 
	std::function<std::ptrdiff_t(const structure_node&,const cuboid&,unsigned,std::vector<hdf_node>&,point_data_offsets_t&)> add_node =
	[&add_node](const structure_node& nd, const cuboid& cub, unsigned depth, std::vector<hdf_node>& hdf_nodes, point_data_offsets_t& points_offsets)->std::ptrdiff_t {
		std::ptrdiff_t this_node_offset = hdf_nodes.size();
		hdf_nodes.push_back(hdf_node());
		auto hn = hdf_nodes.begin() + this_node_offset;
		hn->cuboid_origin = cub.origin;
		hn->cuboid_extremity = cub.extremity;
		for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) {
			// The point data is located in depth-first order, at global offset given by points_offsets
			auto n = nd.number_of_points(lvl);
			hn->data_start[lvl] = points_offsets[lvl];
			hn->data_length[lvl] = nd.number_of_points(lvl);
		}
		if(nd.is_leaf()) {
			for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) hn->children[i] = 0; // no children
			for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) points_offsets[lvl] += nd.number_of_points(lvl); //  count points only once (---> in leaf)
		} else {
			for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) {
				cuboid child_cub = Splitter::node_child_cuboid(i, cub, nd.get_points_information(), depth);
				auto child_offset = add_node(nd.child(i), child_cub, depth + 1, hdf_nodes, points_offsets);
				hn = hdf_nodes.begin() + this_node_offset; // Iterator might have been invalidated
				hn->children[i] = child_offset; // now relative to hdf_nodes. when copying to all_hdf_nodes in postprocessing, will need to add global offset of nodes list
			}
		}
		return this_node_offset;
	};
	
	
	// Traverse pieces tree, and for leaves, schedule task for them to be added.
	std::function<std::ptrdiff_t(const piece_node&,std::ptrdiff_t,std::ptrdiff_t,point_data_offsets_t&)> add_piece_node_and_schedule =
	[&](const piece_node& nd, std::ptrdiff_t p_nd_entry_off, std::ptrdiff_t p_nd_chld, point_data_offsets_t& points_offsets)->std::ptrdiff_t {
		if(nd.is_leaf()) {
			// Piece is leaf; schedule task to add nodes+points in that piece
			add_piece_task task(nd);
			task.parent_node_entry_offset = p_nd_entry_off;
			task.parent_node_child = p_nd_chld;
			task.point_data_offsets = points_offsets;
			scheduled_tasks.push_back(task);
						
			for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) points_offsets[lvl] += s.piece_number_of_points(nd, lvl);
			
		} else {
			// Pieces tree node is not a leaf. 
			
			// Remember current offset in points set
			// (will be incremented as points are added in recursive calls)
			auto old_points_offsets = points_offsets;

			// Add entry for this node.
			std::ptrdiff_t this_node_offset = all_hdf_nodes.size();
			all_hdf_nodes.push_back(hdf_node());
			auto hn = all_hdf_nodes.begin() + this_node_offset;
			const cuboid& cub = nd.get_cuboid();
			hn->cuboid_origin = cub.origin;
			hn->cuboid_extremity = cub.extremity;
			for(std::ptrdiff_t i = 0; i < Splitter::number_of_node_children; ++i) hn->children[i] = 0;

			// Recursively add child nodes after hn, and store the children's offsets in hn
			for(std::ptrdiff_t i = 0; i < PiecesSplitter::number_of_node_children; ++i) {
				if(! nd.has_child(i)) continue;

				// Recursive call. Adds nodes, and schedules points and nodes inside piece to be added.
				// Moves points offsets.
				auto child_offset = add_piece_node_and_schedule(nd.child(i), this_node_offset, i, points_offsets);
				hn = all_hdf_nodes.begin() + this_node_offset; // Iterator might have been invalidated

				if(child_offset) hn->children[i] = child_offset;				
			}
			
			// Make node to segment of points inside that node
			for(std::ptrdiff_t lvl = 0; lvl < Levels; ++lvl) {
				hn->data_start[lvl] = old_points_offsets[lvl];
				hn->data_length[lvl] = points_offsets[lvl] - old_points_offsets[lvl];
			}
			
			return this_node_offset;
		}
	};
	
	
	// Load a piece, and add its nodes tree, points
	// Function designed so that multiple instances can be run simulataneously. (used in parallel version)
	// Loads piece into local variable, then only works on local variables. (except writing to output file --> add mutex in par)
	// Model class designed to allow multiple reading threads. (see model/model.h)
	auto execute_add_piece_task =
	[&file, &file_mutex, &add_node](const Structure& piecewise_s, add_piece_task& task) {
	progress("Adding tree structure piece " + std::to_string(task.node.get_id()) + "...", [&](progress_handle& pr) {
		file_mutex.lock();
		single_piece_structure_t s = piecewise_s.load_and_export_piece(task.node);
		file_mutex.unlock();
		
		// Write all points to the file...
		const auto& pts = s.points_at_level(0);
		file.write_points(pts.begin(), pts.end(), 0, task.point_data_offsets[0]);
		
		for(std::ptrdiff_t lvl = 1; lvl < Levels; ++lvl) {
			s.load_downsampled_points(lvl);
			const auto& pts = s.points_at_level(lvl);
			file_mutex.lock();
			file.write_points(pts.begin(), pts.end(), lvl, task.point_data_offsets[lvl]);
			file_mutex.unlock();
			s.unload_downsampled_points(lvl);
		}

		point_data_offsets_t init_points_offsets = task.point_data_offsets;
		add_node(s.root_node(), s.root_cuboid(), 0, task.output_piece_nodes, init_points_offsets);
	});
	};
	
	
	
	// procedure:
	
	// First, recursively traverse pieces tree, and add corresponding nodes entries in all_hdf_nodes.
	// For leaves, create an add_piece_task, and assign its point set a segment in the hdf file.
	// The exact size of downsampled sets is known without loading the piece.
	point_data_offsets_t init_points_offsets;
	init_points_offsets.fill(0);
	add_piece_node_and_schedule(s.root_piece_node(), -1, -1, init_points_offsets);
	
	// Execute the scheduled tasks.
	// Will write points directly to assigned location in file, and nodes in local array, with local offsets
	// MAIN DIFFERENCE FROM SEQUENTIAL VERSION: spawn multiple threads
	std::vector<std::thread> threads;
	for(unsigned i = 0; i < number_of_threads; ++i) threads.emplace_back([&]() {
		// Inside one of the threads.
		// Choose a task that is still available
		bool none_left = false;
		while(! none_left) { // iterate through tasks till no more task is available
			none_left = true;
			for(add_piece_task& task : scheduled_tasks) {
				bool expected = false; // not taken
				if(task.taken.compare_exchange_strong(expected, true)) { // atomoic
					// Was not taken, now taken by this thread.
					// task if now marked as taken. execute it in this thread
					execute_add_piece_task(s, task);
					none_left = false;
				}
			}
			// no more task is available, exit thread.
			// other threads may still be working on a task, and will also exit afterwards
		}
	});
	// wait until all threads have exited
	for(std::thread& thr : threads) thr.join();
	
	for(const add_piece_task& task : scheduled_tasks) if(! task.taken.load()) throw std::runtime_error("Something went wrong");
	
	// Postprocessong:
	for(add_piece_task& task : scheduled_tasks) {
		std::ptrdiff_t offset = all_hdf_nodes.size(); // Offset of the local nodes tree from this task,in the global tree. (They are concatenated)
		for(hdf_node& entry : task.output_piece_nodes) {
			// Add the offset to child references. Except if 0 = no child.
			for(auto& child_off : entry.children) if(child_off) child_off += offset;
		}
		// Append the adjusted local nodes list.
		all_hdf_nodes.insert(all_hdf_nodes.end(), task.output_piece_nodes.begin(), task.output_piece_nodes.end());
		if(task.parent_node_entry_offset != -1) {
			// Adjust the child reference of the piece node that was pointing to this
			// Cannot be done in advance because size of nodes tree of piece is now known without loading the piece.
			all_hdf_nodes[task.parent_node_entry_offset].children[task.parent_node_child] = offset;
		}
	}
	
	// Finally write the nodes
	file.write_nodes(all_hdf_nodes.begin(), all_hdf_nodes.end());
}

}

#endif
