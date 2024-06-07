#ifndef UTILITY_H
#define UTILITY_H

#include "data_struct.h"
#include "data_utility.h"

#include <cmath>

#define SLOPE_TYPE          double

using namespace std;

// compute min slope 
SLOPE_TYPE              compute_slope           (point_t* p1, point_t* p2);
bool                    compare_points_x        (const point_t* p1, const point_t* p2);
SLOPE_TYPE              min_slope               (point_set_t* P);
SLOPE_TYPE              display_points          (point_set_t* P, int s, double alpha, double beta, int num_iterations);


#endif
