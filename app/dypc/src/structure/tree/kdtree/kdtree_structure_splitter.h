#ifndef DYPC_KDTREE_STRUCTURE_SPLITTER_H_
#define DYPC_KDTREE_STRUCTURE_SPLITTER_H_

#include "../tree_structure_splitter.h"
#include <deque>
#include <algorithm>


namespace dypc {

class kdtree_structure_splitter : public tree_structure_splitter {
public:	
	static constexpr std::size_t number_of_node_children = 2;
	
	struct node_points_information {
		float split_plane;
	};
	
	static std::ptrdiff_t node_child_for_point(const point& pt, const cuboid& cub, const node_points_information& info, unsigned depth);
	static cuboid node_child_cuboid(const std::ptrdiff_t i, const cuboid& cub, const node_points_information& info, unsigned depth);
	
	template<class Iterator>
	static node_points_information compute_node_points_information(Iterator pt_begin, Iterator pt_end, const cuboid& cub, unsigned depth);
};


template<class Iterator>
kdtree_structure_splitter::node_points_information kdtree_structure_splitter::compute_node_points_information(Iterator pt_begin, Iterator pt_end, const cuboid& cub, unsigned depth){
	std::deque<float> coordinates;
	
	unsigned dimension = depth % 3;
	if(dimension == 0) for(Iterator it = pt_begin; it != pt_end; ++it) coordinates.push_back(it->x);
	else if(dimension == 1) for(Iterator it = pt_begin; it != pt_end; ++it) coordinates.push_back(it->y);
	else if(dimension == 2) for(Iterator it = pt_begin; it != pt_end; ++it) coordinates.push_back(it->z);
	
	std::sort(coordinates.begin(), coordinates.end());
	
	std::size_t n = coordinates.size();
	float split_plane;
	if(n % 2) split_plane = (coordinates[(n-1)/2] + coordinates[n/2]) / 2.0;
	else split_plane = coordinates[n/2];
	
	return { split_plane };
}



}

#endif
