#ifndef UTILITY_H
#define UTILITY_H

#include "data_struct.h"
#include "data_utility.h"

#include <algorithm> 
#include <cmath>
#include <cassert>

#define SLOPE_TYPE          double

// data structure for storing pairs of points
typedef struct point_pair
{
    point_t **pair;         // this is an array holding 2 points
    SLOPE_TYPE slope;       // this value holds the slope of the two points
}	point_pair_t;

// data structure for storing point set S hat, generalize for s>2
typedef struct s_point_set
{
	int numberOfPairs = 0;
	point_pair_t **point_pairs;
}	s_point_set_t;

using namespace std;

// compute min slope 
SLOPE_TYPE              compute_slope           (point_t* p1, point_t* p2);
bool                    compare_points_x        (const point_t* p1, const point_t* p2);
SLOPE_TYPE              min_slope               (point_set_t* P);
point_pair_t            display_points          (point_set_t* P, int s, double alpha, double beta, int num_iterations);


#endif


