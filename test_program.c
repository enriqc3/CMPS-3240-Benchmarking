#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "myblas.h"

int	main( int argc, char *argv[] ) {
   if( argc != 2 ) {
     printf( "Usage: ./test_program.out N ... where N is the length of one size of the vector\n"  );
     return 0;
   }

   // Initialize random
   srand( time( 0 ) );

   // Vector is size arg[1] x 1
   const int N = atoi( argv[1] );    
   printf( "Running operation of size %d x 1", N );

   /* Create three N x 1 double precision floating point matrixes on the heap
    * using malloc, also create a scalar double A.
    */
   double A = 0.1; // Some arbitrary value for scalar A
   double *X = (double *) malloc( N * sizeof(double) );   // First vector
   double *Y = (double *) malloc( N * sizeof(double) );   // Second vector
   double *Result = (double *) malloc( N * sizeof(double) );   // Result vector
  
   /* The following code loads random values into the matrixes.
    * We don't need to initialize 'C' because it will hold the
    * result.
    */
   d_initVect( N, X );  // Initialize values in A
   d_initVect( N, Y );  // ... safe for B

   // Carry out DAXPY
   daxpy( N, A, X, Y, Result );

   // Free up the memory
   free( X );
   free( Y );
   free( Result );

   return 0;
}