#include <stdio.h>
#include <stdlib.h>
#include "myblas.h"


int main( int argc, char *argv[] ) {
   if( argc != 2 ) {
     printf( "Usage: ./test_iaxpy.out N ... where N is the length of one side of the vector\n"  );
     return 0;
   }

   // Vector is size arg[1] x 1
   int N = atoi( argv[1] );
   printf( "Running IAXPY operation of size %d x 1", N );

   // Create three N x 1 matrixes on the heap using malloc, also create a scalar
   int A = 13;                                      // Arbitrary value
   int *X = (int *) malloc( N * sizeof(int) );      // First vector
   int *Y = (int *) malloc( N * sizeof(int) );      // Second vector
   int *Result = (int *) malloc( N * sizeof(int) ); // Result vector

   // Carry out the operation
   iaxpy( N, A, X, Y, Result );

   // Free up the memory
   free( X );
   free( Y );
   free( Result );

   return 0;
}
