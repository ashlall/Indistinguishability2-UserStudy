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

//==============================================================================================
// display_points
// randomly select set of points and compute their slopes
// check if their slopes are in the range of alpha and beta, add to s_hat if so
//==============================================================================================
point_pair_t display_points(point_set_t* P, int s, double alpha, double beta, int num_iterations){
    
    // place holders for the random two points that make a slope
    point_t* point1;
    point_t* point2;
    
    // create point pair object
    point_pair_t* current_point_pair;
    
    // create S hat point pair set     
    s_point_set_t* s_hat;                                               
   
    SLOPE_TYPE current_slope;
    point_pair_t* median_slope_pair;

    // initializing random index
    int rand_index1;
    int rand_index2;

    // generate random seed based on time
    srand (time(NULL));

    // generate random pairs of points, find their slope, see if it's in range, add to s_hat
    for (int i = 0; i < num_iterations; i++){
        
        // for each iteration, generate random pairs of points where random index is between 0 and numberOfPoints
        rand_index1 = rand() % P -> numberOfPoints;
        cout << "random index 1: " << rand_index1 << endl; 

        rand_index2 = rand() % P -> numberOfPoints;    
        cout << "random index 2: " << rand_index2 << endl;       

        
        // use random index to get random points from the whole point set
        point1 = P -> points[rand_index1];                  
        point2 = P -> points[rand_index2];

        // find slope of pair
        current_slope = compute_slope(point1, point2);          

        // Save object's attributes
        current_point_pair -> pair[0] = point1;               
        current_point_pair -> pair[1] = point2;
        current_point_pair -> slope = current_slope;

        // if slope is within bounds, add point pair to s_hat and increment numberOfPairs
        if (current_slope >= alpha && current_slope <= beta){
            s_hat -> point_pairs[s_hat -> numberOfPairs] = current_point_pair;
            s_hat -> numberOfPairs += 1;
        }
    }
    
    // find median slope (generalize for other S in the future)
    int median_index = (s_hat -> numberOfPairs) / 2;
    median_slope_pair = s_hat -> point_pairs[median_index];

    // returns the pair that gives median slope
    return *median_slope_pair; 
}