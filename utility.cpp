using namespace std;


#include "utility.h"
#include <time.h> 

// compute slope between two points
SLOPE_TYPE compute_slope(point_t* p1, point_t* p2) {
    if (p2->coord[0] == p1->coord[0]) {
        return INF; 
    }
    return (p2->coord[1] - p1->coord[1]) / (p2->coord[0] - p1->coord[0]);
}

// helper comparison function for sort
bool compare_points_x(const point_t* p1, const point_t* p2) {
    return p1->coord[0] < p2->coord[0];
}

// compute min slope
// P: the input dataset
SLOPE_TYPE min_slope(point_set_t* P) {
    if (P->numberOfPoints < 2) {
        return INF;
    }

    // sort points based on x-coordinate
    sort(P->points, P->points + P->numberOfPoints, compare_points_x);

    // compute slopes of adjacent points
    SLOPE_TYPE min_slope = INF;
    for (int i = 0; i < P->numberOfPoints - 1; i++) {
        SLOPE_TYPE slope = compute_slope(P->points[i], P->points[i+1]);
        if (slope < min_slope) {
            min_slope = slope;
        }
    }

    return min_slope;
}

// randomly select set of points and compute their slopes
// check if their slopes are in the range of alpha and beta, add to s_hat if so
SLOPE_TYPE display_points(point_set_t* P, int s, double alpha, double beta, int num_iterations){
    point_t* point1;
    point_t* point2;
    point_pair_t* current_point_pair;                       // create point pair object
    s_point_set_t* SS;                                      // create S hat point pair set                      
    SLOPE_TYPE current_slope;
    SLOPE_TYPE median_slope;

    int rand_index1;
    int rand_index2;
    srand (time(NULL));                                     // generate random seed based on time

    // generate random pairs of points, find their slope, see if it's in range, add to SS
    for (int i = 0; i < num_iterations; i++){              // for each iteration, generate random pairs of points
        rand_index1 = rand() % P -> numberOfPoints;         // generate random index between 0 and numberOfPoints  
        rand_index2 = rand() % P -> numberOfPoints;    
        
        point1 = P -> points[rand_index1];                  // should be accessing point_t*
        point2 = P -> points[rand_index2];
        current_slope = compute_slope(point1, point2);          // find slope of pair

        current_point_pair -> pair[0] = point1;               // Save object's attributes
        current_point_pair -> pair[1] = point2;
        current_point_pair -> slope = current_slope;

        // if slope is within bounds, add point pair to SS and increment numberOfPairs
        if (current_slope >= alpha && current_slope <= beta){
            SS -> point_pairs[SS -> numberOfPairs] = current_point_pair;
            SS -> numberOfPairs += 1;
        }

    // find median slope (generalize for other S in the future)
    int median_index = (SS -> numberOfPairs) / 2;
    median_slope = SS -> point_pairs[median_index] -> slope;
    //RETURN PAIR THAT MAKES MEDIAN SLOPE INSEAD!!!!!
    return median_slope; // return median_slope or SS? add median to SS as an instance variable?
    }
}