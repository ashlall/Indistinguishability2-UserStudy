// this file includes unit tests for functions written in utility.cpp

#include "utility.cpp"
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
    // regular case
    {
        point_set_t P;
        P.numberOfPoints = 3;
        P.points = new point_t*[P.numberOfPoints];

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

    cout << "Finished testing" << endl;
}

int main() {
    test_min_slope();
    return 0;
}