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

// helper comparison function for min_slope
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
    // NEED TO SORT
    int median_index = (s_hat -> numberOfPairs) / 2;
    median_slope_pair = s_hat -> point_pairs[median_index];

    // returns the pair that gives median slope
    return *median_slope_pair; 
}