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
SLOPE_TYPE compute_slope(point_t* p1, point_t* p2, int dim_a, int dim_i) {
    if (p2->coord[dim_a] == p1->coord[dim_a]) {
        return INF; 
    }
    return (p2->coord[dim_i] - p1->coord[dim_i]) / (p2->coord[dim_a] - p1->coord[dim_i]);
}

//==============================================================================================
// Helper function for comparison in min_slope for x - coord
//==============================================================================================

bool compare_points_x(const point_t& p1, const point_t& p2, int dim_a, int dim_i) {
    if (abs(p1.coord[dim_a] - p2.coord[dim_a]) < .0001) { 
         return p1.coord[dim_i] < p2.coord[dim_i];
    } 
    else {
        return p1.coord[dim_a] < p2.coord[dim_a];
    }
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
    SLOPE_TYPE min_slope = INF;
    for (int i = 0; i < P->numberOfPoints - 1; i++) {
        SLOPE_TYPE slope = compute_slope(P->points[i], P->points[i+1], dim_a, dim_i);
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
int count_slopes(point_set_t* P, double alpha, double beta, bool adjust) {
    if (adjust == true){
        alpha -= 0.0001;
        beta += 0.0001;
    }

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
//      s       - number of points to display each round
//      alpha   - lower threshold
//      beta    - upper threshold
// Return:
//      A set of points that most evenly divides the range ratio u_i / u_x
//==============================================================================================
point_t** display_points_v2(point_set_t* P, int s, double alpha, double beta, int num_iterations) {
    
    // edge case - when beta is smaller than the minimum
    // if (min_slope(P) > beta){
    //     ;
    // }

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

    int slope_count = count_slopes(P, alpha, beta, true);
    cout << "total initial slope count: " << slope_count << endl;

    int mid = slope_count / 2;
    if (slope_count % 2 != 0){
        mid++;
    }

    cout << "median slope count: " << mid << endl;

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
            
            current_slope_count = count_slopes(P, alpha, current_slope, true);    

            if (abs(current_slope_count - mid) == 0){
                points_to_display[0] = point1;               
                points_to_display[1] = point2;  
                return points_to_display;
            }
            else if (abs(current_slope_count - mid) < min_difference){

                    cout << "current estimate of median slope count: " << abs(current_slope_count - mid) << endl;
        
                    // update min_difference and its pair of points
                    min_difference = abs(current_slope_count - mid);
                    points_to_display[0] = point1;               
                    points_to_display[1] = point2;  
            }

        }
    }

    return points_to_display;
}

//==============================================================================================
// breakpoint_one_round
// Helper function for breakpoint - Simulate one round of interaction
// Parameters: 
//      P           - input data set
//      alpha       - lower slope threshold 
//      beta        - upper slope threshold
// Return:
//      slope of the most even breakpoint
//==============================================================================================

COORD_TYPE breakpoint_one_round(point_set_t* P, int s, double alpha, double beta) {
    // Set up I
    point_t** points_to_display = new point_t*[s]; 

    for (int i = 0; i < s; i++) {
        points_to_display[i] = nullptr; 
    }

    int num_iterations = 100;

    SLOPE_TYPE current_slope;
    double min_difference = INF;
    int rand_index1;
    int rand_index2;
    double current_slope_count;

    // calculate total slope between alpha and beta
    int total_slopes = count_slopes(P, alpha, beta);
    int mid = total_slopes / 2;

    // generate random seed based on time
    srand (time(NULL));

    for (int i = 0; i < num_iterations; i++){
        
        // generate random pairs of points
        int rand_index1 = rand() % P -> numberOfPoints;
        int rand_index2 = rand() % P -> numberOfPoints;    

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

    SLOPE_TYPE best_slope = compute_slope(points_to_display[0], points_to_display[1]); 

    return best_slope;
}

//==============================================================================================
// breakpoint_first_attempt
// Problem:
//      Cannot work with multiple dimesion yet, only work with 2 dimensions
//      -> fix other functions (all slope-related is 2-d) to work with multiple dimension
//      -> will take in a parameter to set anchor index (u_i/u_a instead of u_i/u_1) -> a replaces 1 for testing purposes
//      Lack pre-condition checking - e.g: whether negative slopes exist, what to do when no points to display
// Description:
//      Simulate interaction with multiple rounds of interaction
// Parameters: 
//      P           - input data set
//      u           - unknown utility vector
//      s           - number of points to display each round
//      maxRound    - maximum number of rounds of interactions / budget of questions
// Return:
//      For now, estimated utility vector
//      Not sure yet, add in later - What is the most helpful value to return?
//==============================================================================================

point_t* breakpoint_first_attempt(point_set_t* P, point_t* u, int s, int maxRound) {
    //Set up 
    int dim = P->points[0]->dim;
    point_t* estimated_u = new point_t;
    estimated_u->dim = dim;

    SLOPE_TYPE slope_breakpoint;
    COORD_TYPE ratio_breakpoint;

    double alpha = min_slope(P);
    double beta = 0;

    for (int round = 0; round < maxRound; round++) {
        // find points that divides the range alpha beta most evenly, take that slope
        slope_breakpoint = breakpoint_one_round(P, s, alpha, beta);
        ratio_breakpoint = -1 / slope_breakpoint;
        
        // simulate user interaction
        if (u->coord[1]/u->coord[0] < ratio_breakpoint) {
            beta = slope_breakpoint;
        }
        else {
            alpha = slope_breakpoint;
        }
    }

    // To do: write a funtion in header file to convert slope to ratio and vice versa for cleaner code
    // CONSIDER TAKE AVERAGE OF BREAKPOINT INSTEAD OF SLOPES
    SLOPE_TYPE average_slope = (beta + alpha) / 2;
    COORD_TYPE estimated_ratio = - 1 /average_slope;

    // Modify estimated_u - This is currently a bad way to do it
    estimated_u->coord = new double[2];
    estimated_u->coord[0] = 1;
    estimated_u->coord[1] = estimated_ratio;

    return estimated_u;
}

//==============================================================================================
// max_utility_breakpoint
// Precondition: Negative slopes exist, enought points to display
// Problem:
//      Cannot work with multiple dimesion yet, only work with 2 dimensions
//      -> fix other functions (all slope-related is 2-d) to work with multiple dimension
//      -> will take in a parameter to set anchor index (u_i/u_a instead of u_i/u_1) -> a replaces 1 for testing purposes
//      Lack pre-condition checking - e.g: whether negative slopes exist, what to do when no points to display
// Description:
//      Simulate interaction with multiple rounds of interaction
// Parameters: 
//      P           - input data set
//      a           - anchor dimension
//      u           - unknown utility vector
//      s           - number of points to display each round
//      epsilon     - the indistinguishability threshold
//      maxRound    - maximum number of rounds of interactions / budget of questions
//      Qcount      - the number of question asked
//      Csize       - the size the candidate set when finished
// Return:
//      alpha 
//==============================================================================================

double max_utility_breakpoint(point_set_t* P, int a, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize) {
    // Initialize L and H utility vectors
    int dim = P->points[0]->dim;
    vector<double> L, H;

    // Find min slope of each dimension, figure out how to exclude dimension a
    for (int i = 0; i < dim; i++) {
        L.push_back(min_slope(P, a, i));
        H.push_back(0);
    }

    int i = 0;
    while (true) {
    // In maxUtility, the looping condition will be while (QCount < maxRound)
        if (i == a) {
            i = (i % dim) + 1;
        }
        vector<double> B;
        for (int j = 0; j < s - 1; j++) {
            B.push_back(j * bi / s);
        }
    }


    
    return 0;
}
