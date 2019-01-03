// AXPY operations
// Integer-based multiplication and addition
void iaxpy( int length, int a, int *X, int *Y, int *Result ); 

// DOT operations
// Single-precision multiplication
void fdot( int length, int *X, int *Y, int *Result ); 

// GEMM
/* Double-precision matrix multiplication. Has multiplications, additions and ...
 * will stress the cache because of re-references of the same index.
 */
void daxpy( int length, double a, double *X, double *Y, double *Result ); 
