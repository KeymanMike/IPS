#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <iostream>
#include <chrono>

using namespace std::chrono;
int main()
{


    high_resolution_clock::time_point t1, t2;
    t1 = high_resolution_clock::now();
    for (int j = 0; j < 1000000; ++j)

    {

        cilk_for(int i = 0; i < 4; ++i) {}


    }
    t2 = high_resolution_clock::now();
    duration<double> duration_cilk_for = (t2 - t1);

    printf("Cilk for:: %lf sec\n\n", duration_cilk_for.count());
    return 0;
}