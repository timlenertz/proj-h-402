#ifndef DYPC_DOWNSAMPLING_H_
#define DYPC_DOWNSAMPLING_H_

#include <vector>
#include "point.h"

namespace dypc {

enum downsampling_mode_t { random_downsampling_mode = 0, uniform_downsampling_mode };

void random_downsampling(const std::vector<point>& points, float probability, std::vector<point>& output);
void uniform_downsampling(const std::vector<point>& points, float probability, float bounding_area, std::vector<point>& output);

}

#endif
