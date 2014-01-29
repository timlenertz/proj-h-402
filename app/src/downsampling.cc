#include "downsampling.h"
#include <map>
#include <tuple>
#include <glm/glm.hpp>
#include <cmath>


namespace dypc {

void random_downsampling(const std::vector<point>& points, float probability, std::vector<point>& output) {
	probability *= RAND_MAX;
	for(const point& pt : points) if(std::rand() < probability) output.push_back(pt);
}


void uniform_downsampling(const std::vector<point>& points, float probability, float bounding_area, std::vector<point>& output) {
	float expected_number = probability * points.size();
	float cube_area = bounding_area  / expected_number;
	float side = std::pow(cube_area, 1.0/2.3);
	
	using cube_index_t = std::tuple<long, long, long>;
	std::map<cube_index_t, std::vector<point>> cubes;
	for(const point& pt : points) {
		cube_index_t idx(pt.x / side, pt.y / side, pt.z / side);
		cubes[idx].push_back(pt);
	}
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
	}
}

}
