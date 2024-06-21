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

    {
        point_set_t P;
        P.numberOfPoints = 3;
        P.points = new point_t*[P.numberOfPoints];

        // edge case - vertical line
        P.points[0] = create_point(1,10);
        P.points[1] = create_point(1,20);
        P.points[2] = create_point(1,30);

        SLOPE_TYPE result = min_slope(&P);
        assert(result == INF);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 2;
        P.points = new point_t*[P.numberOfPoints];

        // edge case - identical points
        P.points[0] = create_point(50,4);
        P.points[1] = create_point(50,4);

        SLOPE_TYPE result = min_slope(&P);
        assert(result == INF);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 5;
        P.points = new point_t*[P.numberOfPoints];

        // horizontal line
        P.points[0] = create_point(5,20);
        P.points[1] = create_point(10,20);
        P.points[2] = create_point(15,20);
        P.points[3] = create_point(16,20);
        P.points[4] = create_point(40,20);
        SLOPE_TYPE result = min_slope(&P);
        assert(result == 0);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }
    
    {
        point_set_t P;
        P.numberOfPoints = 0;
        P.points = new point_t*[P.numberOfPoints];

        // edge case - no points

        SLOPE_TYPE result = min_slope(&P);
        assert(result == INF);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 4;
        P.points = new point_t*[P.numberOfPoints];

        // points with floats
        P.points[0] = create_point(2,2.5);
        P.points[1] = create_point(1,3.5);
        P.points[2] = create_point(4,1.5);
        P.points[3] = create_point(5.5,1);

        SLOPE_TYPE result = min_slope(&P);
        assert(result == -1);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 2;
        P.points = new point_t*[P.numberOfPoints];

        // regular case - large slopes
        P.points[0] = create_point(1,10000000);
        P.points[1] = create_point(2,10);

        SLOPE_TYPE result = min_slope(&P);
        assert(result == -9999990);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }   

    {
        point_set_t P;
        P.numberOfPoints = 20;
        P.points = new point_t*[P.numberOfPoints];

        // regular case - many points, varied ints and doubles
        P.points[0] = create_point(1, 2000000);
        P.points[1] = create_point(2, 175.25);
        P.points[2] = create_point(4, 3000);
        P.points[3] = create_point(5, 275.52);
        P.points[4] = create_point(3, 10);
        P.points[5] = create_point(20.5 , 20.5);
        P.points[6] = create_point(75, 1300);
        P.points[7] = create_point(100, 35);
        P.points[8] = create_point(60, 45.764);
        P.points[9] = create_point(37.6, 15);
        P.points[10] = create_point(20.0 , 9);
        P.points[11] = create_point(40, 45000.99);
        P.points[12] = create_point(14, 64.293);
        P.points[13] = create_point(120, 42);
        P.points[14] = create_point(56, 80);
        P.points[15] = create_point(42.3, 43.8);
        P.points[16] = create_point(1, 1);
        P.points[17] = create_point(90, 30.9238);
        P.points[18] = create_point(55.32, 60.203);
        P.points[19] = create_point(14, 7000); 

        SLOPE_TYPE result = min_slope(&P);
        assert(abs(-1999824.75 - result)<0.001);   // made from points (1,200000) and (2,175.25)

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 3;
        P.points = new point_t*[P.numberOfPoints];

        // checking points with same x- values to see if sorted properly
        P.points[0] = create_point(4, 20); //a
        P.points[1] = create_point(4, 5); //b
        P.points[2] = create_point(1, 25); //c

        SLOPE_TYPE result = min_slope(&P);
        assert(abs(-6.66667-result)<.0001);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }  

        {
        point_set_t P;
        P.numberOfPoints = 3;
        P.points = new point_t*[P.numberOfPoints];

        // checking points with same x- values to see if sorted properly
        P.points[0] = create_point(4, 20); //a
        P.points[1] = create_point(4, 5); //b
        P.points[2] = create_point(6, 1); //c

        SLOPE_TYPE result = min_slope(&P);
        assert(abs(-9.5-result < .0001));  
        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }  

    {
        point_set_t P;
        P.numberOfPoints = 5;
        P.points = new point_t*[P.numberOfPoints];

        // checking points with same x- values to see if sorted properly
        // we know what the min_slope should be, testing if vertical points
        // between the highest and lowest mess up sorting process
        P.points[0] = create_point(4, 20); 
        P.points[1] = create_point(4, 15); 
        P.points[2] = create_point(4, 10); 
        P.points[3] = create_point(4, 5); 
        P.points[4] = create_point(1, 25); 

        SLOPE_TYPE result = min_slope(&P);
        assert(abs(-6.66667-result)< 0.0001);
        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    } 

    {
        point_set_t P;
        P.numberOfPoints = 8;
        P.points = new point_t*[P.numberOfPoints];

        // checking points with same x- values to see if sorted properly
        // we know what the min_slope should be, testing if vertical points
        // between the highest and lowest mess up sorting process

        P.points[0] = create_point(4, 5);
        P.points[1] = create_point(4, 20); 
        P.points[2] = create_point(5, 60);
        P.points[3] = create_point(4, 15); 
        P.points[4] = create_point(6, 100);
        P.points[5] = create_point(4, 10); 
        P.points[6] = create_point(7, 101);
        P.points[7] = create_point(1, 25); 

        SLOPE_TYPE result = min_slope(&P);
        assert(abs(-6.66667-result)< 0.0001);
        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    } 
    cout << "Finished testing min slope" << endl;
}

void test_count_inversions() {
    {
        vector<int> invert;
        invert.push_back(3);
        invert.push_back(2);
        invert.push_back(1);

        int result = count_inversions(invert);
        assert (result == 3);
    }

    cout << "Finished testing count inversions" << endl;
}


void test_count_slopes() {
    {
        point_set_t P;
        P.numberOfPoints = 3;
        P.points = new point_t*[P.numberOfPoints];

        // regular case 
        P.points[0] = create_point(1, 1);
        P.points[1] = create_point(2, 2);
        P.points[2] = create_point(3, 3);

        int result = count_slopes(&P, -1, 2);
        assert(result == 3);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 5;
        P.points = new point_t*[P.numberOfPoints];

        // regular case, more points
        P.points[0] = create_point(1, 6);
        P.points[1] = create_point(2, 4);
        P.points[2] = create_point(3, 5);
        P.points[3] = create_point(4, 20);
        P.points[4] = create_point(5, 0.5);

        int result = count_slopes(&P, -15, 0);
        assert(result == 5);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 0;
        P.points = new point_t*[P.numberOfPoints];

        // edge case - no points
        int result = count_slopes(&P, -15, 0);
        assert(result == 0);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 0;
        P.points = new point_t*[P.numberOfPoints];

        // edge case - only positive slopes
        P.points[0] = create_point(1, 10);
        P.points[1] = create_point(2, 12);
        P.points[2] = create_point(3, 33);
        P.points[3] = create_point(4, 39);
        P.points[4] = create_point(5, 64);

        int result = count_slopes(&P, -100, 0);
        assert(result == 0);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 4;
        P.points = new point_t*[P.numberOfPoints];

        // regular case - checking float values
        P.points[0] = create_point(2.5, 5.5);
        P.points[1] = create_point(3.6, 2.1);
        P.points[2] = create_point(6.7, 6.7);
        P.points[3] = create_point(8.05, 1.05);

        int result = count_slopes(&P, -2.5, 0);
        assert(result == 2);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    {
        point_set_t P;
        P.numberOfPoints = 3;
        P.points = new point_t*[P.numberOfPoints];

        // edge case - slopes are exactly alpha or beta, testing if altered bounds work
        P.points[0] = create_point(2.5, 5.5);
        P.points[1] = create_point(3.5, 2.75);
        P.points[2] = create_point(4, 3);

        int result = count_slopes(&P, -2.75, 0.5, true);
        // cout << "count_slopes result : " << result << endl;
        assert(result == 3);  

        for (int i = 0; i < P.numberOfPoints; i++) {
            free_point(P.points[i]);
        }
        delete[] P.points;
    }

    cout << "Finished testing count slopes" << endl;
}


void test_display_points_v2(){
    // {
    //     // edge case - empty points_to_display
    //     // edge case - min_slope is greater than beta
    //     // edge case - what happens when there is a lot of points thats not alpha and beta
    //     // would there be a case where the random sampling does not work?

    //     point_set_t P;
    //     P.numberOfPoints = 3;
    //     P.points = new point_t*[P.numberOfPoints];

    //     // regular case 
    //     P.points[0] = create_point(1, 1);
    //     P.points[1] = create_point(2, 2);
    //     P.points[2] = create_point(3, 3);

    //     double alpha = min_slope(&P);
    //     point_t** points_to_display = display_points_v2(&P, 2, alpha, 10, 100);

    //     // cout << "Point 1 x - coordinate: " << points_to_display[0]->coord[0] << endl;
    //     // cout << "Point 1 y - coordinate: " << points_to_display[0]->coord[1] << endl;

    //     // cout << "Point 2 x - coordinate: " << points_to_display[1]->coord[0] << endl;
    //     // cout << "Point 2 y - coordinate: " << points_to_display[1]->coord[1] << endl;
    // }

    // {
    //     point_set_t P;
    //     P.numberOfPoints = 3;
    //     P.points = new point_t*[P.numberOfPoints];

    //     // regular case 
    //     P.points[0] = create_point(1, 3);
    //     P.points[1] = create_point(2, 2);
    //     P.points[2] = create_point(3, 1);

    //     double alpha = min_slope(&P);
    //     point_t** points_to_display = display_points_v2(&P, 2, alpha, 0, 100);

    //     // cout << "Point 1 x - coordinate: " << points_to_display[0]->coord[0] << endl;
    //     // cout << "Point 1 y - coordinate: " << points_to_display[0]->coord[1] << endl;

    //     // cout << "Point 2 x - coordinate: " << points_to_display[1]->coord[0] << endl;
    //     // cout << "Point 2 y - coordinate: " << points_to_display[1]->coord[1] << endl;

    // }

    // {
    //     point_set_t P;
    //     P.numberOfPoints = 5;
    //     P.points = new point_t*[P.numberOfPoints];

    //     // regular case 
    //     P.points[0] = create_point(1, 2);
    //     P.points[1] = create_point(3, 5);
    //     P.points[2] = create_point(4, 7);
    //     P.points[3] = create_point(6, 1);
    //     P.points[4] = create_point(7, 3);

    //     double alpha = min_slope(&P);
    //     point_t** points_to_display = display_points_v2(&P, 2, alpha, 0, 10);

    //     // cout << "Point 1 x - coordinate: " << points_to_display[0]->coord[0] << endl;
    //     // cout << "Point 1 y - coordinate: " << points_to_display[0]->coord[1] << endl;

    //     // cout << "Point 2 x - coordinate: " << points_to_display[1]->coord[0] << endl;
    //     // cout << "Point 2 y - coordinate: " << points_to_display[1]->coord[1] << endl;

    // }

    // {
    //     point_set_t P;
    //     P.numberOfPoints = 5;
    //     P.points = new point_t*[P.numberOfPoints];

    //     // regular case 
    //     P.points[0] = create_point(2, 3);
    //     P.points[1] = create_point(5, 8);
    //     P.points[2] = create_point(7, 2);
    //     P.points[3] = create_point(9, 6);
    //     P.points[4] = create_point(11, 10);

    //     double alpha = min_slope(&P);
    //     point_t** points_to_display = display_points_v2(&P, 2, alpha, 0, 100);

    //     cout << "Point 1 x - coordinate: " << points_to_display[0]->coord[0] << endl;
    //     cout << "Point 1 y - coordinate: " << points_to_display[0]->coord[1] << endl;

    //     cout << "Point 2 x - coordinate: " << points_to_display[1]->coord[0] << endl;
    //     cout << "Point 2 y - coordinate: " << points_to_display[1]->coord[1] << endl;

    //  }

    // {
    //     point_set_t P;
    //     P.numberOfPoints = 4;
    //     P.points = new point_t*[P.numberOfPoints];

    //     // regular case 
    //     P.points[0] = create_point(1, 3);
    //     P.points[1] = create_point(4, 7);
    //     P.points[2] = create_point(6, 2);
    //     P.points[3] = create_point(8, 9);

    //     double alpha = min_slope(&P);
    //     point_t** points_to_display = display_points_v2(&P, 2, -3, 4, 100);

    //     cout << "Point 1 x - coordinate: " << points_to_display[0]->coord[0] << endl;
    //     cout << "Point 1 y - coordinate: " << points_to_display[0]->coord[1] << endl;

    //     cout << "Point 2 x - coordinate: " << points_to_display[1]->coord[0] << endl;
    //     cout << "Point 2 y - coordinate: " << points_to_display[1]->coord[1] << endl;

    // }

    // {
    //     point_set_t P;
    //     P.numberOfPoints = 4;
    //     P.points = new point_t*[P.numberOfPoints];

    //     // regular case 
    //     P.points[0] = create_point(1, 3);
    //     P.points[1] = create_point(4, 7);
    //     P.points[2] = create_point(6, 2);
    //     P.points[3] = create_point(8, 9);

    //     double alpha = min_slope(&P);
    //     point_t** points_to_display = display_points_v2(&P, 2, -0.5, 1.5, 100);

    //     cout << "Point 1 x - coordinate: " << points_to_display[0]->coord[0] << endl;
    //     cout << "Point 1 y - coordinate: " << points_to_display[0]->coord[1] << endl;

    //     cout << "Point 2 x - coordinate: " << points_to_display[1]->coord[0] << endl;
    //     cout << "Point 2 y - coordinate: " << points_to_display[1]->coord[1] << endl;

    // }

    // cout << "Finished testing display points test" << endl;
}

int main() {
    test_min_slope();
    test_count_inversions();
    test_count_slopes();
    test_display_points_v2();
    
    return 0;
}