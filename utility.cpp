#include "utility.h"
#include <time.h> 
#include <iostream>

//==============================================================================================
// Helper function slope_to_ratio
//==============================================================================================
COORD_TYPE slope_to_ratio   (SLOPE_TYPE s) {
    return -1/s;
}

//==============================================================================================
// Helper function ratio_to_slope
//==============================================================================================
COORD_TYPE ratio_to_slope   (COORD_TYPE r) {
    return -1/r;
}

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
// Helper function for comparison in min_slope for x - coord
//==============================================================================================
bool compare_points_x(const point_t* p1, const point_t* p2) {
    // compares x values first, if same, then smaller y gets sort first
    if (abs(p1->coord[0] - p2->coord[0]) < .0001) { 
         return p1->coord[1] < p2->coord[1];
    } 
    else {
        return p1->coord[0] < p2->coord[0];
    }
}


//==============================================================================================
// min_slope
// Computes the minimum slope in a set of points P
// Parameters:
//      P       - input data set
//      0   - anchor dimension
//      dim_i   - current dimension
// Return: 
//      minimum slope in P in dimension a and i
//==============================================================================================
SLOPE_TYPE min_slope(point_set_t* P) {
    if (P->numberOfPoints < 2) {
        return INF;
    }

    // sort points based on x-coordinate
    sort(P->points, P->points + P->numberOfPoints, compare_points_x);
    // prints out in the sorted order
    for (int i = 0; i < P->numberOfPoints; i++) {
            cout << "x: " << P-> points[i]->coord[0]  << endl;
            cout << "y: " << P->points[i]->coord[1]  << endl << endl;
        }

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
int count_slopes(point_set_t* P, double alpha, double beta, bool adjust) {
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

//=============================== ALGORITHMS ================================

//==============================================================================================
// breakpoint_one_round
// Helper function for breakpoint - Simulate one round of interaction
// 2 dimensions and s = 2
// Parameters: 
//      P           - input data set
//      s           - number of options in one round
//      alpha       - lower slope threshold 
//      beta        - upper slope threshold
// Return:
//      slope of the most even breakpoint
//==============================================================================================

SLOPE_TYPE breakpoint_one_round(point_set_t* P, int s, double alpha, double beta) {
    // calculate total slope between alpha and beta
    int total_slopes = count_slopes(P, alpha, beta, true);
    int mid = total_slopes / 2;
    // round up
    if (total_slopes % 2 != 0){
        mid++;             
    }
    
    // sampling set up
    point_t* S[s];   
    point_t* S_best[s];
    srand (time(NULL));
    SLOPE_TYPE current_slope;
    int current_slope_count;        
    double min_difference = INF;

    for (int i = 0; i < 100; i++){
        // generate random set of points
        for (int i = 0; i < s; i++) {
            S[i] = P -> points[rand() % P -> numberOfPoints];
        }

        // compute slope of this set, for now s = 2
        current_slope = compute_slope(S[0], S[1]);          

        // check if slope within range
        if (current_slope >= alpha && current_slope <= beta){
            
            current_slope_count = count_slopes(P, alpha, current_slope, true);       

            if (abs(current_slope_count == mid)){
                return mid;
            }
            else if (abs(current_slope_count - mid) < min_difference){
                    // update min_difference and its set of points
                    min_difference = abs(current_slope_count - mid);
                    for (int i = 0; i < s; i++) {
                        S_best[i] = S[i];
                    }
            }    
        }
    }

    SLOPE_TYPE best_slope = compute_slope(S_best[0], S_best[1]); 

    return best_slope;
}


//==============================================================================================
// max_utility_breakpoint
// Description:
//      Simulate interaction with multiple rounds of interaction and 2-dimensional tuples
//      2 dimensions and s = 2
// Parameters: 
//      P           - input data set
//      u           - unknown utility vector
//      s           - number of points to display each round
//      maxRound    - maximum number of rounds of interactions / budget of questions
// Return:
//      alpha       - approximation
//==============================================================================================

double max_utility_breakpoint(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize) {

    // set of candidate tuples
    vector<int> C_idx;
    for(int i = 0; i < P->numberOfPoints; i++) {
        C_idx.push_back(i); 
    }

    // set up 
    int dim = P->points[0]->dim;
    point_t* estimated_u = new point_t;
    estimated_u->dim = dim;

    SLOPE_TYPE slope_breakpoint;
    COORD_TYPE ratio_breakpoint;

    double alpha = min_slope(P);
    double beta = 0;

    while(Qcount < maxRound) {
        // find points that divides the range alpha beta most evenly, take that slope
        slope_breakpoint = breakpoint_one_round(P, s, alpha, beta);
        ratio_breakpoint = slope_to_ratio(slope_breakpoint);
        
        // simulate user interaction, update alpha and beta
        if (u->coord[1]/u->coord[0] < ratio_breakpoint) {
            beta = slope_breakpoint;
        }
        else {
            alpha = slope_breakpoint;
        }
    }

    COORD_TYPE ratio_alpha = slope_to_ratio(alpha);
    COORD_TYPE ratio_beta = slope_to_ratio(beta);
    COORD_TYPE estimated_ratio = (ratio_alpha + ratio_beta) / 2;

    // This should be in the beginning, but leave this here for the purpose of s = 2
    vector<double> L(dim), H(dim);

    L.push_back(1);
    L.push_back(ratio_alpha);

    H.push_back(1);
    H.push_back(ratio_beta);

    // find the highest value from the low-end of the user utilities
    double highest = 0.0;
    for (int j = 0; j < P->numberOfPoints; ++j)
    {
        double dot = 0.0;
        for(int k = 0; k < dim; ++k)
        dot += P->points[j]->coord[k] * L[k];
        if (dot > highest)
        highest = dot;
    }

    // prune all the points that have their high-end utility (1+epsilon) dominated
    C_idx.clear();
    for (int j = 0; j < P->numberOfPoints; ++j)
    {
        double dot = 0.0;
        for(int k = 0; k < dim; ++k)
        dot += P->points[j]->coord[k] * H[k];
        if (dot * (1 + epsilon) >= highest)
        C_idx.push_back(j);
    }

    // Find out how well this did:
    double max_value = 0;
    for(int i = 0; i < P->numberOfPoints; i++)
    {
        double value = dot_prod(u, P->points[i]);
        if(value > max_value)
        max_value = value;
    }

    int inI = 0;
    double alpha_approx = 0.0;
    double avg_effective_epsilon = 0.0, max_effective_epsilon = 0.0;
    for(int i = 0; i < C_idx.size(); i++)
        {
        double value = dot_prod(u, P->points[C_idx[i]]);
        if(value * (1 + epsilon) > max_value)
        inI++;
        else
        {
        avg_effective_epsilon += max_value/value - 1.0;
        if (max_value/value - 1.0 > max_effective_epsilon)
            max_effective_epsilon = max_value/value - 1.0;

        if (max_value - value * (1 + epsilon) > alpha_approx)
            alpha_approx = max_value - value * (1 + epsilon);
        }
        }
    if (C_idx.size() - inI > 0)
        avg_effective_epsilon /= C_idx.size() - inI;
    printf("Found %d in I; %d false positives; alpha was %lf; avg effective epsilon was %lf; max effective epsilon was %lf.\n", inI, C_idx.size() - inI, alpha, avg_effective_epsilon, max_effective_epsilon);
    Csize = C_idx.size();

    return alpha_approx;
}

int main (void) {
    // ADD CODE HERE TO DEBUG MAX_UTILITY_BREAKPOINT
    return 0;
}