#include <stdio.h>
#include <ctime>

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_vector.h>
#include <cilk/reducer_list.h>

#include <cilk/reducer_opadd.h>
//#include <cilk/cilk.h>
//#include <cilk/cilk_api.h>
#include <cilk/reducer.h>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std::chrono;
duration<double> duration_serial;
duration<double> duration_parallel;
// ���������� ����� � �������� ���������� �������
const int MATRIX_SIZE = 1500;

/// ������� InitMatrix() ��������� ���������� � �������� 
/// ��������� ���������� ������� ���������� ����������
/// matrix - �������� ������� ����
void InitMatrix( double** matrix )
{
	for ( int i = 0; i < MATRIX_SIZE; ++i )
	{
		matrix[i] = new double[MATRIX_SIZE + 1];
	}

	for ( int i = 0; i < MATRIX_SIZE; ++i )
	{
		for ( int j = 0; j <= MATRIX_SIZE; ++j )
		{

                matrix[i][j] = rand() % 2500 + 1;
		}
	}
}

/// ������� SerialGaussMethod() ������ ���� ������� ������ 
/// matrix - �������� ������� �������������� ���������, �������� � ����,
/// ��������� ������� ������� - �������� ������ ������ ���������
/// rows - ���������� ����� � �������� �������
/// result - ������ ������� ����
void SerialGaussMethod( double **matrix, const int rows, double* result )
{
	int k;
	double koef;

    high_resolution_clock::time_point t1, t2;
    t1 = high_resolution_clock::now();
	// ������ ��� ������ ������
	for ( k = 0; k < rows; ++k )
	{
		//
		for ( int i = k + 1; i < rows; ++i )
		{
			koef = -matrix[i][k] / matrix[k][k];

			for ( int j = k; j <= rows; ++j )
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}
	}

    t2 = high_resolution_clock::now();
    duration_serial = (t2 - t1);
    printf("Serial direct passege is:: %lf sec\n\n", duration_serial.count());

	// �������� ��� ������ ������
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for ( k = rows - 2; k >= 0; --k )
	{
		result[k] = matrix[k][rows];

		//
		for ( int j = k + 1; j < rows; ++j )
		{
			result[k] -= matrix[k][j] * result[j];
		}

		result[k] /= matrix[k][k];
	}
}

/// ������� ParallelGaussMethod() ������ ���� ������� ������ 
/// matrix - �������� ������� �������������� ���������, �������� � ����,
/// ��������� ������� ������� - �������� ������ ������ ���������
/// rows - ���������� ����� � �������� �������
/// result - ������ ������� ����
void ParallelGaussMethod(double **matrix, const int rows, double* result)
{
    int k;
    //double koef;

    high_resolution_clock::time_point t1, t2;
    t1 = high_resolution_clock::now();

    
    // ������ ��� ������ ������
    for (k = 0; k < rows; ++k)
    {
        //cilk::reducer_opadd<double> koeff(0.0);
        cilk_for (int i = k + 1; i < rows; ++i)
        {
            
            //koeff->set_value(-matrix[i][k] / matrix[k][k]);
            double koef = -matrix[i][k] / matrix[k][k];
            for(int j = k; j <= rows; ++j)
            {

                matrix[i][j] += koef * matrix[k][j];
                //matrix[i][j] += koeff->get_value() * matrix[k][j];
            }
            
        }
    }

    t2 = high_resolution_clock::now();
    duration_parallel = (t2 - t1);
    printf("Parallel direct passege is:: %lf sec\n\n", duration_parallel.count());    
        
    // �������� ��� ������ ������
    result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

    for (k = rows - 2; k >= 0; --k)
    {
        cilk::reducer_opadd<double> result_k(matrix[k][rows]);

        cilk_for(int j = k + 1; j < rows; ++j)
        {
            result_k -= matrix[k][j] * result[j];
            //result[k] -= matrix[k][j] * result[j];
        }

        //result[k] /= matrix[k][k];
        result[k] = result_k->get_value() / matrix[k][k];

    }
}

//#define TEST_MODE
#define PARALLEL_MODE
int main()
{
	srand( (unsigned) time( 0 ) );

    // ������������� ���������� ���������� ������� = 4
    __cilkrts_set_param("nworkers", "4");

	int i;

#ifdef TEST_MODE
	
    // ���-�� ����� � �������, ���������� � �������� �������
	const int test_matrix_lines = 4;

#else
    // ���-�� ����� � �������, ���������� � �������� �������

    const int test_matrix_lines = MATRIX_SIZE;
#endif /*TEST_MODE*/

	double **test_matrix = new double*[test_matrix_lines];

	// ���� �� �������
	for ( i = 0; i < test_matrix_lines; ++i )
	{
		// (test_matrix_lines + 1)- ���������� �������� � �������� �������,
		// ��������� ������� ������� ������� ��� ������ ����� ���������, �������� � ����
		test_matrix[i] = new double[test_matrix_lines + 1];
	}

	// ������ ������� ����
	double *result = new double[test_matrix_lines];
    //cilk::reducer<cilk::op_vector<double>>result;

#ifdef TEST_MODE
	// ������������� �������� �������
	test_matrix[0][0] = 2; test_matrix[0][1] = 5;  test_matrix[0][2] = 4;  test_matrix[0][3] = 1;  test_matrix[0][4] = 20;
	test_matrix[1][0] = 1; test_matrix[1][1] = 3;  test_matrix[1][2] = 2;  test_matrix[1][3] = 1;  test_matrix[1][4] = 11;
	test_matrix[2][0] = 2; test_matrix[2][1] = 10; test_matrix[2][2] = 9;  test_matrix[2][3] = 7;  test_matrix[2][4] = 40;
	test_matrix[3][0] = 3; test_matrix[3][1] = 8;  test_matrix[3][2] = 9;  test_matrix[3][3] = 2;  test_matrix[3][4] = 37;

#ifdef PARALLEL_MODE
    
    ParallelGaussMethod(test_matrix, test_matrix_lines, result);
#else
	SerialGaussMethod( test_matrix, test_matrix_lines, result );
#endif /*PARALLEL_MODE*/

#else
    
    InitMatrix(test_matrix);

#ifdef PARALLEL_MODE
    SerialGaussMethod(test_matrix, test_matrix_lines, result);
    ParallelGaussMethod(test_matrix, test_matrix_lines, result);

#else
    SerialGaussMethod(test_matrix, test_matrix_lines, result);
#endif /*PARALLEL_MODE*/ 

#endif /*TEST_MODE*/

	for ( i = 0; i < test_matrix_lines; ++i )
	{
		delete[]test_matrix[i];
	}

    printf("Boost = %f\n", duration_serial.count() / duration_parallel.count());
	printf( "Solution:\n" );

	for ( i = 0; i < test_matrix_lines; ++i )
	{
		printf( "x(%d) = %lf\n", i, result[i] );
	}

	delete[] result;

	return 0;
}