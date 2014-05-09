#ifndef DYPC_MIPMAP_STRUCTURE_H_
#define DYPC_MIPMAP_STRUCTURE_H_

#include "structure.h"
#include "../model/model.h"
#include "../downsampling.h"
#include "../geometry/cuboid.h"
#include <stdexcept>
#include <iterator>

namespace dypc {

/**
 * Structure that contains several downsampled points sets.
 * The downsampled point sets are called mipmaps in parts of the code. This class contains facilities to generate downsampled point sets.
 */
class mipmap_structure : public structure {
protected:
	const downsampling_mode downsampling_mode_; ///< Downsampling mode to use for tree structure.
	const std::size_t downsampling_minimum_; ///< Minimal number of points in downsampled point sets.
	const float downsampling_amount_; ///< Downsampling amount used in downsampling_ratios_.
	const downsampling_ratios_t downsampling_ratios_; ///< Downsampling ratios for the different levels.
	const bool exact_downsampling_; ///< Whether downsampling point set sizes must be predictable.

	/**
	 * Create mipmap structure.
	 * Called by subclass.
	 * @see determine_downsampling_ratio
	 * @param dlevels Number of downsampling levels.
	 * @param dmin Minimal number of points in downsampled point sets.
	 * @param damount Downsampling amount.
	 * @param dmode Downsampling mode.
	 * @param dexact If true, generate predictable number of downsampled points.
	 * @param mod The model.
	 */
	mipmap_structure(std::size_t dlevels, std::size_t dmin, float damount, downsampling_mode dmode, bool dexact, model& mod) :
	structure(mod), downsampling_mode_(dmode), downsampling_minimum_(dmin), downsampling_amount_(damount), downsampling_ratios_(determine_downsampling_ratios(dlevels, mod.number_of_points(), dmin, damount)), exact_downsampling_(dexact) { } 
	
	/**
	 * Generate downsampled point set for given level.
	 * @param pt_begin Start iterator to original point set.
	 * @param pt_end End iterator to original point set.
	 * @param lvl Downsampling level, must be greater than 0.
	 * @param bouding_area Area of the region of the points that are to be downsampled.
	 * @param output Output array to receive downsampled points.
	 * @param previous_results Previous results data to use during downsampling.
	 */
	template<class Iterator, class OutputContainer>
	void downsample_points_(Iterator pt_begin, Iterator pt_end, std::ptrdiff_t lvl, const cuboid& bounding_cuboid, OutputContainer& output, uniform_downsampling_previous_results_t& previous_results) const;
	
	/**
	 * Get expected number of output points for downsampling level.
	 * The downsampling algorithm will be called with this value as expected number of output points.
	 * If exact downsampling is enabled, the output set will contain exactly this many points, if not it is an upper bound.
	 * @param input_number_of_points Number of input point set.
	 * @param i Downsampling level.
	 * @result Number of points that downsampling algorithm will output for given number of input points.
	 */
	std::size_t downsampling_expected_number_of_points_(std::size_t input_number_of_points, std::ptrdiff_t i) const {
		return downsampling_ratio(i) * input_number_of_points;
	}
	
	/**
	 * Generate downsampled point set for given level.
	 * @param pt_begin Start iterator to original point set.
	 * @param pt_end End iterator to original point set.
	 * @param lvl Downsampling level, must be greater than 0.
	 * @param bouding_area Area of the region of the points that are to be downsampled.
	 * @param output Output array to receive downsampled points.
	 */
	template<class Iterator, class OutputContainer>
	void downsample_points_(Iterator pt_begin, Iterator pt_end, std::ptrdiff_t lvl, const cuboid& bounding_cuboid, OutputContainer& output) const {
		uniform_downsampling_previous_results_t previous_results;
		downsample_points_(pt_begin, pt_end, lvl, bounding_cuboid, output, previous_results);
	}
	
public:
	downsampling_mode get_downsampling_mode() const { return downsampling_mode_; } ///< Get downsampling mode.
	float get_downsampling_amount() const { return downsampling_amount_; } ///< Get downsampling amount.
	std::size_t get_downsampling_levels() const { return downsampling_ratios_.size(); } ///< Get number of downsampling levels.

	/**
	 * Get downsampling ratio for given level.
	 * @param lvl Downsampling level.
	 */
	float downsampling_ratio(std::ptrdiff_t i) const {
		assert(i >= 0 && i < get_downsampling_levels());
		return downsampling_ratios_[i];
	}
	
	/**
	 * Get upper bound for size of downsampled point set.
	 * @param input_number_of_points Number of input point set.
	 * @param i Downsampling level.
	 */
	std::size_t downsampling_number_of_points_upper_bound(std::size_t input_number_of_points, std::ptrdiff_t i) const {
		return downsampling_expected_number_of_points_(input_number_of_points, i);
	}
	
	/**
	 * Get exact size of downsampled point set.
	 * Only possible when exact downsampling is enabled.
	 * @param input_number_of_points Number of input point set.
	 * @param i Downsampling level.
	 */
	std::size_t downsampling_number_of_points_exact(std::size_t input_number_of_points, std::ptrdiff_t i) const {
		if(exact_downsampling_) return downsampling_expected_number_of_points_(input_number_of_points, i);
		else throw std::logic_error("Exact downsampling not enabled for mipmap structure.");
	}
};


template<class Iterator, class OutputContainer>
void mipmap_structure::downsample_points_(Iterator pt_begin, Iterator pt_end, std::ptrdiff_t lvl, const cuboid& bounding_cuboid, OutputContainer& output, uniform_downsampling_previous_results_t& previous_results) const {
	std::size_t n = pt_end - pt_begin;
	if(n == 0) return;
	std::size_t expected = downsampling_expected_number_of_points_(n, lvl);
	if(expected >= n) {
		for(Iterator pt = pt_begin; pt != pt_end; ++pt) output.push_back(*pt);
	} else if(downsampling_mode_ == downsampling_mode::random) {
		random_downsampling(pt_begin, pt_end, expected, std::inserter(output, output.begin()));
	} else if(downsampling_mode_ == downsampling_mode::uniform) {
		uniform_downsampling(pt_begin, pt_end, expected, bounding_cuboid, std::inserter(output, output.begin()), previous_results);
		if(exact_downsampling_) fill_with_duplicates(output, expected);
	} else {
		throw std::logic_error("Invalid downsampling mode");
	}
	if(exact_downsampling_) assert(output.size() == expected);
	else assert(output.size() <= expected);
}

}

#endif
