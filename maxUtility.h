#ifndef MAXUTILITY_H
#define MAXUTILITY_H

#include "data_struct.h"
#include "data_utility.h"
#include "read_write.h"

#include <vector>
#include <algorithm> 

#include "rtree.h"
#include "lp.h"
#include "pruning.h"
#include "utility.h"
#include <queue>
#include <chrono>
#include <fstream>
#include <map>
#include <iomanip>  // For setting width and formatting

#define RANDOM 1
#define SIMPLEX 2
#define SIMPLEX_FLY 3
#define GREEDY 4
#define MINC 5
#define MINR 6
#define MIND 7

using namespace std;

// get the index of the "current best" point
int get_current_best_pt(point_set_t* P, vector<int>& C_idx, vector<point_t*>& ext_vec);

// generate s cars for selection in a round
void update_ext_vec(point_set_t* P, vector<point_t*> S, vector<int>& C_idx, int max_i, int s, vector<point_t*>& ext_vec, int& current_best_idx, int& last_best, vector<int>& frame, int cmp_option, double delta);

// generate the options for user selection and update the extreme vecotrs based on the user feedback
vector<int> generate_S(point_set_t* P, vector<int>& C_idx, int s, vector<point_t*> ext_vec, int current_best_idx, int& last_best, vector<int>& frame, int cmp_option, int stop_option, int prune_option, int dom_option, double epsilon, double delta, int repeats);

// ask for user's input on which option they prefer
//int get_user_choice(vector<point_t*> S); 

// ask for user's input on which option they prefer for breakpoint
//int get_user_choice_breakpoint(point_t** points_to_display, int a, int i); 

// ask for user's input on which option they prefer
int get_user_choice(vector<point_t *> S_real, point_t** points_to_display, int a, int i, int user_id, int cmp_option);

// the main interactive algorithm for real points
double max_utility(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize,  int cmp_option, int stop_option, int prune_option, int dom_option, int repeats, int user_id);

// the main interactive algorithm for fake points
double max_utility_fake(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int stopSize, int &Qcount, int &Csize);

// random algorithm for fake points
double random_fake(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize);

// the main interactive breakpoint algorithm for real points
double max_utility_breakpoint(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize, int user_id);
double max_utility_TT(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize);

#endif
