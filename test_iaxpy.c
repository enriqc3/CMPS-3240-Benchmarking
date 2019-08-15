#include <stdio.h>
#include <stdlib.h>

//  The benchmark function. IAXPY takes two integer arrays and a scalar as 
//  input. It adds the two arrays, element by element, storing the results in 
//  Result. The first term is multiplied by the scalar, called A. Hence, why 
//  it is called AXPY: scalar A times X plus Y. The I in the name indicates it 
//  is an integer.
void iaxpy( int length, int A, int *X, int *Y, int *Result ) {
  for( int i = 0; i < length; i++ )
    Result[i] = A * X[i] + Y[i]; 
}

//  The benchmark
int main( void ) {
  const int N = 200000000; // The benchmark runs on a fixed size of work
  printf( "Running IAXPY operation of size %d x 1\n", N );

  //  Create three N x 1 matrixes on the heap using malloc. There are 
  //  generally two reasons why we do not want to do this statically i.e. int 
  //  X[N]. One, with the benchmark program we're making we may exceed the 
  //  allowable size of a static array (yep, this is possible given we are 
  //  taxing the system). Two, if you wanted to, you could dynamically adjust 
  //  the workload via the command line (though this is not how it is currently
  //  set up).
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
