#ifndef DYPC_DOWNSAMPLING_H_
#define DYPC_DOWNSAMPLING_H_

#include "enums.h"
#include "point.h"
#include "progress.h"
#include <vector>
#include <vector>
#include <map>
#include <tuple>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <cassert>
#include <random>

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
 * Get downsampling ration for given level.
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
 */
inline float choose_downsampling_level_ratio_continuous(std::size_t levels, float distance, float setting, std::size_t total_points, std::size_t minimum, float amount) {
	float level = choose_downsampling_level_continuous(levels, distance, setting);
	return downsampling_ratio_for_level(level, levels, total_points, minimum, amount);
}


downsampling_ratios_t determine_downsampling_ratios(std::size_t levels, std::size_t total_points, std::size_t minimum, float amount);


/**
 * Apply random downsampling.
 *
 * @tparam Iterator Iterator to point set.
 * @tparam OutputContainer Container type for output.
 * @param pt_begin Begin iterator of points.
 * @param pt_end End iterator of points.
 * @param expected_number_of_points Number of points to output.
 * @param output Container to receive output set.
 */
template<class Iterator, class OutputContainer>
void random_downsampling(Iterator pt_begin, Iterator pt_end, std::size_t expected_number_of_points, OutputContainer& output) {
	std::size_t total_number_of_points = pt_end - pt_begin;
	std::mt19937 random_generator;
	std::discrete_distribution<bool> distribution({ total_number_of_points - expected_number_of_points, expected_number_of_points });
	
	for(Iterator pt = pt_begin; (pt != pt_end) && (output.size() < expected_number_of_points); ++pt)
		if(distribution(random_generator)) output.push_back(*pt);
	
	assert(output.size() <= expected_number_of_points);
}


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
float uniform_downsampling_side_length(Iterator pt_begin, Iterator pt_end, std::size_t expected_number_of_points, float bounding_area, uniform_downsampling_previous_results_t& previous_results) {	
	std::size_t number_of_points = pt_end - pt_begin;
	if(number_of_points <= 1) return std::cbrt(bounding_area);
	
	std::size_t threshold = 0.1 * expected_number_of_points;
	std::size_t increment_max = number_of_points / 3000000;
	
	while(increment_max >= expected_number_of_points) increment_max /= 2;
	if(! increment_max) increment_max = 1;
	
	float minimal_side_length = 0;
	float maximal_side_length = std::cbrt(bounding_area);
			
	std::size_t minimal_number_of_points = 0, maximal_number_of_points = number_of_points;
	for(const auto& prev : previous_results) {
		if(prev.first <= expected_number_of_points && prev.second < maximal_side_length) {
			maximal_side_length = prev.second; 
			minimal_number_of_points = prev.first;
		} else if(prev.first >= expected_number_of_points && prev.second > minimal_side_length) {
			minimal_side_length = prev.second;
			maximal_number_of_points = prev.first;
		}
	}

	float attempt_side_length = minimal_side_length + 0.01*maximal_side_length;
	if(maximal_number_of_points - minimal_number_of_points <= threshold) return attempt_side_length;
	
	
	progress("Finding uniform downsampling cube size", [&](progress_handle& pr) {
		std::size_t attempt_number_of_points;
		do {			
			attempt_number_of_points = 0;
					
			using cube_index_t = std::tuple<long, long, long>;
			std::map<cube_index_t, bool> cubes;
			for(Iterator pt = pt_begin; pt < pt_end;) {
				std::size_t increment = 1 + (std::rand() % increment_max);
				
				cube_index_t idx(pt->x / attempt_side_length, pt->y / attempt_side_length, pt->z / attempt_side_length);
				bool& cube_has_point = cubes[idx];
				if(! cube_has_point) {
					attempt_number_of_points += increment;
					cube_has_point = true;
				}
				
				pt += increment;
			}
			
			previous_results[attempt_number_of_points] = attempt_side_length;
			
			if(attempt_number_of_points > expected_number_of_points) minimal_side_length = attempt_side_length;
			else maximal_side_length = attempt_side_length;		
			attempt_side_length = minimal_side_length + (maximal_side_length - minimal_side_length)/2;
			
			pr.pulse();
			pr.message("expected " + std::to_string(expected_number_of_points) + "; got: " + std::to_string(attempt_number_of_points) + "; side: " + std::to_string(attempt_side_length));
		} while(std::abs(attempt_number_of_points - expected_number_of_points) > threshold);
	});
	
	return attempt_side_length;
}


template<class Iterator, class OutputContainer>
inline void uniform_downsampling(Iterator pt_begin, Iterator pt_end, std::size_t expected_number_of_points, float bounding_area, OutputContainer& output) {
	uniform_downsampling_previous_results_t previous_results;
	uniform_downsampling(pt_begin, pt_end, expected_number_of_points, bounding_area, output);
}


/**
 * 
 */
template<class Iterator, class OutputContainer>
void uniform_downsampling(Iterator pt_begin, Iterator pt_end, std::size_t expected_number_of_points, float bounding_area, OutputContainer& output, uniform_downsampling_previous_results_t& previous_results) {
	std::size_t number_of_points = pt_end - pt_begin;
	
	float side = uniform_downsampling_side_length(pt_begin, pt_end, expected_number_of_points, bounding_area, previous_results);
	
	using cube_index_t = std::tuple<long, long, long>;
	using cubes_t = std::map<cube_index_t, std::vector<point>>;
	cubes_t cubes;
	
	progress_foreach(pt_begin, pt_end, "Computing uniform downsampling cubes", [&](const point& pt) {
		cube_index_t idx(pt.x / side, pt.y / side, pt.z / side);
		cubes[idx].push_back(pt);
	});
	
	
	progress_foreach(cubes, "Computing uniform downsampling points", [&](const cubes_t::value_type& p) {
		const auto& cube = p.second;
		
		glm::vec3 center(0, 0, 0);
		for(const point& pt : cube) center += glm::vec3(pt.x, pt.y, pt.z);
		center /= cube.size(); 
		
		float min_distance = INFINITY;
		const point* median_point = nullptr;
		for(const point& pt : cube) {
			float distance = glm::distance(center, glm::vec3(pt.x, pt.y, pt.z));
			if(distance < min_distance) {
				min_distance = distance;
				median_point = &pt;
			}
		}
		
		output.emplace_back(
			center.x, center.y, center.z,
			median_point->r, median_point->g, median_point->b
		);
		
		if(output.size() == expected_number_of_points) return;
	});
	
	assert(output.size() <= expected_number_of_points);
}

}

#endif
