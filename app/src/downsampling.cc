#include "downsampling.h"
#include "progress.h"
#include <map>
#include <tuple>
#include <glm/glm.hpp>
#include <cmath>


namespace dypc {

void random_downsampling(const point* pt_begin, const point* pt_end, float ratio, std::vector<point>& output) {
	ratio *= RAND_MAX;
	for(const point* pt = pt_begin; pt != pt_end; ++pt) if(std::rand() < ratio) output.push_back(*pt);
}


float uniform_downsampling_side_length(const point* pt_begin, const point* pt_end, float ratio, float bounding_area) {	
	std::size_t number_of_points = pt_end - pt_begin;
	if(number_of_points <= 1) return std::cbrt(bounding_area);
	
	std::size_t expected_number_of_points = ratio * number_of_points;
	std::size_t threshold = 0.03 * number_of_points;
	std::size_t increment = number_of_points / 100000;
	
	while(increment >= expected_number_of_points) increment /= 2;
	if(increment == 0) increment = 1;
	
	float minimal_side_length = 0;
	float maximal_side_length = std::cbrt(bounding_area);
		
	float attempt_side_length = 0.01 * maximal_side_length;
	
	progress("Finding uniform downsampling cube size", 0, 0, [&]() {
		std::size_t attempt_number_of_points;
		do {			
			attempt_number_of_points = 0;
					
			using cube_index_t = std::tuple<long, long, long>;
			std::map<cube_index_t, bool> cubes;
			for(const point* pt = pt_begin; pt < pt_end; pt += increment) {
				cube_index_t idx(pt->x / attempt_side_length, pt->y / attempt_side_length, pt->z / attempt_side_length);
				bool& cube_has_point = cubes[idx];
				if(! cube_has_point) {
					attempt_number_of_points += increment;
					cube_has_point = true;
				}
			}
			
			if(attempt_number_of_points > expected_number_of_points) minimal_side_length = attempt_side_length;
			else maximal_side_length = attempt_side_length;		
			attempt_side_length = minimal_side_length + (maximal_side_length - minimal_side_length)/2;
			
			increment_progress();
		} while(std::abs(attempt_number_of_points - expected_number_of_points) > threshold);
	});
	
	return attempt_side_length;
}


void uniform_downsampling(const point* pt_begin, const point* pt_end, float ratio, float bounding_area, std::vector<point>& output) {
	std::size_t number_of_points = pt_end - pt_begin;
	
	float side = uniform_downsampling_side_length(pt_begin, pt_end, ratio, bounding_area);	
	
	using cube_index_t = std::tuple<long, long, long>;
	std::map<cube_index_t, std::vector<point>> cubes;
	
	progress("Computing uniform downsampling cubes", number_of_points, number_of_points/1000, [&]() {
		for(const point* pt = pt_begin; pt != pt_end; ++pt) {
			cube_index_t idx(pt->x / side, pt->y / side, pt->z / side);
			cubes[idx].push_back(*pt);
			increment_progress();
		}
	});
	
	progress("Computing uniform downsampling points", cubes.size(), cubes.size()/1000, [&]() {
		for(const auto& p : cubes) {
			const auto& cube = p.second;
			
			glm::vec3 center(0, 0, 0);
			for(const point& pt : cube) center += glm::vec3(pt.x, pt.y, pt.z);
			center /= cube.size(); 
			
			float min_distance = side * side;
			const point* median_point = nullptr;
			for(const point& pt : cube) {
				float distance = glm::distance(center, glm::vec3(pt.x, pt.y, pt.z));
				if(distance < min_distance) {
					min_distance = distance;
					median_point = &pt;
				}
			}
			
			if(median_point) output.emplace_back(
				center.x, center.y, center.z,
				median_point->r, median_point->g, median_point->b
			);
			
			increment_progress();
		}
	});
}

}
