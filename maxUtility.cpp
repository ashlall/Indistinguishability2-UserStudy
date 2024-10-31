#include "maxUtility.h"

// get the index of the "current best" point
// P: the input car set
// C_idx: the indexes of the current candidate favorite car in P
// ext_vec: the set of extreme vecotr
int get_current_best_pt(point_set_t* P, vector<int>& C_idx, vector<point_t*>& ext_vec)
{
	int dim = P->points[0]->dim;

	// the set of extreme points of the candidate utility range R
	vector<point_t*> ext_pts;
	ext_pts = get_extreme_pts(ext_vec);

	// use the "mean" utility vector in R (other strategies could also be used)
	point_t* mean = alloc_point(dim);
	for(int i = 0; i < dim; i++)
	{
		mean->coord[i] = 0;
	}
	for(int i = 0; i < ext_pts.size(); i++)
	{
		for(int j = 0; j < dim; j++)
			mean->coord[j] += ext_pts[i]->coord[j];
	}
	for(int i = 0; i < dim; i++)
	{
		mean->coord[i] /= ext_pts.size();
	}

	// look for the maximum utility point w.r.t. the "mean" utility vector
	int best_pt_idx;
	double max = 0;
	for(int i = 0; i < C_idx.size(); i++)
	{
		point_t* pt = P->points[C_idx[i]];

		double v = dot_prod(pt, mean);
		if(v > max)
		{
			max = v;
			best_pt_idx =  C_idx[i];
		}
	}

	release_point(mean);

	for(int i = 0; i < ext_pts.size(); i++)
		release_point(ext_pts[i]);
	return best_pt_idx;
}

// generate s cars for selection in a round
// P: the input car set
// C_idx: the indexes of the current candidate favorite car in P
// s: the number of cars for user selection
// ext_vec: Current bounds on utility function; needed for repeated random selection
// frame: the frame for obtaining the set of neibouring vertices of the current best vertiex (used only if cmp_option = SIMPLEX)
// cmp_option: the car selection mode, which must be either SIMPLEX or RANDOM
// prune_option: Needed for the repeated random selection technique
// epsilon: the desired indistinguishability threshold
// delta: the indistinguishability threshold for user selection
vector<int> generate_S(point_set_t* P, vector<int>& C_idx, int s, vector<point_t*> ext_vec,  int current_best_idx, int& last_best, vector<int>& frame, int cmp_option, int stop_option, int prune_option, int dom_option, double epsilon, double delta, int repeats)
{
	// the set of s cars for selection
        vector<int> S;
	vector<int> C_idx_backup;
	vector<point_t*> ext_vec_backup;
	

	if(cmp_option == RANDOM) // RANDOM car selection mode
	{
		// randoming select at most s non-overlaping cars in the candidate set 
		while(S.size() < s && S.size() < C_idx.size())
		{
			int idx = rand() % C_idx.size();

			bool isNew = true;
			for(int i = 0; i < S.size(); i++)
			{
				if(S[i] == idx)
				{
					isNew = false;
					break;
				}
			}
			if(isNew)
				S.push_back(idx);
		}
	}
	else if(cmp_option == SIMPLEX) // SIMPLEX car selection mode
	{
		if(last_best != current_best_idx || frame.size() == 0) // the new frame is not computed before (avoid duplicate frame computation)
		{
			// create one ray for each car in P for computing the frame
			vector<point_t*> rays;
			int best_i = -1;
			for(int i = 0; i < P->numberOfPoints; i++)
			{
				if(i == current_best_idx)
				{
					best_i = i;
					continue;
				}

				point_t* best = P->points[current_best_idx];
				point_t* newRay = sub(P->points[i], best);
				rays.push_back(newRay);
			}

			// frame compuatation
			frameConeFastLP(rays, frame);
		
			// update the indexes lying after current_best_idx
			for(int i = 0; i < frame.size(); i++)
			{
				if(frame[i] >= current_best_idx)
					frame[i]++;

				//S[i] = C_idx[S[i]];
			}

			for(int i = 0; i < rays.size(); i++)
				release_point(rays[i]);
		}

		//printf("current_best: %d, frame:", P->points[current_best_idx]->id);
		//for(int i = 0; i < frame.size(); i++)
		//	printf("%d ", P->points[frame[i]]->id);
		//printf("\n");

		//S.push_back(best_i);

		for(int j = 0; j < C_idx.size(); j++)
		{
			if(C_idx[j] == current_best_idx) // it is possible that current_best_idx is no longer in the candidate set, then no need to compare again
			{
				S.push_back(j);
				break;
			}
		}

		// select at most s non-overlaping cars in the candidate set based on "neighboring vertices" obtained via frame compuation
		for(int i = 0; i < frame.size() && S.size() < s; i++)
		{
			for(int j = 0; j < C_idx.size() && S.size() < s; j++)
			{
				if(C_idx[j] == current_best_idx)
					continue;

				if(C_idx[j] == frame[i])
					S.push_back(j);
			}
		}

		// if less than s car are selected, fill in the remaing one
		if (S.size() < s && C_idx.size() > s)
		{
			for (int j = 0; j < C_idx.size(); j++)
			{
				bool noIn = true;
				for (int i = 0; i < S.size(); i++)
				{
					if (j == S[i])
						noIn = false;
				}
				if (noIn)
					S.push_back(j);

				if (S.size() == s)
					break;
			}
		}
	}
	else if (cmp_option == MINC)
	{
	  double rr = 1;
	  vector<int> best_S;
	  double best_value = C_idx.size() + 1;

	  vector<point_t*> ext_pts;
	  vector<point_t*> hyperplanes;
	  hyperplane_t* hp = NULL;

	  if(dom_option == HYPER_PLANE)
	    ext_pts = get_extreme_pts(ext_vec); // in Hyperplane Pruning, we need the set of extreme points of R
	  else
	    {
	      // in Conical Pruning, we need bounding hyperplanes for the conical hull
	      get_hyperplanes(ext_vec, hp, hyperplanes);
	      if(stop_option != NO_BOUND) // if an upper bound on the regret ratio is needed, we need the set of extreme points of R
		ext_pts = get_extreme_pts(ext_vec);
	    }


	  if (ext_pts.size() == 0)
       	    cout << "ext_pts has size 0 at start of MINC --- problematic" << endl;

	  //cout << "computed ext_pts of size " << ext_pts.size() << endl;	    

	  // the upper bound of the regret ratio based on (the extreme ponits of) R
	  rr = 1;
	  
	  // run the adapted squential skyline algorihtm
	  int* sl = new int[C_idx.size()]; // this will hold the indexes of C_idx that are on the skyline
	  int sl_size = 0;
	  for (int i = 0; i < C_idx.size(); ++i)
	    {
	      int dominated = 0;
	      point_t* pt = P->points[C_idx[i]];

	      // check if pt is dominated by the skyline so far
	      for (int j = 0; j < sl_size && !dominated; ++j)
                {
                  if(dom(P->points[ C_idx[ sl[j] ] ], pt, ext_pts, hp, hyperplanes, ext_vec, dom_option, 0))
		    dominated = 1;
                }

	      if (!dominated)
                {
		  // eliminate any points in current skyline that it dominates
		  int m = sl_size;
		  sl_size = 0;
		  for (int j = 0; j < m; ++j)
		    {

		      if(!dom(pt, P->points[ C_idx[ sl[j] ] ], ext_pts, hp, hyperplanes, ext_vec, dom_option, 0))
			sl[sl_size++] = sl[j];
		    }
		  
		  // add this point as well
		  sl[sl_size++] = i;
                }
	    }

	  //printf("Skyline: %d points\n", sl_size);
	  //for(int i = 0; i < sl_size; ++i)
	  //  print_point(P->points[ C_idx [ sl[i]] ]);


	  // try 'repeats' random options
	  for (int rep = 0; rep < repeats; ++rep)
	  {
	    // randomly select at most s non-overlaping cars in the skyline
	    
	    S.clear();
	    while(S.size() < s && S.size() < sl_size)
	    {
	      int idx = rand() % sl_size; //C_idx.size();
	      
	      bool isNew = true;
	      for(int i = 0; i < S.size(); i++)
	      {
		if(S[i] == sl[idx]) // || dominates(P->points[ C_idx[ S[i] ] ], P->points[ C_idx[idx] ]) || dominates(P->points[ C_idx[ idx ] ], P->points[ C_idx[ S[i] ] ]))
		{
		  isNew = false;
		  break;
		}
	      }
	      if(isNew)
		S.push_back(sl[idx]);
	    }

	    vector<point_t*> S_real(S.size());
	    for(int i = 0; i < S.size(); ++i)
	      S_real[i] = P->points[ C_idx[ S[i] ] ];
	    

	    // if there are fewer than s points in the skyline, just return them
	    if (S.size() < s)
	    {
	      delete[] sl;
	      return S;
	    }

	    // see how many points don't get pruned for each option and keep track of the most left
            size_t most_left = 0;
	    double avg_left = 0.0;
	    	    
	    //cout << "s = " << s << endl;
	    //cout << "Points in S: " << endl;
	    //for(int j = 0; j < s; ++j)
	    //  print_point(P->points[ S[j] ]);
	    //cout << "Points in ext_vec:" << endl;
	    //for (int j = 0; j < ext_vec.size(); ++j)
	    //  print_point(ext_vec[j]);
	    //cout << "Testing " << repeats << endl;
	    
	    for (int max_i = 0; max_i < s; ++max_i)
	    {
	      // back up variables
	      C_idx_backup = C_idx;
	      ext_vec_backup = ext_vec;

	      // update the extreme vectors based on the user feedback of max_i
	      update_ext_vec(P, S_real, C_idx_backup, max_i, s, ext_vec_backup, current_best_idx, last_best, frame, cmp_option, delta);

	      //update candidate set
	      if(prune_option == SQL)
	        sql_pruning(P, C_idx_backup, ext_vec_backup, rr, stop_option, dom_option, epsilon);
	      else
	        rtree_pruning(P, C_idx_backup, ext_vec_backup, rr, stop_option, dom_option, epsilon);

	      // calculate number of points eliminated and update min
              if (C_idx_backup.size() > most_left)
                most_left = C_idx_backup.size();

	      // calculate the average left, too
	      avg_left += most_left / s;

	      //cout << C_idx_backup.size() << " " << most_left << endl;

	      // release newly created points in ext_vec_backup
	      for (int j = ext_vec.size(); j < ext_vec_backup.size(); ++j)
	      	release_point(ext_vec_backup[j]);
	    }
	    
	    //cout << "most left = " << most_left << endl;
	    
	    if (avg_left < best_value) //(most_left < best_value)
              {
                best_S = S;
                best_value = avg_left; //most_left;
              }
	  }

	  delete[] sl;
	  //cout << "best value = " << best_value << endl;	  
	  S = best_S;
	  //cout << "S:\n";
	  //for(int j = 0; j < S.size(); ++j)
	  //  print_point(P->points[ C_idx[ S[j] ] ]);
	}
	else if (cmp_option == MINR)
	  {
	    double rr = 1;
	    vector<int> best_S;
	    
	    vector<point_t*> ext_pts;
	    vector<point_t*> hyperplanes;
	    hyperplane_t* hp = NULL;

	    if(dom_option == HYPER_PLANE)
	      ext_pts = get_extreme_pts(ext_vec); // in Hyperplane Pruning, we need the set of extreme points of R
	    else
	      {
		// in Conical Pruning, we need bounding hyperplanes for the conical hull
		get_hyperplanes(ext_vec, hp, hyperplanes);
		if(stop_option != NO_BOUND) // if an upper bound on the regret ratio is needed, we need the set of extreme points of R
		  ext_pts = get_extreme_pts(ext_vec);
	      }

	    if (ext_pts.size() == 0)
	      {
		cout << "ext_pts has size 0 at start of MINR --- problematic" << endl;
		cout << "Ext vec:" << endl;
		for(int i = 0; i < ext_vec.size(); ++i)
		  print_point(ext_vec[i]);
	      }

	    //cout << "computed ext_pts of size " << ext_pts.size() << endl;

	    // the upper bound of the regret ratio based on (the extreme ponits of) R
	    rr = 1;

	    // run the adapted squential skyline algorihtm
	    int* sl = new int[C_idx.size()]; // this will hold the indexes of C_idx that are on the skyline
	    int sl_size = 0;
	    for (int i = 0; i < C_idx.size(); ++i)
	      {
		int dominated = 0;
		point_t* pt = P->points[C_idx[i]];

		// check if pt is dominated by the skyline so far
		for (int j = 0; j < sl_size && !dominated; ++j)
		  {
		    if(dom(P->points[ C_idx[ sl[j] ] ], pt, ext_pts, hp, hyperplanes, ext_vec, dom_option, 0))
		      dominated = 1;
		  }

		if (!dominated)
		  {
		    // eliminate any points in current skyline that it dominates
		    int m = sl_size;
		    sl_size = 0;
		    for (int j = 0; j < m; ++j)
		      {

			if(!dom(pt, P->points[ C_idx[ sl[j] ] ], ext_pts, hp, hyperplanes, ext_vec, dom_option, 0))
			  sl[sl_size++] = sl[j];
		      }	      

		    // add this point as well
		    sl[sl_size++] = i;
		  }
	      }

	    //printf("Skyline: %d points\n", sl_size);
	    //for(int i = 0; i < sl_size; ++i)
	    //  print_point(P->points[ C_idx [ sl[i]] ]);


	    double best_R_width = get_R_width(ext_pts) * 2;
	    //cout << "initial R width = " << best_R_width << endl;

	    // try 'repeats' random options
	    for (int rep = 0; rep < repeats; ++rep)
	      {
		// randomly select at most s non-overlaping cars in the skyline
		S.clear();
		while(S.size() < s && S.size() < sl_size) //C_idx.size())
		  {
		    int idx = rand() % sl_size; //C_idx.size();

		    bool isNew = true;
		    for(int i = 0; i < S.size(); i++)
		      {
			if(S[i] == sl[idx]) // C_idx[idx])
			  {
			    isNew = false;
			    break;
			  }
		      }
		    if(isNew)
		      S.push_back(sl[idx]); //idx);
		  }

		//cout << "S:\n";
		//for(int j = 0; j < S.size(); ++j)
		//  print_point(P->points[ C_idx[ S[j] ] ]);


		// if there are fewer than s points in the skyline, just return them
		if (S.size() < s)
		{
		  delete[] sl;
		  return S;
		}

		//cout << "s = " << s << endl;
		//cout << "Points in S: " << endl;
		//for(int j = 0; j < s; ++j)
		//  print_point(P->points[ S[j] ]);
		//cout << "Points in ext_vec:" << endl;
		//for (int j = 0; j < ext_vec.size(); ++j)
		//  print_point(ext_vec[j]);
		//cout << "Testing " << repeats << endl;


		vector<point_t*> S_real(S.size());
		for(int i = 0; i < S.size(); ++i)
		  S_real[i] = P->points[ C_idx[ S[i] ] ];

		double worst_R_width = 0.0;
		double avg_R_width = 0.0;

		for (int max_i = 0; max_i < S.size(); ++max_i)
		{
		  // back up variables
		  C_idx_backup = C_idx;
		  ext_vec_backup = ext_vec;
		  
		  // generate the options for user selection and update the extreme vectors based on the user feedback
		  update_ext_vec(P, S_real, C_idx_backup, max_i, s, ext_vec_backup, current_best_idx, last_best, frame, cmp_option, delta);
		  
		  //DON'T update candidate set 
		  //if(prune_option == SQL)
		  //  sql_pruning(P, C_idx_backup, ext_vec_backup, rr, stop_option, dom_option, epsilon);
		  //else
		  //  rtree_pruning(P, C_idx_backup, ext_vec_backup, rr, stop_option, dom_option, epsilon);
		  
		  
		  vector<point_t*> new_ext_pts = get_extreme_pts(ext_vec_backup);
		  double R_width = get_R_width(new_ext_pts);
		  
		  // calculate number of points eliminated and update min
		  if (R_width > worst_R_width)
		    worst_R_width = R_width;
		  
		  // calculate average as well
		  avg_R_width += R_width / s;

		  //cout << C_idx_backup.size() << " " << most_left << endl;
		  
		  // release newly created points in ext_vec_backup
		  for (int j = ext_vec.size(); j < ext_vec_backup.size(); ++j)
		    release_point(ext_vec_backup[j]);

		  // release points in new_ext_pts
		  for(int j = 0; j < new_ext_pts.size(); ++j)
		    release_point(new_ext_pts[j]);
		}
		
		if (avg_R_width < best_R_width) //(worst_R_width < best_R_width)
		{
		  best_S = S;
		  best_R_width = avg_R_width; //worst_R_width;
		}
		
		//cout << "Best R width : " << best_R_width << endl;
		
	      }
	    
	    delete[] sl;
	    S = best_S;
	    //cout << "Best width = " << best_R_width << endl;
	    //cout << "S has " << S.size() << " elements\n";
	    //for(int j = 0; j < S.size(); ++j)
	    //  print_point(P->points[ C_idx[ S[j] ] ]);
	  }
	else if (cmp_option == MIND)
          {
            double rr = 1;
            vector<int> best_S;

            vector<point_t*> ext_pts;
            vector<point_t*> hyperplanes;
            hyperplane_t* hp = NULL;

            if(dom_option == HYPER_PLANE)
              ext_pts = get_extreme_pts(ext_vec); // in Hyperplane Pruning, we need the set of extreme points of R
            else
              {
                // in Conical Pruning, we need bounding hyperplanes for the conical hull
                get_hyperplanes(ext_vec, hp, hyperplanes);
                if(stop_option != NO_BOUND) // if an upper bound on the regret ratio is needed, we need the set of extreme points of R
                  ext_pts = get_extreme_pts(ext_vec);
              }

            if (ext_pts.size() == 0)
              {
                //cout << "ext_pts has size 0 at start of MIND --- problematic" << endl;
                //cout << "Ext vec:" << endl;
                //for(int i = 0; i < ext_vec.size(); ++i)
                  //print_point(ext_vec[i]);
              }

            //cout << "computed ext_pts of size " << ext_pts.size() << endl;

            // the upper bound of the regret ratio based on (the extreme ponits of) R
            rr = 1;

	    // run the adapted squential skyline algorihtm
            int* sl = new int[C_idx.size()]; // this will hold the indexes of C_idx that are on the skyline
            int sl_size = 0;
            for (int i = 0; i < C_idx.size(); ++i)
              {
                int dominated = 0;
                point_t* pt = P->points[C_idx[i]];

                // check if pt is dominated by the skyline so far
                for (int j = 0; j < sl_size && !dominated; ++j)
                  {
                    if(dom(P->points[ C_idx[ sl[j] ] ], pt, ext_pts, hp, hyperplanes, ext_vec, dom_option, 0))
                      dominated = 1;
                  }

                if (!dominated)
                  {
                    // eliminate any points in current skyline that it dominates
                    int m = sl_size;
                    sl_size = 0;
                    for (int j = 0; j < m; ++j)
                      {

                        if(!dom(pt, P->points[ C_idx[ sl[j] ] ], ext_pts, hp, hyperplanes, ext_vec, dom_option, 0))
                          sl[sl_size++] = sl[j];
                      }

                    // add this point as well
                    sl[sl_size++] = i;
                  }
              }

            //printf("Skyline: %d points\n", sl_size);
            //for(int i = 0; i < sl_size; ++i)
            //  print_point(P->points[ C_idx [ sl[i]] ]);

	    double best_R_diameter = get_R_diameter(ext_pts) * 2;
            //cout << "initial R diameter = " << best_R_diameter << endl;

            // try 'repeats' random options
            for (int rep = 0; rep < repeats; ++rep)
              {
                // randomly select at most s non-overlaping cars in the skyline
                S.clear();
                while(S.size() < s && S.size() < sl_size) //C_idx.size())
                  {
                    int idx = rand() % sl_size; //C_idx.size();

                    bool isNew = true;
                    for(int i = 0; i < S.size(); i++)
                      {
                        if(S[i] == sl[idx]) // C_idx[idx])
                          {
                            isNew = false;
                            break;
                          }
                      }
                    if(isNew)
                      S.push_back(sl[idx]); //idx);
                  }

                //cout << "S:\n";
                //for(int j = 0; j < S.size(); ++j)
                //  print_point(P->points[ C_idx[ S[j] ] ]);


                // if there are fewer than s points in the skyline, just return them
                if (S.size() < s)
		  {
		    delete[] sl;
		    return S;
		  }

		//cout << "s = " << s << endl;
                //cout << "Points in S: " << endl;
                //for(int j = 0; j < s; ++j)
                //  print_point(P->points[ S[j] ]);
                //cout << "Points in ext_vec:" << endl;
                //for (int j = 0; j < ext_vec.size(); ++j)
                //  print_point(ext_vec[j]);
                //cout << "Testing " << repeats << endl;


                vector<point_t*> S_real(S.size());
                for(int i = 0; i < S.size(); ++i)
                  S_real[i] = P->points[ C_idx[ S[i] ] ];

                double worst_R_diameter = 0.0;
                double avg_R_diameter = 0.0;

                for (int max_i = 0; max_i < S.size(); ++max_i)
		  {
		    // back up variables
		    C_idx_backup = C_idx;
		    ext_vec_backup = ext_vec;

		    // generate the options for user selection and update the extreme vectors based on the user feedback
		    update_ext_vec(P, S_real, C_idx_backup, max_i, s, ext_vec_backup, current_best_idx, last_best, frame, cmp_option, delta);

		    //DON'T update candidate set
		    //if(prune_option == SQL)
		    //  sql_pruning(P, C_idx_backup, ext_vec_backup, rr, stop_option, dom_option, epsilon);
		    //else
		    //  rtree_pruning(P, C_idx_backup, ext_vec_backup, rr, stop_option, dom_option, epsilon);


		    vector<point_t*> new_ext_pts = get_extreme_pts(ext_vec_backup);
		    double R_diameter = get_R_diameter(new_ext_pts);

		    // calculate number of points eliminated and update min
		    if (R_diameter > worst_R_diameter)
		      worst_R_diameter = R_diameter;

		    // calculate average as well
		    avg_R_diameter += R_diameter / s;

		    //cout << C_idx_backup.size() << " " << most_left << endl;

		    // release newly created points in ext_vec_backup
		    for (int j = ext_vec.size(); j < ext_vec_backup.size(); ++j)
		      release_point(ext_vec_backup[j]);

		    // release points in new_ext_pts
		    for(int j = 0; j < new_ext_pts.size(); ++j)
		      release_point(new_ext_pts[j]);
		  }

                if (avg_R_diameter < best_R_diameter) //(worst_R_diameter < best_R_diameter)
		  {
		    best_S = S;
		    best_R_diameter = avg_R_diameter; //worst_R_diameter;
		  }

                //cout << "Best R diameter : " << best_R_diameter << endl;

              }

            delete[] sl;
            S = best_S;
            //cout << "Best diameter = " << best_R_diameter << endl;
            //cout << "S has " << S.size() << " elements\n";
            //for(int j = 0; j < S.size(); ++j)
            //  print_point(P->points[ C_idx[ S[j] ] ]);
	}
	else // for testing only. Do not use this!
	{
		vector<point_t*> rays;

		int best_i = -1;
		for(int i = 0; i < C_idx.size(); i++)
		{
			if(C_idx[i] == current_best_idx)
			{
				best_i = i;
				continue;
			}

			point_t* best = P->points[current_best_idx];

			point_t* newRay = sub(P->points[C_idx[i]], best);

			rays.push_back(newRay);
		}

		partialConeFastLP(rays, S, s - 1);
		if(S.size() > s - 1)
			S.resize(s - 1);
		for(int i = 0; i < S.size(); i++)
		{
			if(S[i] >= best_i)
				S[i]++;

			//S[i] = C_idx[S[i]];
		}
		S.push_back(best_i);


		for(int i = 0; i < rays.size(); i++)
			release_point(rays[i]);
	}
	return S;
}

// generate the options for user selection and update the extreme vecotrs based on the user feedback
// wPrt: record user's feedback
// P_car: the set of candidate cars with seqential ids
// S: The subset of tuples shown to the user
// skyline_proc_P: the skyline set of normalized cars
// C_idx: the indexes of the pruned approximation of I
// max_i: The index of the item in S the user prefers the most
// ext_vec: the set of extreme vecotr
// current_best_idx: the current best car
// last_best: the best car in previous interaction
// frame: the frame for obtaining the set of neibouring vertices of the current best vertiex (used only if cmp_option = SIMPLEX)
// cmp_option: the car selection mode, which must be either SIMPLEX or RANDOM
// delta: the amount of error (fraction) possible when a user makes a selection
void update_ext_vec(point_set_t* P, vector<point_t*> S, vector<int>& C_idx, int max_i, int s, vector<point_t*>& ext_vec, int& current_best_idx, int& last_best, vector<int>& frame, int cmp_option, double delta)
{
	// for each non-favorite car, create a new extreme vecotr
	for(int i = 0; i < S.size(); i++)
	{
		if(max_i == i)
			continue;

		point_t* scaled = scale(1 + delta, S[max_i]);
		point_t* tmp = sub(S[i], scaled); 
		//C_idx[S[i]] = -1;  // we can't necessarily eliminate this point

		point_t* new_ext_vec = scale(1 / calc_len(tmp), tmp);
		
		release_point(tmp);
		release_point(scaled);
		ext_vec.push_back(new_ext_vec);
	}

	// directly remove the non-favorite car from the candidate set
	//vector<int> newC_idx;
	//for(int i = 0; i < C_idx.size(); i++)
	//{
	//	if(C_idx[i] >= 0)
	//		newC_idx.push_back(C_idx[i]);
	//}
	//C_idx = newC_idx;
}


// the main interactive algorithm for real points
// P: the input dataset (assumed skyline)
// u: the unkonwn utility vector
// s: the question size
// epsilon: the desired indistinguishability threshold 
// delta: the indistinguishability threshold for user selection
// maxRound: the maximum number of rounds of interacitons
// Qcount: the number of question asked
// Csize: the size the candidate set when finished
// cmp_option: the car selection mode, which must be either SIMPLEX or RANDOM
// stop_option: the stopping condition, which must be NO_BOUND or EXACT_BOUND or APRROX_BOUND
// prune_option: the skyline algorithm, which must be either SQL or RTREE
// dom_option: the domination checking mode, which must be either HYPER_PLANE or CONICAL_HULL
// number of times to perform repeats for MINR and MINC algorithms
double max_utility(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize,  int cmp_option, int stop_option, int prune_option, int dom_option, int repeats, int user_id)
{	
	int dim = P->points[0]->dim;

	// the indexes of the candidate set
	// initially, it is all the skyline cars
	vector<int> C_idx;
	for(int i = 0; i < P->numberOfPoints; i++)
		C_idx.push_back(i);

	double time;

	// the initial exteme vector sets V = {−ei | i ∈ [1, d], ei [i] = 1 and ei [j] = 0 if i , j}.
	vector<point_t*> ext_vec;
	for (int i = 0; i < dim; i++)
	{
		point_t* e = alloc_point(dim);
		for (int j = 0; j < dim; j++)
		{
			if (i == j)
				e->coord[j] = -1;
			else
				e->coord[j] = 0;
		}
		ext_vec.push_back(e);
	}

	int current_best_idx = -1;
	int last_best = -1;
	vector<int> frame;

	// get the index of the "current best" point
	//if(cmp_option != RANDOM)
	current_best_idx = get_current_best_pt(P, C_idx, ext_vec);
	
	// if not skyline
	//sql_pruning(P, C_idx, ext_vec);

	// Qcount - the number of querstions asked
	// Csize - the size of the current candidate set

	Qcount = 0;
	double rr = 1;

	int max_i;
	double max;

	// interactively reduce the candidate set and shrink the candidate utility range
	while (Qcount <  maxRound)  // while none of the stopping conditiong is true
	{
		Qcount++;
		sort(C_idx.begin(), C_idx.end()); // prevent select two different points after different skyline algorithms

		// generate s cars for selection in a round
		vector<int> S_idx = generate_S(P, C_idx, s, ext_vec, current_best_idx, last_best, frame, cmp_option, stop_option, prune_option, dom_option, epsilon, delta, repeats);
		
		vector<point_t*> S_real(S_idx.size());
		for(int i = 0; i < S_idx.size(); ++i)
		  S_real[i] = P->points[C_idx[S_idx[i]]];

		//cout << "** computed S of size " << S_idx.size() << endl;

		// if we have only one skyline point left, we can prune down to its optimal and quit
		if (S_idx.size() == 1)
		{
		//   double max_value = dot_prod(u, P->points[C_idx[S_idx[0]]]);
		//   vector<int> I_idx;
		//   for(int i = 0; i < C_idx.size(); ++i)
		//     if (dot_prod(u, P->points[C_idx[i]]) * (1 + epsilon) >= max_value)
		//       I_idx.push_back(C_idx[i]);
		//   C_idx = I_idx;
		  break; 
		}

		// find the user's favorite point in S_real
		// max = -1;
		// for(int i = 0; i < S_real.size(); i++)
		// {
		//   double v = dot_prod(u, S_real[i]);
		//   if(v > max)
		//     max = v;
		// }

		// FINDING THE USER'S FAVORITE POINT IN S_real with actual users

        // Present the options to the user and get their choice
        int user_choice = get_user_choice(S_real, nullptr, -1, -1, user_id, cmp_option);

		// randomly select one of the delta-indistinguishable ones
		// vector<int> options;
		// for(int i = 0; i < S_real.size(); i++)
		// {
		//   double v = dot_prod(u, S_real[i]);
		//   if(v * (1 + delta) >= max)
		//     options.push_back(i);
		// }

		// max_i = options[rand() % options.size()];
		max_i = user_choice;

		//cout << "** got user's fave\n";
		//cout << max << " " << max_i << endl;
		

		// get the better car among those from the user
		last_best = current_best_idx;
		current_best_idx = C_idx[S_idx[max_i]];
		
		// update the extreme vectors based on the user feedback
		update_ext_vec(P, S_real, C_idx, max_i, s, ext_vec, current_best_idx, last_best, frame, cmp_option, delta);

		if(C_idx.size() == 1 ) // || global_best_idx == current_best_idx
			break;

		//update candidate set
		if(prune_option == SQL)
		  sql_pruning(P, C_idx, ext_vec, rr, stop_option, dom_option, epsilon);
		else
		  rtree_pruning(P, C_idx, ext_vec, rr, stop_option, dom_option, epsilon);
	
	}

	
	double max_value = 0;
	for(int i = 0; i < P->numberOfPoints; i++)
	  {
	    double value = dot_prod(u, P->points[i]);
	    if(value > max_value)
		max_value = value;
	  }


	/*
	int inI = 0;
	double alpha = 0.0;
	for(int i = 0; i < C_idx.size(); i++)
	  {
	    double value = dot_prod(u, P->points[C_idx[i]]);
	    if(value * (1 + epsilon) > max_value)
	      inI++;

	    if (max_value - value * (1 + epsilon) > alpha)
	      alpha = max_value - value * (1 + epsilon);

	  }
	printf("Found %d in I and %d false positives and alpha was %lf.\n", inI, C_idx.size() - inI, alpha);
	*/

	string algorithm_name;
	if (cmp_option == RANDOM){
		algorithm_name = "UH_Random";
	}else{
		algorithm_name = "Min_D";
	}

	// Effort level input
	double effort;
	 cout << "\n\nEnter effort level you have put in for this round from 1 (Easy) to 5 (Difficult): ";

	// Loop until a valid integer between 1 and 5 is entered
	while (true) {
		 cin >> effort;

		// Check if the input is a valid integer in the range [1, 5]
		if ( cin.fail() || effort < 1 || effort > 5 || effort != static_cast<int>(effort)) {
			 cin.clear(); // Clear the error flag
			 cin.ignore( numeric_limits< streamsize>::max(), '\n'); // Ignore the invalid input
			 cout << "Invalid input. Please enter an integer between 1 and 5: ";
		} else {
			break; // Exit the loop if the input is valid
		}
	}

	// Create and write to the effort file
	 string filename = "user_" +  to_string(user_id) + "_" + algorithm_name + "_effort.txt";
	 ofstream effort_file(filename);
	if (effort_file.is_open()) {
		effort_file << effort << "\n";
		effort_file.close();
	} else {
		 cerr << "Unable to open file for writing effort data.\n";
	}

	// Close effort file
	effort_file.close();

 	// Open a file to save the final output
    std::string filename1 = "user_" + std::to_string(user_id) + "_" + algorithm_name + "_final_output_set.txt";
    std::ofstream output_file(filename1);
    if (!output_file.is_open()) {
        std::cerr << "Unable to open file to record final output set\n";
    }

    // Print and write the final output set
    std::cout << "FINAL OUTPUT SET:" << std::endl;
    output_file << "FINAL OUTPUT SET:\n";
    
    for (int i = 0; i < C_idx.size(); i++) {
        point_t* point = P->points[C_idx[i]]; // Access the actual point from P->points using C_idx

        // Print and write the point details
        std::cout << "Point " << i + 1 << " (Index: " << C_idx[i] << "):" << std::endl;
        output_file << "Point " << i + 1 << " (Index: " << C_idx[i] << "):\n";

        std::cout << "Attributes: " << std::endl;
        output_file << "Attributes:\n";
        
        // Print and write each attribute with its label
        std::cout << "1. RAM (GB): " << point->coord[0] << std::endl;
        output_file << "1. RAM (GB): " << point->coord[0] << "\n";
        
        std::cout << "2. SSD (GB): " << point->coord[1] << std::endl;
        output_file << "2. SSD (GB): " << point->coord[1] << "\n";
        
        std::cout << "3. HDD (GB): " << point->coord[2] << std::endl;
        output_file << "3. HDD (GB): " << point->coord[2] << "\n";
        
        std::cout << "4. Warranty: " << point->coord[3] << std::endl;
        output_file << "4. Warranty: " << point->coord[3] << "\n";
        
        std::cout << "5. Star Ratings: " << point->coord[4] << std::endl;
        output_file << "5. Star Ratings: " << point->coord[4] << "\n";
        
        std::cout << "6. Number of Ratings: " << point->coord[5] << std::endl;
        output_file << "6. Number of Ratings: " << point->coord[5] << "\n";
        
        std::cout << "7. Number of Reviews: " << point->coord[6] << std::endl;
        output_file << "7. Number of Reviews: " << point->coord[6] << "\n";
        
        std::cout << "---------------------------------" << std::endl; // Separator for each point
        output_file << "---------------------------------\n";
    }

    // Close the file after writing
    output_file.close();


	int inI = 0;
	double alpha = 0.0;
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

	    if (max_value - value * (1 + epsilon) > alpha)
	      alpha = max_value - value * (1 + epsilon);
	  }
	}
	if (C_idx.size() - inI > 0)
	  avg_effective_epsilon /= C_idx.size() - inI;
	
	if (DEBUG)
		printf("Found %d in I; %d false positives; alpha was %lf; avg effective epsilon was %lf.; max effective epsilon was %lf\n", inI, C_idx.size() - inI, alpha, avg_effective_epsilon, max_effective_epsilon);

	// get the final result 
	point_t* result = P->points[get_current_best_pt(P, C_idx, ext_vec)];
	Csize = C_idx.size();

	for (int i = 0; i < ext_vec.size(); i++)
	  release_point(ext_vec[i]);

	return alpha;
}

// //===========================================================================
// // display options to the user, ask them to choose one, and record the time taken
// //===========================================================================
// int get_user_choice(vector<point_t *> S_real) {

//     // Display the options to the user
//     cout << "Please choose between the following options:\n";

//     // Print details of each point in S_real
//     for (int i = 0; i < S_real.size(); i++) {
//         cout << "Point " << i + 1 << ": ";  // Display point number (1-based index)
        
//         for (int j = 0; j < S_real[i]->dim; j++) {
//             // Print each coordinate of the point
//             // Optionally label each attribute if needed, for example:
//             //cout << "Attribute " << j + 1 << ": " << S_real[i]->coord[j] << " ";
// 			cout << S_real[i]->coord[j] << " ";

//         }
//         cout << "\n";  // Move to the next line after printing all coordinates of the point
//     }

//     // Start timing
//     auto start =  chrono::high_resolution_clock::now();

//     // Prompt user for a choice
//     int choice = -1;
//     while (choice < 1 || choice > S_real.size()) {
//         cout << "Enter the number of the point you prefer (1-" << S_real.size() << "): ";
//         cin >> choice;

//         // Ensure that the input is valid
//         if (choice < 1 || choice > S_real.size()) {
//             cout << "Invalid choice. Please choose a valid option.\n";
//         }
//     }

//     // End timing
//     auto end =  chrono::high_resolution_clock::now();

//     // Calculate the duration in seconds
//      chrono::duration<double> duration = end - start;
//     double time_taken = duration.count();  // Store the time taken for the user to make the choice
//     cout << "Time it took: " << time_taken << " seconds\n\n";  

//     return choice - 1;  // Return 0-based index (adjusted from the 1-based user input)
// }


// //===========================================================================
// // Ask for user's input on which option they prefer for breakpoint
// //===========================================================================
// int get_user_choice_breakpoint(point_t** points_to_display, int a, int i) {

//     // Display the two options to the user
//     cout << "Please choose between the following two options:\n";
    
//     // Print details of Option 1
//     cout << "Option 1: ";
//     cout << "Attribute 1" << ": " << points_to_display[0]->coord[a] << " ";
// 	cout << "Attribute 2" << ": " << points_to_display[0]->coord[i] << " ";

//     cout << "\n";
    
//     // Print details of Option 2
//     cout << "Option 2: ";
//     cout << "Attribute 1" << ": " << points_to_display[1]->coord[a] << " ";
// 	cout << "Attribute 2" << ": " << points_to_display[1]->coord[i] << " ";

//     cout << "\n";

//     auto start =  chrono::high_resolution_clock::now();

//     // Prompt user for a choice
//     int choice = -1;
//     while (choice != 1 && choice != 2) {
//         cout << "Enter 1 for Option 1 or 2 for Option 2: ";
//         cin >> choice;

//         // Validate input
//         if (choice != 1 && choice != 2) {
//             cout << "Invalid choice. Please choose either 1 or 2.\n";
//         }
//     }

// 	    // End timing
//     auto end =  chrono::high_resolution_clock::now();

//     // Calculate the duration in seconds
//      chrono::duration<double> duration = end - start;
//     double time_taken = duration.count();  // Store the time taken for the user to make the choice
//     cout << "Time it took: " << time_taken << " seconds\n\n";  

//     return choice - 1;  // Return 0-based index (adjusted from the 1-based user input)
// }
int get_user_choice(vector<point_t *> S_real, point_t** points_to_display, int a, int i, int user_id, int cmp_option) {
    
     string filename;

    // Attribute names corresponding to indices
     map<int,  string> attribute_names;
    attribute_names[0] = "RAM (GB)";
    attribute_names[1] = "SSD (GB)";
    attribute_names[2] = "HDD (GB)";
    attribute_names[3] = "Warranty";
    attribute_names[4] = "Star Ratings";
    attribute_names[5] = "Number of Ratings";
    attribute_names[6] = "Number of Reviews";
    
    // Start timing
    auto start =  chrono::high_resolution_clock::now();

    // Determine the file prefix based on comparison option
    filename = (!S_real.empty()) ? ((cmp_option == RANDOM) ? "UH_Random" : "Min_D") : "Breakpoint";

    // Create a log file to record the algorithm option and user choices
     string log_filename = "user_" +  to_string(user_id) + "_" + filename + "_log.txt";
     ofstream log_file(log_filename,  ios::app);
    if (!log_file.is_open()) {
         cerr << "Unable to open log file to record choices\n";
        return -1;
    }
    log_file << "Comparison Option (cmp_option): " << cmp_option << "\n\n";

    // Create a separate time log file for recording the time taken for each choice
     string time_log_filename = "user_" +  to_string(user_id) + "_" + filename + "_time_log.txt";
     ofstream time_log_file(time_log_filename,  ios::app);
    if (!time_log_file.is_open()) {
         cerr << "Unable to open time log file\n";
        return -1;
    }

    if (!S_real.empty()) {
         cout << "Please choose between the following options:\n\n";

        for (int idx = 0; idx < S_real.size(); idx++) {
             cout << "Option " <<  setw(2) << idx + 1 << ": \n";
            log_file << "Option " << idx + 1 << ":\n";
            for (int j = 0; j < S_real[idx]->dim; j++) {
                 string attr_name = (attribute_names.count(j) ? attribute_names[j] : "Attribute " +  to_string(j + 1));
                 cout << "  " <<  setw(20) << attr_name 
                          << ": " <<  setw(10) << S_real[idx]->coord[j] << " \n";
                log_file << "  " <<  setw(20) << attr_name 
                         << ": " <<  setw(10) << S_real[idx]->coord[j] << "\n";
            }
             cout << "\n";
            log_file << "\n";
        }

        int choice = -1;
        while (choice < 1 || choice > S_real.size()) {
             cout << "\nEnter the number of the point you prefer (1-" << S_real.size() << "): ";
             cin >> choice;

            if ( cin.fail()) {
                 cin.clear();
                 cin.ignore( numeric_limits< streamsize>::max(), '\n');
                 cout << "Invalid input. Please enter a number between 1 and " << S_real.size() << ".\n";
                choice = -1;
            } else if (choice < 1 || choice > S_real.size()) {
                 cout << "Invalid choice. Please enter a valid option between 1 and " << S_real.size() << ".\n";
            }
        }

        auto end =  chrono::high_resolution_clock::now();
         chrono::duration<double> duration = end - start;
        double time_taken = duration.count();

        log_file << "User selected Point " << choice << "\n";
        log_file.close();

        // Log the time taken in the time log file
        time_log_file << time_taken << "\n";
        time_log_file.close();

        return choice - 1;
    } 
    else if (points_to_display != nullptr) {
        
         cout << "Please choose between the following two options:\n\n";

        for (int option = 0; option < 2; ++option) {
             cout << "Option " << option + 1 << ":\n";
            log_file << "Option " << option + 1 << ":\n";
            if (attribute_names.find(a) != attribute_names.end()) {
                cout << "  " <<  setw(20) << attribute_names[a] << ": " <<  setw(10) << points_to_display[option]->coord[a] << "\n";
                log_file << "  " <<  setw(20) << attribute_names[a] << ": " <<  setw(10) << points_to_display[option]->coord[a] << "\n";
            } else {
                 cout << "  " <<  setw(20) << "Attribute " +  to_string(a + 1) << ": " <<  setw(10) << points_to_display[option]->coord[a] << "\n";
                log_file << "  " <<  setw(20) << "Attribute " +  to_string(a + 1) << ": " <<  setw(10) << points_to_display[option]->coord[a] << "\n";
            }
            if (attribute_names.find(i) != attribute_names.end()) {
                cout << "  " <<  setw(20) << attribute_names[i] << ": " <<  setw(10) << points_to_display[option]->coord[i] << "\n";
                log_file << "  " <<  setw(20) << attribute_names[i] << ": " <<  setw(10) << points_to_display[option]->coord[i] << "\n";
            } else {
                 cout << "  " <<  setw(20) << "Attribute " +  to_string(i + 1) << ": " <<  setw(10) << points_to_display[option]->coord[i] << "\n";
                log_file << "  " <<  setw(20) << "Attribute " +  to_string(i + 1) << ": " <<  setw(10) << points_to_display[option]->coord[i] << "\n";
            }
             cout << "\n";
            log_file << "\n";
        }

        int choice = -1;
        while (choice != 1 && choice != 2) {
             cout << "Enter 1 for Option 1 or 2 for Option 2: ";
             cin >> choice;

            if ( cin.fail()) {
                 cin.clear();
                 cin.ignore( numeric_limits< streamsize>::max(), '\n');
                 cout << "Invalid input. Please enter either 1 or 2.\n";
                choice = -1;
            } else if (choice != 1 && choice != 2) {
                  cout << "Invalid choice. Please choose either 1 or 2.\n";
            }
        }

        auto end =  chrono::high_resolution_clock::now();
         chrono::duration<double> duration = end - start;
        double time_taken = duration.count();

        log_file << "User selected Option " << choice << "\n";
        log_file.close();

        // Log the time taken in the time log file
        time_log_file << time_taken << "\n";
        time_log_file.close();

        return choice - 1;
    } else {
         cout << "Error: No valid input to display.\n";
        return -1;
    }
}


// the main interactive algorithm for fake points
// P: the input dataset (assumed (1+epsilon)-skyline)
// u: the unknown utility vector
// s: the question size
// epsilon: the indistinguishability threshold
// maxRound: the maximum number of rounds of interacitons
// Qcount: the number of question asked
// Csize: the size the candidate set when finished
double max_utility_fake(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize)
{
  int dim = P->points[0]->dim;

  // the indexes of the candidate set
  // initially, it is all the skyline cars
  vector<int> C_idx;
  for(int i = 0; i < P->numberOfPoints; i++)
    C_idx.push_back(i);  
  
  // if we have fewer than (d-1)/(s-1) rounds we don't have enough questions to discover i^*,
  // so we just return the current set
  if (maxRound <= (dim - 1)/(s - 1))
  {
    double max_value = 0;
    for(int i = 0; i < P->numberOfPoints; i++)
    {
      double value = dot_prod(u, P->points[i]);
      if(value > max_value)
	max_value = value;
    }
    double alpha = 0.0;
    for(int i = 0; i < C_idx.size(); i++)
    {
      double value = dot_prod(u, P->points[C_idx[i]]);
      if (max_value - value * (1 + epsilon) > alpha)
	alpha = max_value - value * (1 + epsilon);
    }
    return alpha;
  }

  if ((dim - 1) % (s - 1) == 0)
    Qcount = (dim - 1) / (s - 1);
  else
    Qcount = (dim - 1) / (s - 1) + 1; // round up

  
  // go through the dimensions (s-1) at a time to find the max u_i,
  // making sure to randomly select an option if there are any points
  // that are delta-indistinguishable from the best so far
  int istar = 0;
  double max_ui = u->coord[0];
  for(int i = 1; i < dim; i += s - 1)
  {
    double best = max_ui;
    for(int j = i; j < dim && j < i + s - 1; ++j)
      if (u->coord[j] >= best)
	best = u->coord[j];

    vector<int> options;
    if (u->coord[istar] * (1 + delta) >= best)    // if the best so far is big enough
      options.push_back(istar);                   // it is a contender
    for(int j = i; j < dim && j < i + s - 1; ++j)
      if (u->coord[j] * (1 + delta) >= best)
	options.push_back(j);
    
    istar = options[rand() % options.size()];
    max_ui = u->coord[istar];
  }
	
  double upperbound = (dim - 1) % (s - 1) == 0 ? pow(1 + delta, (dim - 1)/(s - 1)) : pow(1 + delta, (dim - 1)/(s - 1) + 1);
  vector<double> L(dim, 0.0), H(dim, upperbound);
  L[istar] = 1;
  H[istar] = 1;

  int i = 0;
  while(Qcount < maxRound)
  {
    if (i == istar)
      i = (i + 1) % dim;

    // compute chi values
    vector<double> chi(s + 1);
    for (int j = 0; j < s + 1; ++j)
      chi[j] = L[i] + j * (H[i] - L[i]) / s;

    // find the best of the s points p1,...,ps
    int c = 0;
    double best_val = 0;
    for(int j = 1; j <= s; ++j)
    {
      double sum = 0.0;
      for(int k = j; k <= s - 1; ++k)
	sum += chi[k] / s;

      // sum up the utility for the attributes i and istar as the points are zero elsewhere
      double val = (u->coord[i] * j) / s + u->coord[istar] * sum;
      
      if (val > best_val)
	{
	  c = j;
	  best_val = val;
	}
    }

    // find all indistinguishable options
    vector<int> options;
    for(int j = 1; j <= s; ++j)
    {
      double sum = 0.0;
      for(int k = j; k <= s - 1; ++k)
	sum += chi[k] / s;

      double val = (u->coord[i] * j) / s + u->coord[istar] * sum;
      if ((1 + delta) * val >= best_val)
	options.push_back(j);
    }
      
    // choose an option randomly from indistinguishable options
    c = options[rand() % options.size()];

    double sum = 0.0;
    for(int j = c; j < s; ++j)
      sum += chi[j];

    // update L_i and H_i based on this c
    L[i] = max(L[i], (chi[c - 1] - delta * sum)/(1 + c * delta));     //max(L[i], chi[c - 1] - delta * (2 * s - 1));
    H[i] = min(H[i], (chi[c] + delta * sum)/(1 - c * delta));         //min(H[i], chi[c]     + delta * (2 * s -  1));
    
    i = (i + 1) % dim;
    Qcount += 1;

    //cout << Qcount << endl;
    //for (int j = 0; j < dim; ++j)
    //  printf("%d: %lf <= %lf <= %lf\n", j, L[j], u->coord[j]/max_ui, H[j]);
    //cout << endl;

  }

  // debug at the approximation of u
  //for (int j = 0; j < dim; ++j)
  //  printf("%d: %lf <= %lf <= %lf\n", j, L[j], u->coord[j]/max_ui, H[j]);

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
  double alpha = 0.0;
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

	  if (max_value - value * (1 + epsilon) > alpha)
	    alpha = max_value - value * (1 + epsilon);
	}
    }
  if (C_idx.size() - inI > 0)
    avg_effective_epsilon /= C_idx.size() - inI;

	if (DEBUG) {
		printf("Found %d in I; %d false positives; alpha was %lf; avg effective epsilon was %lf; max effective epsilon was %lf.\n", inI, C_idx.size() - inI, alpha, avg_effective_epsilon, max_effective_epsilon);
	}
  Csize = C_idx.size();

	if (DEBUG2){
		cout << "*******************  SQUEEZE-U   *******************" << endl;
		for (int i = 0; i < dim; i++){
			cout << "Lower bound L[" << i << "] of utility: " << L[i] << endl;
			cout << "Higher bound H[" << i << "] of utility: " << H[i] << endl;
		}
		cout << "******************* END SQUEEZE-U *******************" << endl;
	}

  return alpha;
}

// the random algorithm for fake points
// P: the input dataset (assumed (1+epsilon)-skyline)
// u: the unknown utility vector
// s: the question size
// epsilon: the indistinguishability threshold
// maxRound: the maximum number of rounds of interacitons
// Qcount: the number of question asked
// Csize: the size of the candidate set when finished
double random_fake(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize)
{
  int dim = P->points[0]->dim;
  int prune_option = RTREE;
  int dom_option = HYPER_PLANE;
  int stop_option = EXACT_BOUND;
  int cmp_option = RANDOM;
  int current_best_idx = -1;
  int last_best = -1;
  vector<int> frame;
  vector<point_t*> ext_pts;
  vector<point_t*> hyperplanes;
  hyperplane_t* hp = NULL;

  // the indexes of the candidate set
  // initially, it is all the skyline cars
  vector<int> C_idx;
  for(int i = 0; i < P->numberOfPoints; i++)
    C_idx.push_back(i);

  // the initial exteme vector sets V = {?ei | i ? [1, d], ei [i] = 1 and ei [j] = 0 if i , j}.
  vector<point_t*> ext_vec;
  for (int i = 0; i < dim; i++)
    {
      point_t* e = alloc_point(dim);
      for (int j = 0; j < dim; j++)
	{
	  if (i == j)
	    e->coord[j] = -1;
	  else
	    e->coord[j] = 0;
	}
      ext_vec.push_back(e);
    }

  if(dom_option == HYPER_PLANE)
    ext_pts = get_extreme_pts(ext_vec); // in Hyperplane Pruning, we need the set of extreme points of R
  else
    {
      // in Conical Pruning, we need bounding hyperplanes for the conical hull
      get_hyperplanes(ext_vec, hp, hyperplanes);
      if(stop_option != NO_BOUND) // if an upper bound on the regret ratio is needed, we need the set of extreme points of R
	ext_pts = get_extreme_pts(ext_vec);
    }

  // get the index of the "current best" point
  //if(cmp_option != RANDOM)
  //current_best_idx = get_current_best_pt(P, C_idx, ext_vec);

  double rr = 1;

  for(int round = 0; round < maxRound; ++round)
  {
    // generate s random points
    vector<point_t*> S(s);
    for(int i = 0; i < s; ++i)
      S[i] = rand_point(dim);

    // see what the user picks among delta-indistinguishable options
    int best = 0;
    double best_value = 0.0;
    for(int i = 0; i < s; ++i)
    {
      double value = dot_prod(u, S[i]);
      if (value > best_value)
      {
	best = i;
	best_value = value;
      }
    }
    vector<int> options;    // list of optimally delta-indistinguishable points among the s
    for(int i = 0; i < s; ++i)
    {
      double value = dot_prod(u, S[i]);
      if (value * (1 + delta) >= best_value)
	options.push_back(i);
    }
    int max_i = options[rand() % options.size()];

    // update the extreme vectors based on the user feedback
    update_ext_vec(P, S, C_idx, max_i, s, ext_vec, current_best_idx, last_best, frame, cmp_option, delta);

    Qcount = round;

    if(C_idx.size()==1 ) // || global_best_idx == current_best_idx
      break;

    //update candidate set
    if(prune_option == SQL)
      sql_pruning(P, C_idx, ext_vec, rr, stop_option, dom_option, epsilon);
    else
      rtree_pruning(P, C_idx, ext_vec, rr, stop_option, dom_option, epsilon);

    // release memory for random points
    for(int i = 0; i < s; ++i)
      release_point(S[i]);
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
  double alpha = 0.0;
  double avg_effective_epsilon = 0.0, max_effective_epsilon = 0.0;
  for(int i = 0; i < C_idx.size(); i++)
    {
      double value = dot_prod(u, P->points[C_idx[i]]);
      if(value * (1 + epsilon) > max_value)
	inI++;
      else
	{
	  if (max_value/value - 1.0 > max_effective_epsilon)
            max_effective_epsilon = max_value/value - 1.0;

	  avg_effective_epsilon += max_value/value - 1.0;

	  if (max_value - value * (1 + epsilon) > alpha)
	    alpha = max_value - value * (1 + epsilon);
	}
    }
  if (C_idx.size() - inI > 0)
    avg_effective_epsilon /= C_idx.size() - inI;
  printf("Found %d in I; %d false positives; alpha was %lf; avg effective epsilon was %lf; max effective epsilon was %lf.\n", inI, C_idx.size() - inI, alpha, avg_effective_epsilon, max_effective_epsilon);
  
  Csize = C_idx.size();

  for(int i = 0; i < ext_pts.size(); i++)
    release_point(ext_pts[i]);
  
  for(int i = 0; i < ext_vec.size(); i++)
    release_point(ext_vec[i]);

  return alpha;
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
double max_utility_breakpoint(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize, int user_id) {	
	// Set up 
	vector<point_t*> empty;
	Qcount = 0;
    int dim = P->points[0]->dim;

	// Anchor dimension
	int a = 0;		

	// Initialize L and H slope bounds
	vector<double> L(dim, -INF), H(dim, 0);

	// Feature: Correlation check
	int highest_min_breakpoint = -INF;

	for (int i = 0; i < dim; i++) {
		int min_breakpoint = INF;
		for (int j = 0; j < dim; j++) {
			if (i == j) {
				continue;
			}
			int num_breakpoint = count_slopes(P, -INF, 0, true, i, j);
			if (num_breakpoint < min_breakpoint) {
				min_breakpoint = num_breakpoint;
			}
		if (DEBUG) {
			cout << "Min breakpoint for dimension " << i << " with dimension " << j << " is " << min_breakpoint << endl;
		}
		}
		if (min_breakpoint > highest_min_breakpoint) {
			highest_min_breakpoint = min_breakpoint;

			// Set anchor dimension to current dimension
			a = i;
		}
	}
	if (DEBUG) {
		cout << "Anchor dimension is " << a  << " because it has the highest number of breakpoints of " 
			<< highest_min_breakpoint << endl;
	}

	// Anchor dimension
	L[a] = -1;
	H[a] = -1;

	int i = 0;
    while(Qcount < maxRound) {
		if (i == a) {
			i = (i + 1) % dim;
		}

		SLOPE_TYPE max_diff = -INF;

		// Feature: prioritise dimension
		for (int j = 0; j < dim; j++) {
			SLOPE_TYPE diff = abs(slope_to_ratio(H[j]) - slope_to_ratio(L[j]));
			if (diff >= max_diff) {
				max_diff = diff;
				i = j;
			}
			if (DEBUG) { 
				cout << "Max difference is " << max_diff << endl;
				cout << "Difference is " << diff << endl;
			}
		}

		if (DEBUG) {cout << "This is breakpoint round for dimension " << i << endl; }
        point_t** points_to_display = breakpoint_one_round(P, s, L[i], H[i], a, i);

		// Skip iteration if random set not found
		if (points_to_display == nullptr) {
			Qcount++;
			i = (i + 1) % dim;
			continue;
		}

		// if (DEBUG) { cout << "User utility: (" << u->coord[a] << ", " << u->coord[i] << ")" << endl; }

		// // Check if we can access points_to_display as wanted
		// if (DEBUG) {
		// 	for (int j = 0; j < s; j++) {
		// 		cout << "Point to display is (" << points_to_display[j]->coord[a] << ", " 
		// 										<< points_to_display[j]->coord[i] << ")" 
		// 			<< " with utility " << u->coord[a] * points_to_display[j]->coord[a] + u->coord[i] * points_to_display[j]->coord[i] 
		// 			<< endl;
		// 	}
		// }

		// Simulate user interaction, user picks their favorite 
		// double max_value = 0;
		// int best_index = 0;
		// for (int j = 0; j < s; j++) {
		// 	double value = u->coord[a] * points_to_display[j]->coord[a] + u->coord[i] * points_to_display[j]->coord[i];
		// 	if (value > max_value) {
		// 		max_value = value;
		// 		best_index = j;
		// 	}
		// }

		// cout << "point1 to display from the algorithm --->> Attribute 1: " << points_to_display[0]->coord[a]
		// 	<< " Attribute 2: " << points_to_display[0]->coord[i] << endl; 
		// cout << "point2 to display from the algorithm --->> Attribute 1: " << points_to_display[1]->coord[a]
		// 	<< " Attribute 2: " << points_to_display[1]->coord[i] << endl; 

		int best_index = get_user_choice(empty, points_to_display, a, i, user_id, 100);

		// if (DEBUG) { cout << "Best point found correctly at index " << best_index << endl; }

		SLOPE_TYPE 		X[] = {L[i], compute_slope(points_to_display[0], points_to_display[1], a, i), H[i]};
		
		// Update L[i] and H[i]
		L[i] = X[best_index];
		H[i] = X[best_index + 1];

		// if (DEBUG) {
		// 	for (int i = 0; i < dim; i++) {
		// 		cout << "Real slope for dimension " << i << " is: " << ratio_to_slope(u->coord[i]/u->coord[a]) << endl;
		// 		cout << "L[" << i << "] = " << L[i] << endl;
		// 		cout << "H[" << i << "] = " << H[i] << endl;
		// 	}
		// }

		// Finish one round
		Qcount++;
		i = (i + 1) % dim;
	}

	// Convert from slope to ratio
	for (int i = 0; i < dim; i++) {
		L[i] = slope_to_ratio(L[i]);
		// Prevent negative infinity H[i] bound
		if (H[i] == 0) { H[i] = INF; }
		else { H[i] = slope_to_ratio(H[i]); }
	}

	// debug block
	if (DEBUG) {
		cout << "Last debug print block" << endl;
		for (int i = 0; i < dim; i++) {
			cout << "Real ratio for dimension " << i << " is: " << u->coord[i]/u->coord[a] << endl;
			cout << "L[" << i << "] = " << L[i] << endl;
			cout << "H[" << i << "] = " << H[i] << endl;
		}
	}

    // Find the highest value from the low-end of the user utilities
    double highest = 0.0;
    for (int j = 0; j < P->numberOfPoints; ++j)
    {
        double dot = 0.0;
        for(int k = 0; k < dim; ++k)
        	dot += P->points[j]->coord[k] * L[k];
        if (dot > highest)
        	highest = dot;
    }

    // Prune all the points that have their high-end utility (1+epsilon) dominated
	vector<int> C_idx;
    C_idx.clear();
    for (int j = 0; j < P->numberOfPoints; ++j)
    {
        double dot = 0.0;
        for(int k = 0; k < dim; ++k)
        	dot += P->points[j]->coord[k] * H[k];
        if (dot * (1 + epsilon) >= highest)
        	C_idx.push_back(j);
    }

	// Effort level input
	double effort;
	 cout << "\n\nEnter effort level you have put in for this round from 1 (Easy) to 5 (Difficult): ";

	// Loop until a valid integer between 1 and 5 is entered
	while (true) {
		 cin >> effort;

		// Check if the input is a valid integer in the range [1, 5]
		if ( cin.fail() || effort < 1 || effort > 5 || effort != static_cast<int>(effort)) {
			 cin.clear(); // Clear the error flag
			 cin.ignore( numeric_limits< streamsize>::max(), '\n'); // Ignore the invalid input
			 cout << "Invalid input. Please enter an integer between 1 and 5: ";
		} else {
			break; // Exit the loop if the input is valid
		}
	}

	// Create and write to the effort file
	 string filename = "user_" +  to_string(user_id) + "_Breakpoint" + "_effort.txt";
	 ofstream effort_file(filename);
	if (effort_file.is_open()) {
		effort_file << effort << "\n";
		effort_file.close();
	} else {
		 cerr << "Unable to open file for writing effort data.\n";
	}

	// Close effort file
	effort_file.close();

 // Open a file to save the final output
    std::string filename1 = "user_" + std::to_string(user_id) + "_Breakpoint" + "_final_output_set.txt";
    std::ofstream output_file(filename1);
    if (!output_file.is_open()) {
        std::cerr << "Unable to open file to record final output set\n";
    }

    // Print and write the final output set
    std::cout << "FINAL OUTPUT SET:" << std::endl;
    output_file << "FINAL OUTPUT SET:\n";
    
    for (int i = 0; i < C_idx.size(); i++) {
        point_t* point = P->points[C_idx[i]]; // Access the actual point from P->points using C_idx

        // Print and write the point details
        std::cout << "Point " << i + 1 << " (Index: " << C_idx[i] << "):" << std::endl;
        output_file << "Point " << i + 1 << " (Index: " << C_idx[i] << "):\n";

        std::cout << "Attributes: " << std::endl;
        output_file << "Attributes:\n";
        
        // Print and write each attribute with its label
        std::cout << "1. RAM (GB): " << point->coord[0] << std::endl;
        output_file << "1. RAM (GB): " << point->coord[0] << "\n";
        
        std::cout << "2. SSD (GB): " << point->coord[1] << std::endl;
        output_file << "2. SSD (GB): " << point->coord[1] << "\n";
        
        std::cout << "3. HDD (GB): " << point->coord[2] << std::endl;
        output_file << "3. HDD (GB): " << point->coord[2] << "\n";
        
        std::cout << "4. Warranty: " << point->coord[3] << std::endl;
        output_file << "4. Warranty: " << point->coord[3] << "\n";
        
        std::cout << "5. Star Ratings: " << point->coord[4] << std::endl;
        output_file << "5. Star Ratings: " << point->coord[4] << "\n";
        
        std::cout << "6. Number of Ratings: " << point->coord[5] << std::endl;
        output_file << "6. Number of Ratings: " << point->coord[5] << "\n";
        
        std::cout << "7. Number of Reviews: " << point->coord[6] << std::endl;
        output_file << "7. Number of Reviews: " << point->coord[6] << "\n";
        
        std::cout << "---------------------------------" << std::endl; // Separator for each point
        output_file << "---------------------------------\n";
    }

    // Close the file after writing
    output_file.close();


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
	if (DEBUG)
    printf("Breakpoint - Found %d in I; %d false positives; alpha was %lf; avg effective epsilon was %lf; max effective epsilon was %lf.\n", inI, C_idx.size() - inI, alpha_approx, avg_effective_epsilon, max_effective_epsilon);
    Csize = C_idx.size();

    return alpha_approx;
}

//==============================================================================================
// Copy of max_utility_breakpoint_for_vary_T
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
double max_utility_breakpoint(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize, int repeats, int user_id) {
    // Set up 
	Qcount = 0;
    int dim = P->points[0]->dim;

	// Anchor dimension
	int a = 0;		

	// Initialize L and H slope bounds
	vector<double> L(dim, -INF), H(dim, 0);

	// Feature: Correlation check
	int highest_min_breakpoint = -INF;

	for (int i = 0; i < dim; i++) {
		int min_breakpoint = INF;
		for (int j = 0; j < dim; j++) {
			if (i == j) {
				continue;
			}
			int num_breakpoint = count_slopes(P, -INF, 0, true, i, j);
			if (num_breakpoint < min_breakpoint) {
				min_breakpoint = num_breakpoint;
			}
		if (DEBUG) {
			cout << "Min breakpoint for dimension " << i << " with dimension " << j << " is " << min_breakpoint << endl;
		}
		}
		if (min_breakpoint > highest_min_breakpoint) {
			highest_min_breakpoint = min_breakpoint;

			// Set anchor dimension to current dimension
			a = i;
		}
	}
	if (DEBUG) {
		cout << "Anchor dimension is " << a  << " because it has the highest number of breakpoints of " 
			<< highest_min_breakpoint << endl;
	}

	// Anchor dimension
	L[a] = -1;
	H[a] = -1;

	int i = 0;
    while(Qcount < maxRound) {
		if (i == a) {
			i = (i + 1) % dim;
		}

		SLOPE_TYPE max_diff = -INF;

		// Feature: prioritise dimension
		for (int j = 0; j < dim; j++) {
			SLOPE_TYPE diff = abs(slope_to_ratio(H[j]) - slope_to_ratio(L[j]));
			if (diff >= max_diff) {
				max_diff = diff;
				i = j;
			}
			if (DEBUG) { 
				cout << "Max difference is " << max_diff << endl;
				cout << "Difference is " << diff << endl;
			}
		}

		if (DEBUG) {cout << "This is breakpoint round for dimension " << i << endl; }
        point_t** points_to_display = breakpoint_one_round(P, s, L[i], H[i], a, i, repeats);

		// Skip iteration if random set not found
		if (points_to_display == nullptr) {
			Qcount++;
			i = (i + 1) % dim;
			continue;
		}

		if (DEBUG) { cout << "User utility: (" << u->coord[a] << ", " << u->coord[i] << ")" << endl; }

		// Check if we can access points_to_display as wanted
		if (DEBUG) {
			for (int j = 0; j < s; j++) {
				cout << "Point to display is (" << points_to_display[j]->coord[a] << ", " 
												<< points_to_display[j]->coord[i] << ")" 
					<< " with utility " << u->coord[a] * points_to_display[j]->coord[a] + u->coord[i] * points_to_display[j]->coord[i] 
					<< endl;
			}
		}

		// Simulate user interaction, user picks their favorite 
		double max_value = 0;
		int best_index = 0;
		for (int j = 0; j < s; j++) {
			double value = u->coord[a] * points_to_display[j]->coord[a] + u->coord[i] * points_to_display[j]->coord[i];
			if (value > max_value) {
				max_value = value;
				best_index = j;
			}
		}

		if (DEBUG) { cout << "Best point found correctly at index " << best_index << endl; }

		SLOPE_TYPE 		X[] = {L[i], compute_slope(points_to_display[0], points_to_display[1], a, i), H[i]};
		
		// Update L[i] and H[i]
		L[i] = X[best_index];
		H[i] = X[best_index + 1];

		if (DEBUG) {
			for (int i = 0; i < dim; i++) {
				cout << "Real slope for dimension " << i << " is: " << ratio_to_slope(u->coord[i]/u->coord[a]) << endl;
				cout << "L[" << i << "] = " << L[i] << endl;
				cout << "H[" << i << "] = " << H[i] << endl;
			}
		}

		// Finish one round
		Qcount++;
		i = (i + 1) % dim;
	}

	// Convert from slope to ratio
	for (int i = 0; i < dim; i++) {
		L[i] = slope_to_ratio(L[i]);
		// Prevent negative infinity H[i] bound
		if (H[i] == 0) { H[i] = INF; }
		else { H[i] = slope_to_ratio(H[i]); }
	}

	// Debug block
	if (DEBUG) {
		cout << "Last debug print block" << endl;
		for (int i = 0; i < dim; i++) {
			cout << "Real ratio for dimension " << i << " is: " << u->coord[i]/u->coord[a] << endl;
			cout << "L[" << i << "] = " << L[i] << endl;
			cout << "H[" << i << "] = " << H[i] << endl;
		}
	}

    // Find the highest value from the low-end of the user utilities
    double highest = 0.0;
    for (int j = 0; j < P->numberOfPoints; ++j)
    {
        double dot = 0.0;
        for(int k = 0; k < dim; ++k)
        	dot += P->points[j]->coord[k] * L[k];
        if (dot > highest)
        	highest = dot;
    }

    // Prune all the points that have their high-end utility (1+epsilon) dominated
	vector<int> C_idx;
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
	if (DEBUG)
    	printf("Breakpoint - Found %d in I; %d false positives; alpha was %lf; avg effective epsilon was %lf; max effective epsilon was %lf.\n", inI, C_idx.size() - inI, alpha_approx, avg_effective_epsilon, max_effective_epsilon);
    Csize = C_idx.size();

    return alpha_approx;
}


//==============================================================================================
// max_utility_TT
// Description:
//      Simulate interaction with multiple rounds of interaction and 2-dimensional tuples
//      n - dimensions and s = 2
// Parameters: 
//      P           - input data set
//      u           - unknown utility vector
//      s           - number of points to display each round (s = 2 in this case)
//      maxRound    - maximum number of rounds of interactions / budget of questions
// Return:
//      alpha       - approximation
//==============================================================================================
double max_utility_TT(point_set_t* P, point_t* u, int s,  double epsilon, double delta, int maxRound, int &Qcount, int &Csize) {	
	// number of dimensions
	Qcount = 0;
	int dim = P->points[0]->dim;

	double min_dim_values[dim]; 
	point_t* max_dim_points[dim];							
	double current_min;
	double current_max;

	point_t* max_point= new point_t;
	max_point -> dim = dim;
	max_point -> coord = new double[dim];

	// keep track of minimum val. for each dimension, line 1
	for (int i = 0; i < dim; i++){							// for all dimensions
		current_min = INF;
		current_max = -INF;
		for (int j = 0; j < P -> numberOfPoints; j++){ 		// for all points
			if (P -> points[j] -> coord[i] < current_min){
				current_min = P -> points[j] -> coord[i];
			}
			if (P -> points[j] -> coord[i] > current_max){
				max_dim_points[i] = P -> points[j];					// save a point with max dim val
				current_max = P -> points[j] -> coord[i];
			}
		min_dim_values[i] = current_min;
		}
	}

	if (DEBUG1){
		for (int i=0; i<dim ;i++){
			cout << "Point " << i << " in max dim points: (";
			for (int j=0; j<dim ;j++){
				cout << max_dim_points[i] -> coord[j] << ",";
			}
			cout << ")" << endl;
		}
	}

	int i_star = 0; // line 2
	int i = 1;

	if (DEBUG1){
		for (int i=0; i<dim ; i++){
			cout << "Min value for dim" << i << ": " << min_dim_values[i] << endl;
		}
	}
	
	double a;
	point_t* q= new point_t;
	q -> dim = dim;
    q -> coord = new double[dim];

	// Simulate user interaction, user picks their preference to find i*
	while (i < dim){
		// line 4: find point p
		q -> coord[i_star] = (max_dim_points[i_star]->coord[i_star]+ max_dim_points[i]->coord[i_star])/2;
		q -> coord[i] = (max_dim_points[i_star]->coord[i]+ max_dim_points[i]->coord[i])/2;

		if (DEBUG1){
			cout << "Max point for dim " << i_star << ": (" << max_dim_points[i_star]->coord[i_star] << "," << max_dim_points[i_star]->coord[i] << ")" << endl;
			cout << "Max point for dim " << i << ": (" << max_dim_points[i]->coord[i_star] << "," << max_dim_points[i]->coord[i] << ")" << endl;
		}

		// line 5: find a
		if (q -> coord[i_star] - min_dim_values[i_star] < q -> coord[i] - min_dim_values[i]){
			a = q -> coord[i_star] - min_dim_values[i_star];
		} else {
			a = q -> coord[i] - min_dim_values[i];
		}

		// line 5: construct points to display to user to find i*, user will pick point with higher utility 'score'
		double pt_i_utility = (u->coord[i_star] * min_dim_values[i_star])+ (u->coord[i] * q->coord[i]);
		double pt_i_star_utility = (u->coord[i_star] * (min_dim_values[i_star] + a)) + (u->coord[i] * (q->coord[i] - a));

		if(DEBUG1){
		cout << "a: " << a << endl;
		cout << "p1: (" << q -> coord[i_star] << "," << q -> coord[i] << ")" << endl;
		cout << "p2: (" <<  min_dim_values[i_star] + a << "," << q -> coord[i] - a << ")" << endl;
		cout << "Utility of pt1_utility: " << pt_i_utility << endl;
		cout << "Utility of pt2_utility: " << pt_i_star_utility << endl;
		}

		// line 6: update i* dimension accordingly
		if (pt_i_utility > pt_i_star_utility){
			i_star = i;

			if(DEBUG1){
				cout << "I* updated, Utility of i_star: " << pt_i_star_utility << ", I* dimension is: " << i_star << endl;
			}
		}
		else{
			if (DEBUG1){
				cout << "I* not updated, Utility of i_star: " << pt_i_utility << ", I* dimension is: " << i_star << endl;
			}
		}
		
		// line 7
		i = i + 1;
		Qcount = Qcount+1;
	}

	// line 8: for each dim, initialize L_i and H_i, set i* bounds to 1
	vector<double> L(dim, 0), H(dim, 1);
	L[i_star] = 1;
	H[i_star] = 1;

	double b;					// desired ratio breakpoint
	double b_hat;				// desired slope breakpoint
	double width;
	double height;
	point* p= new point_t; // q in psuedocode, maintaining p to match prev code and pseudocode
	p -> dim = dim;
	p -> coord = new double[dim];
	point_t* p_new= new point_t;
	p_new -> dim = dim;
    p_new -> coord = new double[dim];

	i = 0;						// line 10
	while (Qcount < maxRound){  // line 11: user interaction, cycling through dims
		if (i == i_star){		// line 12
			i = (i + 1) % dim;
		}
		// line 9: construct point p ("q_i" in pseudocode)
		//p -> coord[i_star] = (max_dim_points[i_star]->coord[i_star]+ max_dim_points[i]->coord[i_star])/2;

		q -> coord[i_star] = (max_dim_points[i_star]->coord[i_star]+ max_dim_points[i]->coord[i_star])/2;
		q -> coord[i] = (max_dim_points[i_star]->coord[i]+ max_dim_points[i]->coord[i])/2;

		p -> coord[i_star] = (min_dim_values[i_star]);
		p -> coord[i] = q -> coord[i];



		if (DEBUG1){
			cout << "***************** New while iteration, i= " << i << ", Qcount= " << Qcount << " ********************" << endl;
		}
		
		if (DEBUG1){
			cout << "Beginning Lower bound L[" << i << "] of utility: " << L[i] << endl;
			cout << "Beginning Higher bound H[" << i << "] of utility: " << H[i] << endl;
		}

		b = L[i] + ((H[i] - L[i])/2); // line 14: find ratio breakpoint
		b_hat= (-1/b);				  // line 15: find slope breakpoint

		if (DEBUG1){
			cout << "ratio breakpoint b: " << b << endl;
			cout << "slope breakpoint b_hat: " << b_hat << endl;
		}

		width = q -> coord[i_star] - min_dim_values[i_star]; // line 16: find TT field width
		if (DEBUG1){
			cout << "Width: " << width << endl;
		}
		height = q -> coord[i] - min_dim_values[i];			// line 16: find TT field height
		if (DEBUG1){
			cout << "Height: " << height << endl;
		}

		// line 17: check if point intersects with horizonal line of TT field
		if (q -> coord[i] + (width * b_hat) < min_dim_values[i]){
			p_new -> coord[i_star] = (min_dim_values[i_star] - (height/b_hat)); // line 18 mini_star -(height/b_hat)
			p_new -> coord[i] = (q -> coord[i] - height);
		} else {
			p_new -> coord[i_star] = (min_dim_values[i_star] + width);			// line 20
			p_new -> coord[i] = (q -> coord[i] + (width * b_hat));
		}

		double p_utility;
		double p_new_utility;
		if (DEBUG1){
			cout << "p: (" << p -> coord[i_star] << "," << p -> coord[i] << ")" << endl;
			cout << "p_new: (" << p_new -> coord[i_star] << "," << p_new -> coord[i] << ")" << endl;
		}

		// line 21: simulate user interaction, higher utility is preferred by the user
		p_utility = (u -> coord[i_star] * p -> coord[i_star]) + (u -> coord[i] * p -> coord[i]);
		p_new_utility = (u -> coord[i_star] * p_new -> coord[i_star]) + (u->coord[i] * p_new -> coord[i]);
		if (DEBUG1){
			cout << "Utility of p: " << p_utility << endl;
			cout << "Utility of p_new: " << p_new_utility << endl;
		}


		// lines 22-25: update utility ratio bounds based on user interaction
		if (p_utility > p_new_utility){
			L[i] = b;
			if (DEBUG1){
				cout << "P was chosen, Updated L[i] to :" << L[i] << endl;
			}
		} 
		else {
			H[i] = b;
			if (DEBUG1){
				cout << "P_new was chosen, Updated H[i] to :" << H[i] << endl;
			}
		}
		
		// line 26
		i = (i + 1) % dim;
		Qcount++;
	}
	delete[] max_point->coord;
	delete[] p_new->coord;
	delete[] p->coord;
	delete[] q->coord;
	delete max_point;
    delete p_new;
	delete p;
	delete q;

	if (DEBUG2){
	cout << "*******************   TT   *******************" << endl;
	for (int i = 0; i < dim; i++){
		cout << "Lower bound L[" << i << "] of utility: " << L[i] << endl;
		cout << "Higher bound H[" << i << "] of utility: " << H[i] << endl;
	}
	cout << "******************* END TT *******************" << endl;
	}
	
	// lines 27-30
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
	vector<int> C_idx;
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
	if (DEBUG) {
		printf("TT - Found %d in I; %d false positives; alpha was %lf; avg effective epsilon was %lf; max effective epsilon was %lf.\n", inI, C_idx.size() - inI, alpha_approx, avg_effective_epsilon, max_effective_epsilon);
	}
    Csize = C_idx.size();

    return alpha_approx;
}