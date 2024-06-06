#ifndef UTILITY_H
#define UTILITY_H

#include "data_struct.h"
#include "data_utility.h"

#define SLOPE_TYPE          double

using namespace std;

// compute min slope 
SLOPE_TYPE min_slope(point_set_t* P);

#endif
