# CMPS-3240-Benchmarking
The start of your own BLAS library

## Prerequisites

* Read Chapter 1.6
* Understand that microprocessors have a varying clock rate, CPI and program performance
* Understand difference between execution time (also known as wall time), user time and system time
* Familiarize yourself with the operation, generic matrix multiplication (GEMM) in Figure 3.21 as C code

## Requirements

* make and GCC (or MinGW on Windows)

### Linux

If you're on Ubuntu/Debian, run: ```sudo apt-get install build-essential```  which should install make and GCC. 
*If using Odin you should not need to install additional programs.*

### Mac

On a Mac, run the command: ```xcode-select --install``` in the terminal to install both GCC and make.

### Windows

The labs in this class assume that you're using *PowerShell* in Windows, not the git bash, Cygwin or the basic terminal.

If you're on Windows you should download MinGW, a port of GNU's GCC for Windows. The link is here: http://www.mingw.org/. Make sure to install the C++ package. You also need make, which can be downloaded here: http://gnuwin32.sourceforge.net/packages/make.htm. After both installations, add the binaries of both MinGW and C++ to the path.

You may also need git to download the files in this repository. This can be found here: https://git-scm.com/download/win. 

## Objectives

* Benchmark different processors with a fixed benchmark program

## Background

Processors can vary quite a bit in their clock rate, CPI and program performance. Most PC-builders often only pay attention to the clock rate of a CPU but this is just the tip of the iceberg. Two AMD and Intel microprocessors may have the same clock rate, but the number of clock cycles it takes to execute a single instruction can vary wildly. This is due to the following:
* Instructions types (such as branching, arithmetic, floating point) take a varying amount of clock cycles
* How the instruction is implemented in hardware (via logic gates) will cause a varying amount of clock cycles per instruction 
* A varying number and type of instructions for a program
Thus, comparing microprocessors just on clock rates is an invalid way to compare them; especially when a benchmark program is not used for comparison, which would define the number and types of instructions.

The execution time of a program is calculated as (instr./program)x(cycles/instr.)x(s/cycle) where (s/cycle) is the inverse of the clock rate. The (cycles/instr.) and (s/cycle) vary from microprocessor to microprocessor. However, when comparing two processors a common practice is to fix program thus fixing the (instr./program). You may have done this before by running a benchmark program on your machine. Examples are SPEC, passmark, dhrystone, whetsone, SETI@home, jack the ripper, and so on. 

The purpose of this lab is to start your very own BLAS library that we will continuously improve over the course with our knowledge of computer architecture. You will be given a starting framework and use this code to benchmark multiple machines.

## Approach

### Part 0 - Verify git, make and gcc

If you're on odin `git`, `make` and `gcc` should be installed. The following instructions run through executing each one of these programs and will indicate if your non-odin local machine needs these things to be installed. Download this repository:

```shell
$ git clone https://github.com/DrAlbertCruz/CMPS-3240-Benchmarking.git
```

Running `make all` will compile the BLAS library into `myblas.o`, as well as compile a test program located in `myblas.c`.

```shell
$ make all
gcc -c myblas.c -Wall -O0
gcc -c test_program.c -Wall -O0
gcc test_program.o myblas.o -o test_program -Wall -O0
```

The `-Wall` flag enables all warnings from the compiler. The `-O0` flag prevents the compiler from performing any optimizations under the hood. In time, we will implement our own optimizations, and when we do so we will want to make sure that only *our* optimizations are being carries out, and not ones done automatically by the compiler. If you got to this point without any issues, you are clear to proceed to the next part of the lab.

### Part 1 - AXPY

The start of our BLAS library has only one operation called "A times X plus Y" abbreviated as "AXPY". It carries out the following operation:

a * X + Y

where a is a scalar, and X and Y are vectors (Nx1 matrixes) of the same length. This operation involves a multiplication, and an addition. AXPY operations on a large vector are a good candidate for a benchmark operation to compare different processors. 

Study `myblas.h`. There are three operations:

```c
// AXPY operations
void iaxpy( const int length, int a, int *X, int *Y, int *Result ); // For ints
void faxpy( const int length, float a, float *X, float *Y, float *Result ); // For floats
void daxpy( const int length, double a, double *X, double *Y, double *Result ); // For doubles
```

these are all the same operation, except that they take different types as an input. There are three types of operations we will consider for our BLAS library: integers, single-precision floating point values, and double-precision floating point values. Integers and single-precision floating point values take 32 bits per value, whereas a double-precision floating point value takes 64 bits. Integers are the fastest operation and double-precision floating point operations are the slowest. *Just ignore the helper functions for now. They randomly initialize an array and are not needed for this lab.*

Now consider `myblas.c`. At the top I define the AXPY operation with a C macro:

```c
#define AXPY(i, a, X, Y) (a) * (X)[i] + (Y)[i]
```

Consider the `int` version of AXPY:

```c
// Integer version
void iaxpy( const int length, int a, int *X, int *Y, int *Result ) {
	for( int i = 0; i < length; i++ )
		Result[i] = AXPY(i, a, X, Y);
}
```

This steps through the arrays and carries out the AXPY operation, defined by the `AXPY` macro. Now consider `test_program.c`. The `myblas.o` library does not actually contain a program it only contains helper subroutines that carry out basic linear algebra routines (as the name BLAS implies). `test_program.c` is set up as a benchmark program that runs the AXPY operation over an array of a size given during run time. First, consider the macros:

```c
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
```

there are three types of AXPY operations: `iaxpy`, `daxpy` and `faxpy`. Specifying `#define DP` triggers the `#ifdef DP` block. This block sets the type to double, and indicates that the function (to be called later) should be `daxpy`. This might seem like a confusing way to define this, but it makes for a more convienient way to recompile the program separately for `int`s, `float`s and `double`s. We will need to benchmark the processor later on for each of these cases. 

The program initializes variables to carry out an AXPY operation: 

```c
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
```

The size of the arrays is given at run-time as an argument via `atoi( argv[1] )`. `CONST_VAL` is defined via macro because 1.0 is a floating point literal and 1 is an integer literal, and this will vary based on the type of operation being carried out. `malloc` is used rather than defining an array the standard way via `TYPE[N]` because you cannot dynamically declare an array the later way. `malloc` will create an array for you dynamically provided that you specify the size of the array in bits. However, when allocating memory this way you must always free the memory via `free()`:

```c
free( X );
free( Y );
free( Result );
```

You should not need to modify `myblas.c` at all for this lab. You will need to tweak the `#define` at the top of `test_program.c` to switch the program to test `int`s, `float`s and `double`s. 

### Part 2 - Benchmarking

Now to the benchmarking. You can use the `time` command to time the performance of the benchmark. For the input size N, we want some arbitrarily large value so that we can really see the difference in run times for varying instruction types.  `test_program.c` comes configured for double-precision floating points. When running any experiment, you want to run it *at least three times* and take the average, so we use a bit of scripting to call a timing operation on `./test_program` three times:

```shell
$ for i in {1..3}; do time ./test_program 200000000; done;
```

This command runs the command `time ./test_program 200000000`, which, out of the box will run a DAXPY operation on vectors of size 200000000x1. On my old Dell T5500 I get the following:

```shell
Running operation of size 200000000 x 1
real	0m2.813s
user	0m1.538s
sys	0m1.270s
Running operation of size 200000000 x 1
real	0m2.815s
user	0m1.659s
sys	0m1.154s
Running operation of size 200000000 x 1
real	0m2.811s
user	0m1.686s
sys	0m1.125s
```

Recall from the text that real (wall) time includes the time that was spent by the operating system allocating memory and doing I/O. We want to focus on the user time. So, for double-precision floating points my old T5500 has an average of 2.813 seconds. To test `int`s and `float`s you will need to change the `#define DP` to `#define SP` and `#define INT` respectively, recompiling each time. For reference, I get 1.492 seconds for single-precision floating points and 1.484 seconds for integers.

You should run this benchmark operation on odin for each of the three data types. You should get faster results because I have a slower processor. To determine what processor you are running via the command line execute:

```shell
$ cat /proc/cpuinfo | grep "model name"
model name	: Intel(R) Xeon(R) CPU           E5606  @ 2.13GHz
```

You will get something different on Sleipnir. Carry out the a benchmark of AXPY for three different instruction types on at least one more computer (other than odin). Some suggestions: the local machine you're using to ssh to odin on, or your macbook.

## Lab requirements

You should have a table in your report with the average run time for the AXPY operation for integers and the two types of floating point values, for three different computers. The first computer is my T5500 (the results given in this manual), the second should be odin, and the third should be some other Linux machine.

