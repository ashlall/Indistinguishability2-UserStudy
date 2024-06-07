using namespace std;

#include "utility.h"

// compute slope between two points
SLOPE_TYPE compute_slope(point_t* p1, point_t* p2) {
    if (p2->coord[0] == p1->coord[0]) {
        return INFINITY; 
    }
    return (p2->coord[1] - p1->coord[1]) / (p2->coord[0] - p1->coord[0]);
}

// compare function
bool compare_points_x(const point_t* p1, const point_t* p2) {
    return p1->coord[0] < p2->coord[0];
}

// compute min slope
// P: the input dataset
SLOPE_TYPE min_slope(point_set_t* P) {
    if (P->numberOfPoints < 2) {
        cerr << "Not enough points to compute a slope." << endl;
        return INFINITY;
    }

    // sort points based on x-coordinate
    sort(P->points, P->points + P->numberOfPoints, compare_points_x);

    // compute slopes of adjacent points
    SLOPE_TYPE min_slope = INFINITY;
    for (int i = 0; i < P->numberOfPoints - 1; i++) {
        SLOPE_TYPE slope = compute_slope(P->points[i], P->points[i+1]);
        if (slope < min_slope) {
            min_slope = slope;
        }
    }

    return min_slope;
}