#include "myblas.h"

/****************************************************************************
 * AXPY operations
 * A linear algebra operation that carries out
 * 	result(i) = a * X(i) + Y(i)
 * where a is a scalar, and X and Y are vectors of the same length.
 ****************************************************************************/

// Integer version
void iaxpy( int length, int a, int *X, int *Y, int *Result ) {
    for( int i = 0; i < length; i++ )   // Loop over every element
        Result[i] = a * X[i] + Y[i];    // Carry out AXPY op
}

/****************************************************************************
 * GEMM operations
 * A linear algebra operation that carries out matrix multiplication of two 
 * matrixes. See Fig. 3.21. in the text book for an explanation. Hennessey 
 * and Patterson 5e.
 ****************************************************************************/

void dgemm ( int n, double *X, double *Y, double *Result) {
    /* Note that we use 1-D indexing for a 2-D array. Normally it would be 
     * something like this: 
     * a[i][j]
     * but 2-D arrays are not desireable because of the overhead. A 2-D arr-
     * ay is really just a 1-D pointer to another 1-D array. Indexing is
     * carried out like this via a 1-D array:
     * a[i + j * n]
     * where n is the length of the other side of the array. This results in
     * a 2-D array without the overhead.
     */
    for (int i = 0; i < n; ++i)                     // Loop over every point
        for (int j = 0; j < n; ++j) {               // Carry out mm
            double Resultij = Result[i+j*n];        // Point is a dot prod.
            for( int k = 0; k < n; k++ )            // Iterate over necessary
                Resultij += X[i+k*n] * Y[k+j*n];    // ... to do dot prod.
            Result[i+j*n] = Resultij;               // Store result
        }
}

/****************************************************************************
 * Dot operations
 * A linear algebra operation that carries out
 * 	Result = SUM_i( X(i) * Y(i) )
 * where X and Y are vectors of the same length.
 ****************************************************************************/

float fdot( int length, float *X, float *Y ) {
    float Result = 0;                   // Initialize dot product result to 0
    for( int i = 0; i < length; i++ )   // Iterate over vectors
        Result += X[i] * Y[i];          // Carry out element-wise mult
    return Result;
}
