using namespace std;


#include "utility.h"
#include <time.h> 
#include <iostream>


//==============================================================================================
// compute_slope
// 
// 
// compute slope between two points
//==============================================================================================
SLOPE_TYPE compute_slope(point_t* p1, point_t* p2) {
    if (p2->coord[0] == p1->coord[0]) {
        return INF; 
    }
    return (p2->coord[1] - p1->coord[1]) / (p2->coord[0] - p1->coord[0]);
}


//==============================================================================================
// compute_slope
// 
// 
// helper comparison function for sort
//==============================================================================================
bool compare_points_x(const point_t* p1, const point_t* p2) {
    return p1->coord[0] < p2->coord[0];
}


//==============================================================================================
// min_slope
// 
// compute min slope
// P: the input dataset
//==============================================================================================
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

//==============================================================================================
// count_slopes
// 
// 
// 
//==============================================================================================
int count_slopes(point_set_t* P, double alpha, double beta) {
    int num_slopes;
    SLOPE_TYPE y_at_alpha;

    // compute y values at x = alpha for all point-line
    for (int i = 0; i < P->numberOfPoints; i++) {
        point_t *currentPoint = P->points[i];
        y_at_alpha = currentPoint -> coord[0] * alpha - currentPoint->coord[1];
    }
    return num_slopes;
}

//==============================================================================================
// display_points_v2
// 
// randomly select set of points and compute their slopes
// check if their slopes are in the range of alpha and beta, add to points_to_display if so
//==============================================================================================
point_set_t display_points_v2(point_set_t* P, int s, double alpha, double beta, int num_iterations){
    
    // initialiaze required variables
    point_t* point1;
    point_t* point2;
    point_set_t* points_to_display;                                               
    SLOPE_TYPE current_slope;
    double min_difference = INF;
    int rand_index1;
    int rand_index2;
    double current_slope_count;

    int slope_count = count_slopes(P, alpha, beta);
    int mid = slope_count / 2;


    // generate random seed based on time
    srand (time(NULL));

    for (int i = 0; i < num_iterations; i++){
        
        // generate random pairs of points
        rand_index1 = rand() % P -> numberOfPoints;
        rand_index2 = rand() % P -> numberOfPoints;    

        point1 = P -> points[rand_index1];                  
        point2 = P -> points[rand_index2];

        // compute slope
        current_slope = compute_slope(point1, point2);          

        // check if slope within range
        if (current_slope >= alpha && current_slope <= beta){
            
            current_slope_count = count_slopes(P, alpha, current_slope);            
            
            if (abs(current_slope_count - mid) < min_difference){
        
                    // update min_difference and its pair of points
                    min_difference = abs(current_slope_count - mid);
                    points_to_display -> points[0] = point1;               
                    points_to_display -> points[1] = point2;  
            }
        }
    }

    cout << "min difference: " << min_difference << endl;
    cout << "point 1: (" << points_to_display -> points[0] -> coord[0] << ", " << points_to_display -> points[0] -> coord[1] << endl;
    cout << "point 2: (" << points_to_display -> points[1] -> coord[0] << ", " << points_to_display -> points[1] -> coord[1] << endl;

    // Pointers to the memory addresses
    point_t *ptr1 = points_to_display -> points[0];
    point_t *ptr2 = points_to_display -> points[1];


    // cout << "point 1: " << &ptr1 << endl;
    // cout << "point 2: " << &ptr2 << endl;

    return *points_to_display;
}