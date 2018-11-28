#include <stdlib.h>
#include "myblas.h"

/********************************************
 * AXPY operations
 * A linear algebra operation that carries out
 * 	a * X + Y
 * where a is a scalar, and X and Y are vectors
 * of the same length.
 *
 * You could probably use macros for this, but in the
 * future with SSE and AVX operations and macros 
 * will not work so well for that scenario
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
void d_initVect( const int n, double* A ) {
  for ( int i = 0; i < n; i++ ) 
    A[ i ] = 2 * rand() / (double) RAND_MAX - 1; // [-1,1]
}

// Code to randomly initialize a matrix
void d_initMat( const int n, double* A ) {
   for ( int i = 0; i < n; i++ ) 
      for ( int j = 0; j < n; j++ ) 
         A[ i + j * n ] = 2 * rand() / (double) RAND_MAX - 1; // [-1,1]
}

// Code to randomly initialize a vector
void f_initVect( const int n, float* A ) {
  for ( int i = 0; i < n; i++ ) 
    A[ i ] = 2 * rand() / (float) RAND_MAX - 1; // [-1,1]
}

// Code to randomly initialize a matrix
void f_initMat( const int n, float* A ) {
   for ( int i = 0; i < n; i++ ) 
      for ( int j = 0; j < n; j++ ) 
         A[ i + j * n ] = 2 * rand() / (float) RAND_MAX - 1; // [-1,1]
}

// Code to randomly initialize a vector
void i_initVect( const int n, int* A ) {
  for ( int i = 0; i < n; i++ ) 
    A[ i ] = 2 * rand() / (double) RAND_MAX - 1; // [-1,1]
}

// Code to randomly initialize a matrix
void i_initMat( const int n, int* A ) {
   for ( int i = 0; i < n; i++ ) 
      for ( int j = 0; j < n; j++ ) 
         A[ i + j * n ] = 2 * rand() / (double) RAND_MAX - 1; // [-1,1]
}