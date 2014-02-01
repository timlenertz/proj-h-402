#ifndef DYPC_DOWNSAMPLING_H_
#define DYPC_DOWNSAMPLING_H_

#include <vector>
#include "point.h"

namespace dypc {

enum downsampling_mode_t { random_downsampling_mode = 0, uniform_downsampling_mode };

void random_downsampling(const point* pt_begin, const point* pt_end, float ratio, std::vector<point>& output);
void uniform_downsampling(const point* pt_begin, const point* pt_end, float ratio, float bounding_area, std::vector<point>& output);

float uniform_downsampling_side_length(const std::vector<point>& points, float ratio, float bounding_area);

}

#endif
