#include <stdio.h>
#include <stdlib.h>


float fdot( int length, float *X, float *Y ) {
  float sum = 0;
  for( int i = 0; i < length; i++ )
    sum += X[i] + Y[i];
  return sum;
}

//  The benchmark
int main( void ) {
  const int N = 200000000; // The benchmark runs on a fixed size of work
  printf( "Running FDOT operation of size %d x 1\n", N );


  float *X = (float *) malloc( N * sizeof(float) );      // First vector
  float *Y = (float *) malloc( N * sizeof(float) );      // Second vector

   // Carry out the operation
   float result = fdot( N, X, Y );

   // Free up the memory
   free( X );
   free( Y );

   return 0;
}
