#ifndef DYPC_MIPMAP_STRUCTURE_H_
#define DYPC_MIPMAP_STRUCTURE_H_

#include "structure.h"
#include "../model/model.h"
#include "../downsampling.h"
#include "../geometry/cuboid.h"
#include <stdexcept>


namespace dypc {

class mipmap_structure : public structure {
protected:
	const downsampling_mode downsampling_mode_;
	const downsampling_ratios_t downsampling_ratios_;

	mipmap_structure(std::size_t dlevels, std::size_t dmin, float damount, downsampling_mode dmode, model& mod) :
	structure(mod), downsampling_mode_(dmode), downsampling_ratios_(determine_downsampling_ratios(dlevels, mod.number_of_points(), dmin, damount)) { } 
	
	template<class Iterator, class OutputContainer>
	void downsample_points_(Iterator pt_begin, Iterator pt_end, std::ptrdiff_t lvl, float bounding_area, OutputContainer& output, uniform_downsampling_previous_results_t&) const;
	
	template<class Iterator, class OutputContainer>
	void downsample_points_(Iterator pt_begin, Iterator pt_end, std::ptrdiff_t lvl, float bounding_area, OutputContainer& output) const {
		uniform_downsampling_previous_results_t previous_results;
		downsample_points_(pt_begin, pt_end, lvl, bounding_area, output, previous_results);
	}
	
public:
	downsampling_mode get_downsampling_mode() const { return downsampling_mode_; }
	std::size_t get_downsampling_levels() const { return downsampling_ratios_.size(); }

	float downsampling_ratio(std::ptrdiff_t i) const {
		assert(i >= 0 && i < get_downsampling_levels());
		return downsampling_ratios_[i];
	}
	
	std::size_t downsampling_expected_number_of_points(std::size_t input_number_of_points, std::ptrdiff_t i) const {
		return downsampling_ratio(i) * input_number_of_points;
	}
};


template<class Iterator, class OutputContainer>
void mipmap_structure::downsample_points_(Iterator pt_begin, Iterator pt_end, std::ptrdiff_t lvl, float bounding_area, OutputContainer& output, uniform_downsampling_previous_results_t& previous_results) const {
	std::size_t n = pt_end - pt_begin;
	if(n == 0) return;
	std::size_t expected = downsampling_expected_number_of_points(n, lvl);
	if(expected >= n) {
		for(Iterator pt = pt_begin; pt != pt_end; ++pt) output.push_back(*pt);
	} else if(downsampling_mode_ == downsampling_mode::random) {
		random_downsampling(pt_begin, pt_end, expected, output);
	} else if(downsampling_mode_ == downsampling_mode::uniform) {
		uniform_downsampling(pt_begin, pt_end, expected, bounding_area, output, previous_results);
	} else {
		throw std::logic_error("Invalid downsampling mode");
	}
}

}

#endif
