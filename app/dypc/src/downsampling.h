#ifndef DYPC_DOWNSAMPLING_H_
#define DYPC_DOWNSAMPLING_H_

#include "point.h"
#include "progress.h"
#include <vector>
#include <map>
#include <tuple>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

namespace dypc {

enum downsampling_mode_t { random_downsampling_mode = 0, uniform_downsampling_mode };

using uniform_downsampling_previous_results_t = std::map<std::size_t, float>;

template<class Iterator, class OutputContainer>
void random_downsampling(Iterator pt_begin, Iterator pt_end, float ratio, OutputContainer& output) {
	ratio *= RAND_MAX;
	for(Iterator pt = pt_begin; pt != pt_end; ++pt) if(std::rand() < ratio) output.push_back(*pt);
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
float uniform_downsampling_side_length(Iterator pt_begin, Iterator pt_end, float ratio, float bounding_area, uniform_downsampling_previous_results_t& previous_results) {	
	std::size_t number_of_points = pt_end - pt_begin;
	if(number_of_points <= 1) return std::cbrt(bounding_area);
	
	std::size_t expected_number_of_points = ratio * number_of_points;
	std::size_t threshold = 0.1 * expected_number_of_points;
	std::size_t increment_max = number_of_points / 3000000;
	
	while(increment_max >= expected_number_of_points) increment_max /= 2;
	if(! increment_max) increment_max = 1;
	
	float minimal_side_length = 0;
	float maximal_side_length = std::cbrt(bounding_area);
			
	std::size_t minimal_number_of_points = 0, maximal_number_of_points = -1;
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
	
	
	progress("Finding uniform downsampling cube size", 0, 0, [&]() {
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
			
			increment_progress();
			
			std::cout << "expected: " << expected_number_of_points << ", got: " << attempt_number_of_points << "; side: " << attempt_side_length << std::endl;
		} while(std::abs(attempt_number_of_points - expected_number_of_points) > threshold);
	});
	
	return attempt_side_length;
}


template<class Iterator, class OutputContainer>
inline void uniform_downsampling(Iterator pt_begin, Iterator pt_end, float ratio, float bounding_area, OutputContainer& output) {
	uniform_downsampling_previous_results_t previous_results;
	uniform_downsampling(pt_begin, pt_end, ratio, bounding_area, output);
}


template<class Iterator, class OutputContainer>
void uniform_downsampling(Iterator pt_begin, Iterator pt_end, float ratio, float bounding_area, OutputContainer& output, uniform_downsampling_previous_results_t& previous_results) {
	std::size_t number_of_points = pt_end - pt_begin;
	
	float side = uniform_downsampling_side_length(pt_begin, pt_end, ratio, bounding_area, previous_results);
	
	using cube_index_t = std::tuple<long, long, long>;
	std::map<cube_index_t, std::vector<point>> cubes;
	
	progress("Computing uniform downsampling cubes", number_of_points, [&]() {
		for(Iterator pt = pt_begin; pt != pt_end; ++pt) {
			cube_index_t idx(pt->x / side, pt->y / side, pt->z / side);
			cubes[idx].push_back(*pt);
			increment_progress();
		}
	});
	
	progress("Computing uniform downsampling points", cubes.size(), [&]() {
		for(const auto& p : cubes) {
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
			
			increment_progress();
		}
	});
}

}

#endif
