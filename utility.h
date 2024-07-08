#ifndef UTILITY_H
#define UTILITY_H

#include "data_struct.h"
#include "data_utility.h"

#include <algorithm> 
#include <cmath>
#include <cassert>
#include <vector>
#include <time.h> 
#include <iostream>

#include "operation.h"
#include "rtree.h"
#include "lp.h"
#include "pruning.h"
#include "utility.h"
#include <queue>

#define SLOPE_TYPE          double

// Helper data structure for count_slopes
typedef struct point_order
{
    point_t *point;
    int order;
}   point_order_t;

using namespace std;

COORD_TYPE              slope_to_ratio          (SLOPE_TYPE s);
COORD_TYPE              ratio_to_slope          (COORD_TYPE r);
SLOPE_TYPE              compute_slope           (point_t* p1, point_t* p2, int dim_a, int dim_i);
SLOPE_TYPE              min_slope               (point_set_t* P, int dim_a, int dim_i);
int                     count_inversions        (vector<int>& arr);
int                     count_slopes            (point_set_t* P, double alpha, double beta, bool adjust, int dim_a, int dim_i);

// Strongly truthful algorithms
point_t**               breakpoint_one_round    (point_set_t* P, int s, double alpha, double beta, int dim_a, int dim_i);
double                  max_utility_breakpoint  (point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize);

#endif


