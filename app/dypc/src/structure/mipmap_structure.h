#ifndef DYPC_MIPMAP_STRUCTURE_H_
#define DYPC_MIPMAP_STRUCTURE_H_

#include "structure.h"
#include "../model/model.h"
#include "../downsampling.h"
#include "../cuboid.h"
#include <stdexcept>


namespace dypc {

template<std::size_t Levels>
class mipmap_structure : public structure {
protected:
	downsampling_levels_t<Levels> downsampling_levels_;
	const downsampling_mode downsampling_mode_;
	const std::size_t downsampling_minimum_;
	const float downsampling_amount_;

	mipmap_structure(std::size_t dmin, float damount, downsampling_mode dmode) :
	downsampling_mode_(dmode), downsampling_minimum_(dmin), downsampling_amount_(damount) { }
	
	template<class Iterator, class OutputContainer>
	void downsample_points_(Iterator pt_begin, Iterator pt_end, std::ptrdiff_t lvl, const cuboid& bounding_cuboid, OutputContainer& output, uniform_downsampling_previous_results_t&) const;
	
	template<class Iterator, class OutputContainer>
	void downsample_points_(Iterator pt_begin, Iterator pt_end, std::ptrdiff_t lvl, const cuboid& bounding_cuboid, OutputContainer& output) const {
		uniform_downsampling_previous_results_t previous_results;
		downsample_points_(pt_begin, pt_end, lvl, bounding_cuboid, output, previous_results);
	}

	void load_model_(model& mod) {
		downsampling_levels_ = determine_downsampling_levels<Levels>(mod.number_of_points(), downsampling_minimum_, downsampling_amount_);
	}

public:
	float downsampling_ratio(std::ptrdiff_t i) const {
		assert(i >= 0 && i < Levels);
		return downsampling_levels_[i];
	}
	
	std::size_t downsampling_expected_number_of_points(std::size_t input_number_of_points, std::ptrdiff_t i) const {
		return downsampling_ratio(i) * input_number_of_points;
	}
};


template<std::size_t Levels> template<class Iterator, class OutputContainer>
void mipmap_structure<Levels>::downsample_points_(Iterator pt_begin, Iterator pt_end, std::ptrdiff_t lvl, const cuboid& bounding_cuboid, OutputContainer& output, uniform_downsampling_previous_results_t& previous_results) const {
	std::size_t n = pt_end - pt_begin;
	if(n == 0) return;
	std::size_t expected = downsampling_expected_number_of_points(n, lvl);
	if(expected >= n) {
		for(Iterator pt = pt_begin; pt != pt_end; ++pt) output.push_back(*pt);
	} else if(downsampling_mode_ == downsampling_mode::random) {
		random_downsampling(pt_begin, pt_end, expected, output);
	} else if(downsampling_mode_ == downsampling_mode::uniform) {
		uniform_downsampling(pt_begin, pt_end, expected, bounding_cuboid.area(), output, previous_results);
	} else {
		throw std::logic_error("Invalid downsampling mode");
	}
}

}

#endif
