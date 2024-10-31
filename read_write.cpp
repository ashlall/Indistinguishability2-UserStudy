#include "read_write.h"

// read points from the input file
point_set_t* read_points(char* input)
{
	FILE* c_fp;

	char filename[MAX_FILENAME_LENG];
	sprintf(filename, "input/%s", input);

	if ((c_fp = fopen(filename, "r")) == NULL)
	{
		fprintf(stderr, "Cannot open the data file %s.\n", filename);
		exit(0);
	}

	int number_of_points, dim;
	fscanf(c_fp, "%i%i", &number_of_points, &dim);

	point_set_t* point_set = alloc_point_set(number_of_points);


	double max = 0;

	// read points line by line
	for (int i = 0; i < number_of_points; i++)
	{
		point_t* p = alloc_point(dim, i);
		for (int j = 0; j < dim; j++)
		{
			fscanf(c_fp, "%lf", &p->coord[j]);
			if (p->coord[j] > max)
			  max = p->coord[j];
		}
		point_set->points[i] = p;
	}

	// here
	// print out all the max values
	// first max = 5, then multiply everything in that column by 5
	// in my code, then for each column i would mulitply by the maximum value for that column


	//cout << "THIS IS THE MAX VALUE: " << max << endl;


	// normalize the points so that the maximum value in any dimension is exactly 1
	for (int i = 0; i < number_of_points; i++)
	  for (int j = 0; j < dim; j++)
	  	// got rid of dividing by max since we want to show the users real raw values of the dataset.
	    point_set->points[i]->coord[j] = point_set->points[i]->coord[j]; // / max;

	fclose(c_fp);
	return point_set;
}

// check domination
int dominates(point_t* p1, point_t* p2)
{
	int i;

	for (i = 0; i < p1->dim; ++i)
		if (p1->coord[i] < p2->coord[i])
			return 0;

	return 1;
}

// check c-domination
int c_dominates(double c, point_t* p1, point_t* p2)
{
  int i;

  for (i = 0; i < p1->dim; ++i)
    if (p1->coord[i] < p2->coord[i] * c)
      return 0;

  return 1;
}


// compute the skyline set
point_set_t* c_skyline_point(double c, point_set_t *p)
{
	int i, j, dominated, index = 0, m;
	point_t* pt;
	int* sl = new int[p->numberOfPoints];

	for (i = 0; i < p->numberOfPoints; ++i)
	{
		dominated = 0;
		pt = p->points[i];

		// check if pt is c-dominated by the skyline so far   
		for (j = 0; j < index && !dominated; ++j)
		  if (c_dominates(c, p->points[sl[j]], pt))
		    dominated = 1;

		if (!dominated)
		{
			// eliminate any points in current skyline that it c-dominates
			m = index;
			index = 0;
			for (j = 0; j < m; ++j)
			  if (!c_dominates(c, pt, p->points[sl[j]]))
			    sl[index++] = sl[j];

			// add this point as well
			sl[index++] = i;
		}
	}

	point_set_t* skyline = alloc_point_set(index);
	for (int i = 0; i < index; i++)
		skyline->points[i] = p->points[sl[i]];

	delete[] sl;
	return skyline;
}

// prepare the file for computing the convex hull (the candidate utility range R) via half space interaction
void write_hyperplanes(vector<hyperplane_t*> utility_hyperplane, point_t* feasible_pt, char* filename)
{
	//char filename[MAX_FILENAME_LENG];
	int dim = feasible_pt->dim;

	FILE *wPtr = NULL;
	//sprintf(filename, "output/hyperplane_data");

	//while(wPtr == NULL)
	//	wPtr = (FILE *)fopen(filename, "w");
	wPtr = (FILE *)fopen(filename, "w");

	// write the feasible point
	fprintf(wPtr, "%i\n1\n", dim);
	for(int i = 0; i < dim; i++)
		fprintf(wPtr, "%lf ", feasible_pt->coord[i]);
	fprintf(wPtr, "\n");

	// write the halfspaces
	fprintf(wPtr, "%i\n%i\n", dim+1, utility_hyperplane.size());
	for (int i = 0; i < utility_hyperplane.size(); i++)
	{
		for (int j = 0; j < dim; j++)
		{
			fprintf(wPtr, "%lf ",utility_hyperplane[i]->normal->coord[j]);
		}
		fprintf(wPtr, "%lf ",utility_hyperplane[i]->offset);
		fprintf(wPtr, "\n");
	}

	fclose(wPtr);

}
