#include "data_utility.h"
#include "data_struct.h"
#include "read_write.h"
#include "maxUtility.h"
#include "medianhull.h"
#include <vector>
#include <ctime>
#include <ratio>
#include <chrono>
#include <map>
#include <cstdlib>  // for rand() and srand()
#include <ctime>    // for time()

using namespace std;

void run_test(point_set_t* P, point_set_t* cskyline, double epsilon, double delta, int s, int q, int user_id);
double user_feedback(string algorithm_name, int user_id);
double user_log(string order, int user_id);

int main(int argc, char *argv[])
{
  // parameters
  int s = 2;
  double epsilon = 0.01, delta;
  int q = 10;                // maximum number of rounds to run for
  int Qcount, Csize;
  int prune_option = RTREE;
  int dom_option = HYPER_PLANE;
  int stop_option = EXACT_BOUND;
  int cmp_option;
  int user_id;
  double alpha = 1;                   // factor by which we are willing to overestimate the size of the output set
 
  cout << "Plug in User ID: " ;
  cin >> user_id;

  char* filename = "user_study_data.txt";   // TODO: Update type in case too long
  char type;
  bool real, fake;
  point_set_t *P, *skyline, *cskyline;

  if (argc < 9)
  {
    //cout << "Usage: run type[S/Q/P/E/D/B/n/d/t/T] filename epsilon delta s q [R]eal/[F]ake/[B]oth seed" << endl;
    return 0;
  }

  type = argv[1][0];

  filename = argv[2];

  epsilon = stod(argv[3]);

  delta = stod(argv[4]);

  s = stoi(argv[5]);

  q = stoi(argv[6]);

  real = argv[7][0] == 'R' || argv[7][0] == 'r' || argv[7][0] == 'B' || argv[7][0] == 'b';
  fake = argv[7][0] == 'F' || argv[7][0] == 'f' || argv[7][0] == 'B' || argv[7][0] == 'b';

  // use the random seed as a parameter to get repeatable results
  srand(atoi(argv[8]));


  if (type != 'n' && type != 'd')
  {
    //printf("file = %s\nepsilon = %lf, delta = %lf, s = %d, q = %d\n", filename, epsilon, delta, s, q);

    // read the dataset
    P = read_points(filename);
    //cout << "n = " << P->numberOfPoints << endl;
    
    skyline = c_skyline_point(1.0, P);
    //cout << "skyline size = " << skyline->numberOfPoints << endl;
  
    if (P->numberOfPoints <= 2000000)
    {
      cskyline = c_skyline_point(1.0 + epsilon, P);
      //cout << "c-skyline size = " << cskyline->numberOfPoints << endl;
    }
    else
      {
	cout << "Data set too large to compute c-skyline" << endl;
	cskyline = P;
      }

    int dim = P->points[0]->dim;
  }
  run_test(P, cskyline, epsilon, delta, s, q, user_id);
  
  return 0;
}

// running all the tests  
void run_test(point_set_t* P, point_set_t* cskyline, double epsilon, double delta, int s, int q, int user_id)
{
  int dim = P->points[0]->dim;
  int Qcount, Csize;
  point_t* p;
  int prune_option = RTREE;
  int dom_option = HYPER_PLANE;
  int stop_option = EXACT_BOUND;
  int cmp_option;
  double alpha = 1;                   // factor by which we are willing to overestimate the size of the output set
  int reps = 10;
  string ordering;

  // generate the utility vector
  point_t* u = alloc_point(dim);
  //cout << "Real utility is: " << endl;
  for (int i = 0; i < dim; i++) {
    u->coord[i] = ((double)rand()) / RAND_MAX;
    //cout << u->coord[i] << endl;
  }


  // look for the ground truth maximum utility point
  int maxIdx = 0;
  double maxValue = 0;
  for(int i = 0; i < cskyline->numberOfPoints; i++)
    {
      double value = dot_prod(u, cskyline->points[i]);
      if(value > maxValue)
	{
	  maxValue = value;
	  maxIdx = i;
	}
    }

  // count the number of points that are within epsilon of the optimal
  int Isize = 0;
  for(int i = 0; i < P->numberOfPoints; i++)
    {
      double value = dot_prod(u, P->points[i]);
      if(value * (1 + epsilon) > maxValue)
	    Isize++;
    }

  // Seed the random number generator
  // Random for the ordering of the algorithms, however, for each algorithm, the ordering has a random seed of 0.
    srand(time(0));

    // Array to store the function results
    double results[3];

    // Generate a random order number between 1 and 6
    int order = rand() % 6 + 1;

    // Run the functions in random order based on the random number
    switch (order) {
        case 1:
            ordering = "alpha_UH alpha_MD alpha_BP";
            cout << "Order: alpha_UH, alpha_MD, alpha_BP\n";
            user_log(ordering, user_id);

            cout << "Round 1 ---------------------------------------------------- \n\n";
            srand(0);

            results[0] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, RANDOM, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 1\n\n";

            cout << "\n\nFeedback for Round 1";
            user_feedback("UH_Random", user_id);


            cout << " Round 2 ----------------------------------------------------\n\n";
            srand(0);

            results[1] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, MIND, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 2\n\n";

            cout << "\n\nFeedback for Round 2";
            user_feedback("Min_D", user_id);


            cout << " Round 3 ----------------------------------------------------\n\n";
            srand(0);  
            results[2] = max_utility_breakpoint(cskyline, u, s, epsilon, delta, q, Qcount, Csize, user_id);
            cout << "Finished Round 3\n\n";

            cout << "\n\nFeedback for Round 3";
            user_feedback("Breakpoint", user_id);
            
            break;
        case 2:
            ordering = "alpha_UH alpha_BP alpha_MD";
            cout << "Order: alpha_UH, alpha_BP, alpha_MD\n";
            user_log(ordering, user_id);

            cout << " Round 1 ----------------------------------------------------\n\n";
                srand(0);

            results[0] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, RANDOM, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 1\n\n";

            cout << "\n\nFeedback for Round 1";
            user_feedback("UH_Random", user_id);
            

            cout << " Round 2 ----------------------------------------------------\n\n";
                srand(0);

            results[1] = max_utility_breakpoint(cskyline, u, s, epsilon, delta, q, Qcount, Csize, user_id);
            cout << "Finished Round 2\n\n";
          
            cout << "\n\nFeedback for Round 2";
            user_feedback("Breakpoint", user_id);

            cout << " Round 3 ----------------------------------------------------\n\n";
                srand(0);

            results[2] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, MIND, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 3\n\n";

            cout << "\n\nFeedback for Round 3";
            user_feedback("Min_D", user_id);
            
            break;
        case 3:
            ordering = "alpha_MD, alpha_UH, alpha_BP";
            cout << "Order: alpha_MD, alpha_UH, alpha_BP\n";
            user_log(ordering, user_id);


            cout << " Round 1 ----------------------------------------------------\n\n";
                srand(0);

            results[0] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, MIND, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 1\n\n";
            
            cout << "\n\nFeedback for Round 1";
            user_feedback("Min_D", user_id);


            cout << " Round 2 ----------------------------------------------------\n\n";
                srand(0);

            results[1] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, RANDOM, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 2\n\n";

            cout << "\n\nFeedback for Round 2";
            user_feedback("UH_Random", user_id);

            cout << " Round 3 ----------------------------------------------------\n\n";
                srand(0);

            results[2] = max_utility_breakpoint(cskyline, u, s, epsilon, delta, q, Qcount, Csize, user_id);
            cout << "Finished Round 3\n\n";

            cout << "\n\nFeedback for Round 3";
            user_feedback("Breakpoint", user_id);
            
            break;
        case 4:
            ordering = "alpha_MD alpha_BP alpha_UH";
            cout << "Order: alpha_MD, alpha_BP, alpha_UH\n";
            user_log(ordering, user_id);

            cout << " Round 1 ----------------------------------------------------\n\n";
                srand(0);

            results[0] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, MIND, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 1\n\n";
            
            cout << "\n\nFeedback for Round 1";
            user_feedback("Min_D", user_id);


            cout << " Round 2 ----------------------------------------------------\n\n";
                srand(0);

            results[1] = max_utility_breakpoint(cskyline, u, s, epsilon, delta, q, Qcount, Csize, user_id);
            cout << "Finished Round 2\n\n";

            cout << "\n\nFeedback for Round 2";
            user_feedback("Breakpoint", user_id);

            cout << " Round 3 ---------------------------------------------------\n\n";
                srand(0);

            results[2] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, RANDOM, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 3\n\n";

            cout << "\n\nFeedback for Round 3";
            user_feedback("UH_Random", user_id);

            break;
        case 5:
            ordering = "alpha_BP alpha_UH alpha_MD";
            cout << "Order: alpha_BP, alpha_UH, alpha_MD\n";
            user_log(ordering, user_id);

            cout << " Round 1 ----------------------------------------------------\n\n";
                srand(0);

            results[0] = max_utility_breakpoint(cskyline, u, s, epsilon, delta, q, Qcount, Csize, user_id);
            cout << "Finished Round 1\n\n";

            cout << "\n\nFeedback for Round 1";
            user_feedback("Breakpoint", user_id);

            cout << " Round 2 ----------------------------------------------------\n\n";
                srand(0);

            results[1] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, RANDOM, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 2\n\n";

            cout << "\n\nFeedback for Round 2";
            user_feedback("UH_Random", user_id);


            cout << " Round 3 ----------------------------------------------------\n\n";
                srand(0);

            results[2] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, MIND, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 3\n\n";

            cout << "\n\nFeedback for Round 3";
            user_feedback("Min_D", user_id);

            break;
        case 6:
            ordering = "alpha_BP alpha_MD alpha_UH";
            cout << "Order: alpha_BP, alpha_MD, alpha_UH\n";
            user_log(ordering, user_id);

            cout << " Round 1\n\n";
                srand(0);

            results[0] = max_utility_breakpoint(cskyline, u, s, epsilon, delta, q, Qcount, Csize, user_id);
            cout << "Finished Round 1\n\n";

            cout << "\n\nFeedback for Round 1";
            user_feedback("Breakpoint", user_id);

            cout << " Round 2\n\n";
                srand(0);

            results[1] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, MIND, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 2\n\n";

            cout << "\n\nFeedback for Round 2";

            user_feedback("Min_D", user_id);

            cout << " Round 3\n\n";
                srand(0);

            results[2] = max_utility(cskyline, u, s, epsilon, delta, q, Qcount, Csize, RANDOM, stop_option, prune_option, dom_option, reps, user_id);
            cout << "Finished Round 3\n\n";

            cout << "\n\nFeedback for Round 3";
            user_feedback("UH_Random", user_id);

            break;
    }

    // Output the results
    // for (int i = 0; i < 3; i++) {
    //     cout << "Result " << i + 1 << ": " << results[i] << "\n";
    // }

}

double user_feedback(string algorithm_name, int user_id) {
    // Satisfaction level input
    double satisfaction;

     cout << "\nPlease review the final output set. " <<  endl;
     cout << "\nEnter satisfaction level from the output result for " << algorithm_name << " from 1 (Not Satisfied) to 5 (Satisfied): ";

    // Loop until a valid integer between 1 and 5 is entered
    while (true) {
         cin >> satisfaction;

        // Check if the input is a valid integer in the range [1, 5]
        if ( cin.fail() || satisfaction < 1 || satisfaction > 5 || satisfaction != static_cast<int>(satisfaction)) {
             cin.clear(); // Clear the error flag
             cin.ignore( numeric_limits< streamsize>::max(), '\n'); // Ignore the invalid input
             cout << "Invalid input. Please enter an integer between 1 and 5: ";
        } else {
            break; // Exit the loop if the input is valid
        }
    }

    // Create and write to the satisfaction file
     string filename2 = "user_" +  to_string(user_id) + "_" + algorithm_name + "_satisfaction.txt";
     ofstream satisfaction_file(filename2);
    if (satisfaction_file.is_open()) {
        satisfaction_file << satisfaction << "\n";
        satisfaction_file.close();
    } else {
        cerr << "Unable to open file for writing satisfaction data.\n";
    }

    return satisfaction;
}

// Function to get user's effort input and write it to a file
double user_log(string order, int user_id) {

  double choice;

    // Create and write to the effort file
     string filename = "user_" +  to_string(user_id) + "_log.txt";
     ofstream effort_file(filename);
    if (effort_file.is_open()) {
        effort_file << order << "\n";
        effort_file.close();
    } else {
         cerr << "Unable to open file for writing effort data.\n";
    }

  return choice;
}