// this file includes unit tests for functions written in utility.cpp

#include "utility.h"
#include <cassert>

// helper function to create a point
point_t* create_point(double x, double y) {
    point_t* p = new point_t;
    p->dim = 2;
    p->coord = new double[2];
    p->coord[0] = x;
    p->coord[1] = y;
    return p;
}

// free memory for a point
void free_point(point_t* p) {
    delete[] p->coord;
    delete p;
}

void test_min_slope() {
    {
        point_set_t P;
        P.numberOfPoints = 3;
        P.points = new point_t*[P.numberOfPoints];

        // regular case
        P.points[0] = create_point(1, 1);
        P.points[1] = create_point(3, 3);
        P.points[2] = create_point(2, 2);

        SLOPE_TYPE result = min_slope(&P);
        assert(result == 1.0);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 3;
        P.points = new point_t*[P.numberOfPoints];

        // regular case with zero slope
        P.points[0] = create_point(1, 1);
        P.points[1] = create_point(2, 1);
        P.points[2] = create_point(3, 3);

        SLOPE_TYPE result = min_slope(&P);
        assert(result == 0.0);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 3;
        P.points = new point_t*[P.numberOfPoints];

        // regular case with negative slope
        P.points[0] = create_point(1, 1);
        P.points[1] = create_point(2, 0);
        P.points[2] = create_point(3, 3);

        SLOPE_TYPE result = min_slope(&P);
        assert(result == -1.0);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 1;
        P.points = new point_t*[P.numberOfPoints];

        // edge case - cannot compute slope
        P.points[0] = create_point(1, 1);

        SLOPE_TYPE result = min_slope(&P);
        assert(result == INF);

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    cout << "Finished testing min slope" << endl;
}

void test_display_points(){
    //display_points(point_set_t* P, int s, double alpha, double beta, int num_iterations);

}

void test_display_points(){
    //display_points(point_set_t* P, int s, double alpha, double beta, int num_iterations);

}

int main() {
    test_min_slope();
    return 0;
}