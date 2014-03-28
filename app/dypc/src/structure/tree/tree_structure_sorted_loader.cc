#include "tree_structure_sorted_loader.h"
#include <algorithm>
#include <fstream>

namespace dypc {

std::size_t tree_structure_sorted_loader::extract_node_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req, const tree_structure_source::node& nd, const source_node* skip) const {
	std::size_t n = tree_structure_ordered_loader::extract_node_points_(points, capacity, req, nd, skip);
	/*std::sort(points, points + n, [&req](const point& a, const point& b) {
		glm::vec3 pa(a.x, a.y, a.z), pb(b.x, b.y, b.z);
		return (glm::distance(req.position, pa) < glm::distance(req.position, pb));
	});*/
	return n;
}

	

std::size_t tree_structure_sorted_loader::extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
	std::size_t n = tree_structure_ordered_loader::extract_points_(points, capacity, req);
		
	/*
	std::sort(points, points + n, [&req](const point& a, const point& b) {
		glm::vec3 pa(a.x, a.y, a.z), pb(b.x, b.y, b.z);
		return (glm::distance(req.position, pa) < glm::distance(req.position, pb));
	});*/
	return n;
}

}
