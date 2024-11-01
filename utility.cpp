#include "utility.h"

//==============================================================================================
// Helper function slope_to_ratio
//==============================================================================================
COORD_TYPE slope_to_ratio   (SLOPE_TYPE s) {
    return (-1/s);
}

//==============================================================================================
// Helper function ratio_to_slope
//==============================================================================================
COORD_TYPE ratio_to_slope   (COORD_TYPE r) {
    return (-1/r);
}

//==============================================================================================
// compute_slope
// Computes the slope between points p1 and p2
// Parameters://      p1 - point 1
//      p2 - point 2
// Return: 
//      Slope between p1 and p2 
//==============================================================================================
SLOPE_TYPE compute_slope(point_t* p1, point_t* p2, int dim_a, int dim_i) {
    if (p2->coord[dim_a] == p1->coord[dim_a]) {
        return INF; 
    }
    return (p2->coord[dim_i] - p1->coord[dim_i]) / (p2->coord[dim_a] - p1->coord[dim_a]);
}

//==============================================================================================
// Helper function for comparison in min_slope for x - coord
//==============================================================================================
bool compare_points_x(const point_t& p1, const point_t& p2, int dim_a, int dim_i) {
    // edge case - if x values are equal, sort by y
    if (abs(p1.coord[dim_a] - p2.coord[dim_a]) < .000001) { 
         return p1.coord[dim_i] < p2.coord[dim_i];
    } 
    return p1.coord[dim_a] < p2.coord[dim_a];
}

struct Comparator_X {
    int dim_a;
    int dim_i;
    Comparator_X(int a, int i) : dim_a(a), dim_i(i) {}

    bool operator()(const point_t* p1, const point_t* p2) const {
        return compare_points_x(*p1, *p2, dim_a, dim_i);
    }
};

//==============================================================================================
// min_slope
// Computes the minimum slope in a set of points P
// Parameters:
//      P       - input data set
//      dim_a   - anchor dimension
//      dim_i   - current dimension
// Return: 
//      minimum slope in P in dimension a and i
//==============================================================================================
SLOPE_TYPE min_slope(point_set_t* P, int dim_a, int dim_i) {
    if (P->numberOfPoints < 2) {
        return INF;
    }

    // sort points based on x-coordinate
    sort(P->points, P->points + P->numberOfPoints, Comparator_X(dim_a, dim_i));

    // compute slopes of adjacent points
    SLOPE_TYPE min_slope_value = INF;
    for (int i = 0; i < P->numberOfPoints - 1; i++) {
        SLOPE_TYPE slope = compute_slope(P->points[i], P->points[i+1], dim_a, dim_i);
        if (slope < min_slope_value) {
            min_slope_value = slope;
        }
    }
    return min_slope_value;
}

//==============================================================================================
// Helper function for comparison in count_slopes - y at alpha
//==============================================================================================
bool compare_points_alpha(const point_t& p1, const point_t& p2, double alpha, int dim_a, int dim_i) {
    return (p1.coord[dim_a] * alpha - p1.coord[dim_i]) < (p2.coord[dim_a] * alpha - p2.coord[dim_i]);
}

struct Comparator_Alpha {
    double alpha;
    int dim_a;
    int dim_i;
    Comparator_Alpha(double f, int a, int i) : alpha(f), dim_a(a), dim_i(i) {}

    bool operator()(const point_t* p1, const point_t* p2) const {
        return compare_points_alpha(*p1, *p2, alpha, dim_a, dim_i);
    }
};

//==============================================================================================
// Helper function for comparison in count_slopes - y at beta
//==============================================================================================

bool compare_points_beta(const point_order_t& p1, const point_order_t& p2, double beta, int dim_a, int dim_i) {
    return (p1.point->coord[dim_a] * beta - p1.point->coord[dim_i]) < (p2.point->coord[dim_a] * beta - p2.point->coord[dim_i]);
}

struct Comparator_Beta {
    double beta;
    int dim_a;
    int dim_i;
    Comparator_Beta(double f, int a, int i) : beta(f), dim_a(a), dim_i(i) {}

    bool operator()(const point_order_t& p1, const point_order_t& p2) const {
        return compare_points_beta(p1, p2, beta, dim_a, dim_i);
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
//
// Important Note: The default parameter value "adjust" is set to false
//
// Parameters: 
//      P       - input data set
//      alpha   - lower threshold
//      beta    - upper threshold
//      adjust  - indicates whether to expand alpha and beta range to include the inversions
//                happening exactly at alpha and beta by a small amount
// Return:
//      number of slopes in range [alpha, beta]
//==============================================================================================
int count_slopes(point_set_t* P, double alpha, double beta, bool adjust, int dim_a, int dim_i) {
    if (adjust == true){
        alpha -= 0.0001;
        beta += 0.0001;
    }

    // sort points based on y_at_alpha value
    sort(P->points, P->points + P->numberOfPoints, Comparator_Alpha(alpha, dim_a, dim_i));

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
    sort(point_order.begin(), point_order.end(), Comparator_Beta(beta, dim_a, dim_i));

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
// Helper function for comparison in sampled set
//==============================================================================================

bool compare_points_x_reverse(const point_t& p1, const point_t& p2, int dim_a, int dim_i) {
    // edge case - if x values are equal, sort by y
    if (abs(p1.coord[dim_a] - p2.coord[dim_a]) < .000001) { 
         return p1.coord[dim_i] > p2.coord[dim_i];
    } 
    return p1.coord[dim_a] > p2.coord[dim_a];
}

struct Comparator_X_Reverse {
    int dim_a;
    int dim_i;
    Comparator_X_Reverse(int a, int i) : dim_a(a), dim_i(i) {}

    bool operator()(const point_t* p1, const point_t* p2) const {
        return compare_points_x_reverse(*p1, *p2, dim_a, dim_i);
    }
};

//=============================== ALGORITHMS ================================

//==============================================================================================
// breakpoint_one_round
// Helper function for breakpoint - Simulate one round of interaction
// Parameters: 
//      P           - input data set
//      s           - number of options in one round
//      alpha       - lower slope threshold 
//      beta        - upper slope threshold
//      dim_a       - anchor dimension
//      dim_i       - dimension
// Return:
//      array of pointers to points to display
//==============================================================================================

point_t** breakpoint_one_round(point_set_t* P, int s, double alpha, double beta, int dim_a, int dim_i) 
{
    // calculate total slope between alpha and beta
    int total_slopes = count_slopes(P, alpha, beta, true, dim_a, dim_i);
    
    // sampling set up
    bool            found_best  = false;
    int             good_samples = 0;

    // array of pointers, not dynamically allocated
    point_t*        S[s]; 
    point_t*        S_best[s];    

    int             min_V       = INF;

    for (int j = 0; j < 100; j++) {
        // generate random set of points
        for (int i = 0; i < s; i++) {
            S[i] = P -> points[rand() % (P -> numberOfPoints)];
        }

        // sort randomly sampled points in descending order
        sort(S, S + s, Comparator_X_Reverse(dim_a, dim_i));

        // compute slope
        SLOPE_TYPE slope_S = compute_slope(S[0], S[1], dim_a, dim_i);

        // line 14
        if (slope_S > alpha && slope_S < beta) {
            good_samples++;

            SLOPE_TYPE      X[] = {alpha, slope_S, beta};

            // line 15
            int num_slopes_S = count_slopes(P, alpha, slope_S, true, dim_a, dim_i);

            // reset V 
            int V = abs(num_slopes_S - total_slopes/s);

            // maintain set S that minimizes V
            if (V < min_V) {
                // update min_V
                min_V = V;

                found_best = true;
                for (int i = 0; i < s; i++) {
                    S_best[i] = S[i];
                }
            }
        }
    }

    if (DEBUG) {
        cout << "The number of satisfying samples is " << good_samples << endl; 
    }

    // Copy the result to a dynamically allocated array to return
    if (found_best) { 
        point_t** result = new point_t*[s];
        for (int i = 0; i < s; ++i) {
            result[i] = S_best[i];
        }

        return result;
    }   
    else {
        return nullptr;
    }
}

// Extra version to test vary T     
//      repeats         - number of samples

point_t** breakpoint_one_round(point_set_t* P, int s, double alpha, double beta, int dim_a, int dim_i, int repeats) 
{
    // calculate total slope between alpha and beta
    int total_slopes = count_slopes(P, alpha, beta, true, dim_a, dim_i);
    
    // sampling set up
    bool            found_best  = false;
    int             good_samples = 0;

    // array of pointers, not dynamically allocated
    point_t*        S[s]; 
    point_t*        S_best[s];    

    int             min_V       = INF;

    for (int j = 0; j < repeats; j++) {
        // generate random set of points
        for (int i = 0; i < s; i++) {
            S[i] = P -> points[rand() % (P -> numberOfPoints)];
        }

        // sort randomly sampled points in descending order
        sort(S, S + s, Comparator_X_Reverse(dim_a, dim_i));

        // compute slope
        SLOPE_TYPE slope_S = compute_slope(S[0], S[1], dim_a, dim_i);

        // line 14
        if (slope_S > alpha && slope_S < beta) {
            good_samples++;

            SLOPE_TYPE      X[] = {alpha, slope_S, beta};

            // line 15
            int num_slopes_S = count_slopes(P, alpha, slope_S, true, dim_a, dim_i);

            // reset V 
            int V = abs(num_slopes_S - total_slopes/s);

            // maintain set S that minimizes V
            if (V < min_V) {
                // update min_V
                min_V = V;

                found_best = true;
                for (int i = 0; i < s; i++) {
                    S_best[i] = S[i];
                }
            }
        }
    }

    if (DEBUG) {
        cout << "The number of satisfying samples is " << good_samples << endl; 
    }

    // Copy the result to a dynamically allocated array to return
    if (found_best) { 
        point_t** result = new point_t*[s];
        for (int i = 0; i < s; ++i) {
            result[i] = S_best[i];
        }

        return result;
    }   
    else {
        return nullptr;
    }
}





