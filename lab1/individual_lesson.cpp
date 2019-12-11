#include <iostream>
#include <chrono>
#include <locale.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer.h>
#include <cilk/reducer_opadd.h>
#include <fstream>

const int N = 10000000;

const double PREDEL_A = 0;
const double PREDEL_B = 1;

//2.0943951023931954923084289221863
double func(double x)
{
    return 2 / (2 - 2 * x * x);
}


double intergal(int n)
{
    double sum = 0.0;
    double h = (PREDEL_B - PREDEL_A) / (n-1);
    for (auto i = 0; i < n; i++)
        sum += func(i * h) + func((i+1)*h);
    return sum * h / 2.0;
}


double parallel_integral(int n)
{
    double h = (PREDEL_B - PREDEL_A) / n;
    cilk::reducer_opadd<double> sum(0.0);
    cilk_for (auto i = 0; i < n; i++)
        sum += func(i * h) + func((i + 1)*h);
    return sum->get_value()*h / 2.0;
}

void write_result(char *filename, int n, double time_work)
{
    std::ofstream timework(filename, std::ios::app);
    timework << n << " " << time_work << "\n";
}

int main()
{
    __cilkrts_set_param("nworkers", "4");
    
    std::chrono::high_resolution_clock::time_point t_begin = std::chrono::high_resolution_clock::now();
    double result = intergal(N);
    std::chrono::duration<double> elapsed_time_serial = std::chrono::high_resolution_clock::now() - t_begin;
    std::cout << "Integral = " << result << std::endl;
    std::cout << "Elapsed time = " << elapsed_time_serial.count() << std::endl;
   
    t_begin = std::chrono::high_resolution_clock::now();
    result = parallel_integral(N);
    std::chrono::duration<double> elapsed_time_parallel = std::chrono::high_resolution_clock::now() - t_begin;
    std::cout << "Parallel Integral = " << result << std::endl;
    std::cout << "Elapsed time = " << elapsed_time_parallel.count() << std::endl;

    std::cout << "Boost = " << elapsed_time_serial.count() / elapsed_time_parallel.count() << std::endl;
    
    int point = 10000000;
    for (auto i = 100; i < point; i *= 2)
    {
        t_begin = std::chrono::high_resolution_clock::now();
        result = parallel_integral(i);
        elapsed_time_parallel = std::chrono::high_resolution_clock::now() - t_begin;
        write_result("parallel_result.txt", i, elapsed_time_parallel.count());

    }

    for (auto i = 100; i < point; i *= 2)
    {
        t_begin = std::chrono::high_resolution_clock::now();
        result =  intergal(i);
        elapsed_time_serial = std::chrono::high_resolution_clock::now() - t_begin;
        write_result("serial_result.txt", i, elapsed_time_serial.count());

    }

    return 0;
}



