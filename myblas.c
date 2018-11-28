#include <stdlib.h>
#include "myblas.h"

#define DAXPY(i, a, X, Y) (a) * (X)[i] + (Y)[i]

// RAND_T for random using a template. 
// C does not support templates, so use macros instead
#define RAND_T(type) (type) rand()

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
		Result[i] = DAXPY(i, a, X, Y);
}

// Float version
void faxpy( const int length, float a, float *X, float *Y, float *Result ) {
	for( int i = 0; i < length; i++ )
		Result[i] = DAXPY(i, a, X, Y);
}

// Double version
void daxpy( const int length, double a, double *X, double *Y, double *Result ) {
	for( int i = 0; i < length; i++ )
		Result[i] = DAXPY(i, a, X, Y);
}

/********************************************
 * Helper functions
 ********************************************/

// Code to randomly initialize a vector
void d_initVect( const int n, double* A ) {
  for ( int i = 0; i < n; i++ ) 
    A[ i ] = RAND_T(double); 
}

// Code to randomly initialize a matrix
void d_initMat( const int n, double* A ) {
   for ( int i = 0; i < n; i++ ) 
      for ( int j = 0; j < n; j++ ) 
         A[ i + j * n ] = RAND_T(double);
}

// Code to randomly initialize a vector
void f_initVect( const int n, float* A ) {
  for ( int i = 0; i < n; i++ ) 
    A[ i ] = RAND_T(float); // [-1,1]
}

// Code to randomly initialize a matrix
void f_initMat( const int n, float* A ) {
   for ( int i = 0; i < n; i++ ) 
      for ( int j = 0; j < n; j++ ) 
         A[ i + j * n ] = RAND_T(float); // [-1,1]
}

// Code to randomly initialize a vector
void i_initVect( const int n, int* A ) {
  for ( int i = 0; i < n; i++ ) 
    A[ i ] = rand(); // [-1,1]
}

// Code to randomly initialize a matrix
void i_initMat( const int n, int* A ) {
   for ( int i = 0; i < n; i++ ) 
      for ( int j = 0; j < n; j++ ) 
         A[ i + j * n ] = rand(); // [-1,1]
}