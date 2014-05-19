#ifndef DYPC_DOWNSAMPLING_H_
#define DYPC_DOWNSAMPLING_H_

#include "enums.h"
#include "point.h"
#include "progress.h"
#include "util.h"
#include "geometry/cuboid.h"
#include <vector>
#include <map>
#include <tuple>
#include <glm/glm.hpp>
#include <cmath>
#include <cassert>
#include <random>
#include <cstdint>
#include <stdexcept>
#include <iterator>

namespace dypc {

/**
 * Structure which stores uniform downsampling results.
 * If uniform downsampling is applied several times (i.e. different levels) on the same point set, this structure stores cube side length - ratio information that can be reused to accelerate uniform_downsampling_side_length.
 */
using uniform_downsampling_previous_results_t = std::map<std::size_t, float>;


/**
 * Structure which stores downsampling ratios for level.
 */
using downsampling_ratios_t = std::vector<float>;


/**
 * Choose a continuous downsampling level based on distance.
 * See report for details.
 * @param levels Number of downsampling levels.
 * @param distance Distance of region being downsampled.
 * @param setting Downsampling setting.
 * @return Downsampling level.
 */
float choose_downsampling_level_continuous(std::size_t levels, float distance, float setting);

/**
 * Choose a discrete downsampling level based on distance.
 * See report for details.
 * @param levels Number of downsampling levels.
 * @param distance Distance of region being downsampled.
 * @param setting Downsampling setting.
 * @return Downsampling level.
 */
inline std::ptrdiff_t choose_downsampling_level(std::size_t levels, float distance, float setting) {
	return std::ceil(choose_downsampling_level_continuous(levels, distance, setting));
}

/**
 * Get downsampling ratio for given level.
 * See report for details.
 * @param l Downsampling level. 0 means no downsampling. Must be lesser than \a levels.
 * @param levels Number of levels.
 * @param total_points Total points in region.
 * @param minimum Minimal number of points to output.
 * @param amount Downsampling amount.
 * @return Downsampling ratio.
 */
float downsampling_ratio_for_level(float l, std::size_t levels, std::size_t total_points, std::size_t minimum, float amount);

/**
 * Get continous downsampling ratio for given ratio.
 * See report for details.
 * @param levels Number of downsampling levels.
 * @param distance Distance of region being downsampled.
 * @param setting Downsampling setting.
 * @param total_points Total points in region.
 * @param minimum Minimal number of points to output.
 * @param amount Downsampling amount.
 * @return Downsampling ratio.
 */
inline float choose_downsampling_level_ratio_continuous(std::size_t levels, float distance, float setting, std::size_t total_points, std::size_t minimum, float amount) {
	float level = choose_downsampling_level_continuous(levels, distance, setting);
	return downsampling_ratio_for_level(level, levels, total_points, minimum, amount);
}


/**
 * Get downsampling ratios for all levels.
 * @see downsampling_ratio_for_level
 * @param levels Number of levels.
 * @param total_points Total points in region.
 * @param minimum Minimal number of points to output.
 * @param amount Downsampling amount.
 * @return Structure containing levels and corresponding ratios.
 */
downsampling_ratios_t determine_downsampling_ratios(std::size_t levels, std::size_t total_points, std::size_t minimum, float amount);


/**
 * Apply random downsampling.
 * Randomly chooses exactly \a expected_number_of_points points from the input point set.
 * It iterates through the point set only once.
 * @tparam Iterator Iterator to point set.
 * @tparam OutputContainer Container type for output.
 * @param pt_begin Begin iterator of points.
 * @param pt_end End iterator of points.
 * @param expected_number_of_points Number of points to output.
 * @param output Container to receive output set.
 */
template<class Iterator, class Inserter>
void random_downsampling(Iterator pt_begin, Iterator pt_end, std::size_t expected_number_of_points, Inserter output) {
	random_generator_t random_generator;
	const std::size_t total_number_of_points = pt_end - pt_begin;
	std::size_t picked = 0, left = total_number_of_points, needed = expected_number_of_points;
	for(Iterator pt = pt_begin; (pt != pt_end) && (needed > 0); ++pt) {
		std::uniform_int_distribution<std::size_t> distribution(0, left - 1);
		if(distribution(random_generator) < needed) {
			*output = *pt;
			--needed;
		}
		--left;
	}	
}

/**
 * Generate statistical information for uniform downsampling side lengths.
 * 
 */
template<class Iterator>
void uniform_downsampling_side_length_statistics(std::ostream& output, Iterator pt_begin, Iterator pt_end, float max_side, float step) {	
	for(float side = max_side; side >= 0.0; side -= step) {
		using cube_index_t = std::tuple<long, long, long>;
		std::map<cube_index_t, bool> cubes;
		for(Iterator pt = pt_begin; pt != pt_end; ++pt) {				
			cube_index_t idx(pt->x / side, pt->y / side, pt->z / side);
			cubes[idx] = true;
		}
		output << side << " " << cubes.size() << std::endl;
	}
}


template<class Iterator>
std::size_t uniform_downsampling_number_of_points_for_side_length(Iterator pt_begin, Iterator pt_end, float side_length, std::size_t maximal_increment) {
	assert(maximal_increment >= 1);
	
	random_generator_t random_generator;
	std::uniform_int_distribution<std::size_t> increment_dist(1, maximal_increment);

	std::size_t attempt_number_of_points = 0;
	using cube_index_t = std::tuple<long, long, long>;
	std::map<cube_index_t, bool> cubes;
	for(Iterator pt = pt_begin; pt < pt_end;) {
		std::size_t increment = increment_dist(random_generator);
		cube_index_t idx(pt->x / side_length, pt->y / side_length, pt->z / side_length);
		bool& cube_has_point = cubes[idx];
		if(! cube_has_point) {
			attempt_number_of_points += increment;
			cube_has_point = true;
		}
		pt += increment;
	}
	
	return attempt_number_of_points;
}


/**
 * Estimate cubes side length for uniform downsampling.
 */
template<class Iterator>
float uniform_downsampling_side_length(Iterator pt_begin, Iterator pt_end, std::size_t expected_number_of_points, const cuboid& bounding_cuboid, uniform_downsampling_previous_results_t& previous_results) {
	std::size_t total_number_of_points = pt_end - pt_begin;
	if(total_number_of_points <= 1) return bounding_cuboid.maximal_side_length();
	
	assert(expected_number_of_points < total_number_of_points);
	
	std::size_t tolerance = 0.1 * expected_number_of_points; if(tolerance == 0) tolerance = 1;
	std::size_t maximal_increment = expected_number_of_points / 3000000; if(maximal_increment == 0) maximal_increment = 1;
	
	struct sample {
		sample(float l, std::size_t n) : side_length(l), number_of_points(n) { }
		float side_length;
		std::size_t number_of_points;
	};
	sample lower_bound(0.0, total_number_of_points);
	sample upper_bound(bounding_cuboid.maximal_side_length(), 1);
	
	for(const auto& prev : previous_results) {
		if(prev.first <= expected_number_of_points && prev.second < upper_bound.side_length) upper_bound = sample(prev.second, prev.first);
		else if(prev.first >= expected_number_of_points && prev.second > lower_bound.side_length) lower_bound = sample(prev.second, prev.first);
	}
	
	float last_side_length = lower_bound.side_length + 0.01*upper_bound.side_length;
	sample attempt(last_side_length, 0);
	
	progress("Finding uniform downsampling cube size...", [&](progress_handle& pr) {
		do {			
			attempt.number_of_points = uniform_downsampling_number_of_points_for_side_length(pt_begin, pt_end, attempt.side_length, maximal_increment);

			pr.pulse();
			pr.message("expected: " + std::to_string(expected_number_of_points) + "; got: " + std::to_string(attempt.number_of_points) + "; side: " + std::to_string(attempt.side_length));
				
			if(attempt.number_of_points > expected_number_of_points) lower_bound = attempt;
			else upper_bound = attempt;
			
			previous_results[attempt.number_of_points] = attempt.side_length;
			
			last_side_length = attempt.side_length;
			attempt.side_length = lower_bound.side_length + (upper_bound.side_length - lower_bound.side_length)/2;
			if(attempt.side_length < float_comparison_epsilon) break; // don't let it become zero
			if(approximately_equal(attempt.side_length, last_side_length)) break; // happens in non-monotonic segment
		} while(attempt.number_of_points > expected_number_of_points || expected_number_of_points - attempt.number_of_points > tolerance);
	
	});
	
	return last_side_length;
}
	

template<class Iterator, class Inserter>
inline void uniform_downsampling(Iterator pt_begin, Iterator pt_end, std::size_t expected_number_of_points, const cuboid& bounding_cuboid, Inserter output) {
	uniform_downsampling_previous_results_t previous_results;
	uniform_downsampling(pt_begin, pt_end, expected_number_of_points, bounding_cuboid, output, previous_results);
}


/**
 * 
 */
template<class Iterator, class Inserter>
void uniform_downsampling(Iterator pt_begin, Iterator pt_end, std::size_t expected_number_of_points, const cuboid& bounding_cuboid, Inserter output, uniform_downsampling_previous_results_t& previous_results) {
	std::size_t total_number_of_points = pt_end - pt_begin;
	if(expected_number_of_points == 0) return;
	else if(expected_number_of_points >= total_number_of_points) {
		for(Iterator pt = pt_begin; pt != pt_end; ++pt) *output = *pt;
	}
	
	float side = uniform_downsampling_side_length(pt_begin, pt_end, expected_number_of_points, bounding_cuboid, previous_results);
	
	using cube_index_t = std::tuple<long, long, long>;
	
	class cube {
	private:	
		const glm::vec3 center_coordinates_;
		std::size_t number_of_points_ = 0;
		float minimal_sqdistance_to_center_;
		glm::vec3 mean_coordinates_ = glm::vec3(0, 0, 0);
		std::uint8_t r_, g_, b_;

	public:
		explicit cube(cube_index_t idx, float l) :
			center_coordinates_( std::get<0>(idx) * l, std::get<1>(idx) * l, std::get<2>(idx) * l ),
			minimal_sqdistance_to_center_(INFINITY) { }

		void add_point(const point& pt) {
			float dx = pt.x - center_coordinates_.x, dy = pt.y - center_coordinates_.y, dz = pt.z - center_coordinates_.z;
			float sqdistance = dx*dx + dy*dy + dz*dz;
			if(sqdistance < minimal_sqdistance_to_center_) {
				minimal_sqdistance_to_center_ = sqdistance;
				r_ = pt.r; g_ = pt.g; b_ = pt.b;
			}
			
			glm::vec3 ptv = pt;
			auto pos = ptv - center_coordinates_;
			if(number_of_points_ == 0) {
				mean_coordinates_ = pos;
			} else {
				float n1 = (float)(number_of_points_ + 1);
				mean_coordinates_ *= (float)number_of_points_ / n1;
				mean_coordinates_ += pos / n1;
			}
			
			++number_of_points_;
		}
		
		point make_point() const {
			assert(number_of_points_ >= 0);
			return point(center_coordinates_ - mean_coordinates_, r_, g_, b_);
		}
	};
	
	using cubes_t = std::map<cube_index_t, cube>;
	cubes_t cubes;
	
	progress_foreach(pt_begin, pt_end, "Computing uniform downsampling cubes..."+std::to_string(side), [&](const point& pt) {
		cube_index_t idx(pt.x / side, pt.y / side, pt.z / side);
		auto it = cubes.find(idx);
		if(it == cubes.end()) {
			auto r = cubes.emplace(std::piecewise_construct, idx, std::forward_as_tuple(idx, side));
			if(! r.second) throw std::runtime_error("Failed to insert cube");
			it = r.first;
		}
		it->second.add_point(pt);
	});
	
	assert(cubes.size() <= expected_number_of_points);
	
	progress_foreach(cubes, "Computing uniform downsampling points..."+std::to_string(cubes.size()), [&](const typename cubes_t::value_type& p) {
		const auto& cube = p.second;
		*output = cube.make_point();
	});
}

}

#endif
