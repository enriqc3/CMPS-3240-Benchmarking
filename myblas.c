#include <stdlib.h>

/********************************************
 * AXPY operations
 * A linear algebra operation that carries out
 * 	a * X + Y
 * where a is a scalar, and X and Y are vectors
 * of the same length.
 ********************************************/

// Integer version
void iaxpy( const int length, int a, int *X, int *Y, int *Result ) {
	for( int i = 0; i < length; i++ )
		Result[i] = a * X[i] + Y[i];
}

// Float version
void faxpy( const int length, float a, float *X, float *Y, float *Result ) {
	for( int i = 0; i < length; i++ )
		Result[i] = a * X[i] + Y[i];
}

// Double version
void daxpy( const int length, double a, double *X, double *Y, double *Result ) {
	for( int i = 0; i < length; i++ )
		Result[i] = a * X[i] + Y[i];
}

/********************************************
 * Helper functions
 ********************************************/

// Code to randomly initialize a vector
void initRandVect( const int n, double* A ) {
  for ( int i = 0; i < n; i++ ) 
    A[ i ] = 2 * rand() / (double) RAND_MAX - 1; // [-1,1]
}

// Code to randomly initialize a matrix
void initRandMat( const int n, double* A ) {
   for ( int i = 0; i < n; i++ ) 
      for ( int j = 0; j < n; j++ ) 
         A[ i + j * n ] = 2 * rand() / (double) RAND_MAX - 1; // [-1,1]
}