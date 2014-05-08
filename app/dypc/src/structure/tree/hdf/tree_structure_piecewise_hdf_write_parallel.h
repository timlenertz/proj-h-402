#ifndef DYPC_TREE_STRUCTURE_PIECEWISE_HDF_WRITE_PARALLEL_H_
#define DYPC_TREE_STRUCTURE_PIECEWISE_HDF_WRITE_PARALLEL_H_

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
void write_to_hdf_parallel(const std::string& filename, tree_structure_piecewise<Splitter, Levels, PointsContainer, PiecesSplitter>& s) {

}

}

#endif
