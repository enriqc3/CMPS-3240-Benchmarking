#include "myblas.h"

/********************************************
 * AXPY operations
 * A linear algebra operation that carries out
 * 	result(i) = a * X(i) + Y(i)
 * where a is a scalar, and X and Y are vectors
 * of the same length.
 ********************************************/

// Integer version
void iaxpy( int length, int a, int *X, int *Y, int *Result ) {
    for( int i = 0; i < length; i++ )
        Result[i] = a * X[i] + Y[i];
}

/********************************************
 * GEMM operations
 * A linear algebra operation that carries 
 * out matrix multiplication of two matrixes.
 * See Fig. 3.21. in the text book for an
 * explanation. Hennessey and Patterson 5e.
 ********************************************/

void dgemm ( int n, double *X, double *Y, double *Result) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            double Resultij = Result[i+j*n]; 
            for( int k = 0; k < n; k++ )
                Resultij += X[i+k*n] * Y[k+j*n]; 
            Result[i+j*n] = cij; 
        }
}

/********************************************
 * Dot operations
 * A linear algebra operation that carries out
 * 	Result = SUM_i( X(i) * Y(i) )
 * where a is a scalar, and X and Y are vectors
 * of the same length.
 ********************************************/

single fdot( int length, single *X, single *Y ) {
    single Result = 0;
    for( int i = 0; i < length; i++ )
        Result += X[i] * Y[i];
    return Result;
}
