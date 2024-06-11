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

// helper comparison function for count_slopes: y at alpha
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

// helper comparison function for count_slopes: y at beta
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

// function declaration
int mergesort_and_count(vector<int>& arr, vector<int>& temp, int left, int right);
int merge_and_count(vector<int>& arr, vector<int>& temp, int left, int mid, int right);

// sort the array and count inversions
int count_inversions(vector<int>& arr) {
    vector<int> temp(arr.size());
    return mergesort_and_count(arr, temp, 0, arr.size() - 1);
}

// recursively sort and count inversions
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

// merge two sorted halves of the array and count inversions
int merge_and_count(vector<int>& arr, vector<int>& temp, int left, int mid, int right) {
    int i = left;    // starting index for left subarray
    int j = mid;     // starting index for right subarray
    int k = left;    // starting index to be sorted
    int inversionCount = 0;

    // merge the two halves while counting inversions
    while ((i <= mid - 1) && (j <= right)) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
            inversionCount += (mid - i);  // count inversions
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

// count the number of slopes between the range [alpha, beta]
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

    // initialize inverted order array
    vector<int> inverted_order;
    for (int i = 0; i < point_order.size(); i++) {
        int order = point_order[i].order;
        inverted_order.push_back(order);
    }

    // perform sort and count on inverted order array
    int inversionCount = count_inversions(inverted_order);

    return inversionCount;
}

//==============================================================================================
// display_points_v2
// 
// randomly select set of points and compute their slopes
// check if their slopes are in the range of alpha and beta, add to points_to_display if so
//==============================================================================================
point_t** display_points_v2(point_set_t* P, int s, double alpha, double beta, int num_iterations) {
    
    // initialize set of points to display
    point_t* points_to_display[s];

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

    cout << "min difference: " << min_difference << endl;
    cout << "point 1: (" << points_to_display[0] -> coord[0] << ", " << points_to_display[0] -> coord[1] << ")" << endl;
    cout << "point 2: (" << points_to_display[1] -> coord[0] << ", " << points_to_display[1] -> coord[1] << ")" << endl;


    // cout << "point 1: " << &ptr1 << endl;
    // cout << "point 2: " << &ptr2 << endl;

    return points_to_display;
}