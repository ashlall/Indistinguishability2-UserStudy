using namespace std;


#include "utility.h"
#include <time.h> 
#include <iostream>


//==============================================================================================
// compute_slope
// Computes the slope between points p1 and p2
// Parameters:
//      p1 - point 1
//      p2 - point 2
// Return: 
//      Slope between p1 and p2 
//==============================================================================================
SLOPE_TYPE compute_slope(point_t* p1, point_t* p2) {
    if (p2->coord[0] == p1->coord[0]) {
        return INF; 
    }
    return (p2->coord[1] - p1->coord[1]) / (p2->coord[0] - p1->coord[0]);
}

//==============================================================================================
// Helper function for comparison in min_slope
//==============================================================================================
bool compare_points_x(const point_t* p1, const point_t* p2) {
    return p1->coord[0] < p2->coord[0];
}


//==============================================================================================
// min_slope
// Bug:
//      Does not work correctly with equal x values
// Computes the minimum slope in a set of points P
// Parameters:
//      P - input data set
// Return: 
//      minimum slope in P
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
// Helper function for comparison in count_slopes - y at alpha
//==============================================================================================
bool compare_points_alpha(const point_t& p1, const point_t& p2, double alpha) {
    return (p1.coord[0] * alpha - p1.coord[1]) < (p2.coord[0] * alpha - p2.coord[1]);
}

struct Comparator_Alpha {
    double alpha;
    Comparator_Alpha(double f) : alpha(f) {}

    bool operator()(const point_t* p1, const point_t* p2) const {
        return compare_points_alpha(*p1, *p2, alpha);
    }
};

//==============================================================================================
// Helper function for comparison in count_slopes - y at beta
//==============================================================================================

bool compare_points_beta(const point_order_t& p1, const point_order_t& p2, double beta) {
    return (p1.point->coord[0] * beta - p1.point->coord[1]) < (p2.point->coord[0] * beta - p2.point->coord[1]);
}

struct Comparator_Beta {
    double beta;
    Comparator_Beta(double f) : beta(f) {}

    bool operator()(const point_order_t& p1, const point_order_t& p2) const {
        return compare_points_beta(p1, p2, beta);
    }
};

//==============================================================================================
// Helper functions for counting inversions in count_slopes
//==============================================================================================
int mergesort_and_count(vector<int>& arr, vector<int>& temp, int left, int right);
int merge_and_count(vector<int>& arr, vector<int>& temp, int left, int mid, int right);

//==============================================================================================
// count_inversions
// Counts the number of inversions in a vector / array with Merge Sort logic
// Parameters: 
//      arr - vector of intergers
// Postcondition:
//      arr is sorted in non-descending order
// Return:
//      number of inversions in arr
//==============================================================================================
int count_inversions(vector<int>& arr) {
    vector<int> temp(arr.size());
    return mergesort_and_count(arr, temp, 0, arr.size() - 1);
}

int mergesort_and_count(vector<int>& arr, vector<int>& temp, int left, int right) {
    int mid, inversionCount = 0;
    if (left < right) {
        mid = (left + right) / 2;

        // recursively sort and count inversions in the left half
        inversionCount += mergesort_and_count(arr, temp, left, mid);

        // recursively sort and count inversions in the right half
        inversionCount += mergesort_and_count(arr, temp, mid + 1, right);

        // merge the two halves and count inversions during merging
        inversionCount += merge_and_count(arr, temp, left, mid + 1, right);
    }
    return inversionCount;
}

int merge_and_count(vector<int>& arr, vector<int>& temp, int left, int mid, int right) {
    int i = left;    // starting index for left subarray
    int j = mid;     // starting index for right subarray
    int k = left;    // starting index to be sorted
    int inversionCount = 0;

    // merge the two halves while counting inversions
    while ((i <= mid - 1) && (j <= right)) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } 
        else {
            temp[k++] = arr[j++];
            inversionCount += (mid - i);    // count inversions
        }
    }

    // copy the remaining elements of left subarray, if any
    while (i <= mid - 1) {
        temp[k++] = arr[i++];
    }

    // copy the remaining elements of right subarray, if any
    while (j <= right) {
        temp[k++] = arr[j++];
    }

    // copy the merged elements back into the original array
    for (i = left; i <= right; i++) {
        arr[i] = temp[i];
    }

    return inversionCount;
}

//==============================================================================================
// count_slopes
// Counts the number of slopes in range [alpha, beta]
// Parameters: 
//      P       - input data set
//      alpha   - lower threshold
//      beta    - upper threshold
// Return:
//      number of slopes in range [alpha, beta]
//==============================================================================================
int count_slopes(point_set_t* P, double alpha, double beta) {
    // sort points based on y_at_alpha value
    sort(P->points, P->points + P->numberOfPoints, Comparator_Alpha(alpha));

    // copy original points into vector with order
    vector<point_order_t> point_order;
    for (int i = 0; i < P->numberOfPoints; ++i) {
        point_order_t p_order;
        // shallow copy for simplicity
        p_order.point = P->points[i];
        p_order.order = i + 1;
        point_order.push_back(p_order);
    }

    // sort points based on y_at_beta value
    sort(point_order.begin(), point_order.end(), Comparator_Beta(beta));

    // put order of y_at_beta into vector inverted_order
    vector<int> inverted_order;
    for (int i = 0; i < point_order.size(); i++) {
        int order = point_order[i].order;
        inverted_order.push_back(order);
    }

    // perform count_inversion / sort and count on inverted order array
    int inversionCount = count_inversions(inverted_order);

    return inversionCount;
}

//==============================================================================================
// display_points_v2
// Displays a set of points for interaction that most evenly 
// divides the range ratio u_i / u_x
// Parameters: 
//      P       - input data set
//      s       - number of points to display
//      alpha   - lower threshold
//      beta    - upper threshold
// Return:
//      A set of points that most evenly divides the range ratio u_i / u_x
//==============================================================================================
point_t** display_points_v2(point_set_t* P, int s, double alpha, double beta, int num_iterations) {
    
    // initialize set of points to display
    point_t** points_to_display = new point_t*[s]; 

    for (int i = 0; i < s; i++) {
        points_to_display[i] = nullptr; 
    }

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

        point_t* point1 = P -> points[rand_index1];                  
        point_t* point2 = P -> points[rand_index2];

        // compute slope
        current_slope = compute_slope(point1, point2);          

        // check if slope within range
        if (current_slope >= alpha && current_slope <= beta){
            
            current_slope_count = count_slopes(P, alpha, current_slope);            
            
            if (abs(current_slope_count - mid) < min_difference){
        
                    // update min_difference and its pair of points
                    min_difference = abs(current_slope_count - mid);
                    points_to_display[0] = point1;               
                    points_to_display[1] = point2;  
            }
        }
    }

    return points_to_display;
}