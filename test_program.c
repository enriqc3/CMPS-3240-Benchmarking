#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "myblas.h"

// For convenience, changes types and function names with macros.
#define DP

#ifdef DP 
  #define TYPE double
  #define CONST_VAL 1.1
  #define OPERATION daxpy
#endif
#ifdef SP 
  #define TYPE float
  #define CONST_VAL 1.1
  #define OPERATION faxpy
#endif
#ifdef INT 
  #define TYPE int
  #define CONST_VAL 1
  #define OPERATION iaxpy
#endif

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

   /* Create three N x 1 matrixes on the heap
    * using malloc, also create a scalar
    */
   TYPE A = CONST_VAL; // Some arbitrary value for scalar A
   TYPE *X = (TYPE *) malloc( N * sizeof(TYPE) );   // First vector
   TYPE *Y = (TYPE *) malloc( N * sizeof(TYPE) );   // Second vector
   TYPE *Result = (TYPE *) malloc( N * sizeof(TYPE) );   // Result vector

   // Carry out the operation
   OPERATION( N, A, X, Y, Result );

   // Free up the memory
   free( X );
   free( Y );
   free( Result );

   return 0;
}