# CMPS-3240-Benchmarking
Benchmarking linear algebra subroutines

## Objectives

* Learn how to benchmark a processor given an arbitrary subroutine
* Benchmark several machines and compare performance

## Prerequisites

* Read Chapter 1.6
* Understand that microprocessors have a varying clock rate, CPI and program performance
* Understand difference between execution time (also known as wall time), user time and system time
* Familiarize yourself with the following linear algebra operations: dot product, matrix multiplication (book has C code), and AXPY

## Requirements

### General

* Knowledge of linux CLI and GCC
* Some experience with C language `printf()`
* Some experience with `make`

### Software

This lab requires the following software:

* `gcc`
* `make`
* `git`

odin.cs.csubak.edu has these already installed.


* make and GCC (or MinGW on Windows). If you're on Ubuntu/Debian, run: ```sudo apt-get install build-essential```  which should install make and GCC. *If using Odin you should not need to install additional programs.*

### Compatability

| Linux | Mac | Windows |
| :--- | :--- | :--- |
| Yes | Yes | Untested<sup>*</sup> |

<sup>*</sup>Untested, but assuming that you have `gcc`, `make` and `git` installed it should work.

This lab requires you to test a benchmark program across multiple environments (PCs), so you're encouraged to try this across multiple environments.

## Background

Processors can vary quite a bit in their clock rate, CPI and program performance. Most PC-builders often only pay attention to the clock rate of a CPU but this is just the tip of the iceberg. Two AMD and Intel microprocessors may have the same clock rate, but the number of clock cycles it takes to execute a single instruction can vary wildly. This is due to the following:
* Instructions types (such as branching, arithmetic, floating point) take a varying amount of clock cycles
* How the instruction is implemented in hardware (via logic gates) will cause a varying amount of clock cycles per instruction 
* A varying number and type of instructions for a program
Thus, comparing microprocessors just on clock rates is an invalid way to compare them; especially when a benchmark program is not used for comparison, which would define the number and types of instructions.

The execution time of a program is calculated as (instr./program)x(cycles/instr.)x(s/cycle) where (s/cycle) is the inverse of the clock rate. The (cycles/instr.) and (s/cycle) vary from microprocessor to microprocessor. However, when comparing two processors a common practice is to fix program thus fixing the (instr./program). You may have done this before by running a benchmark program on your machine. Examples are SPEC, passmark, dhrystone, whetsone, SETI@home, jack the ripper, and so on. 

The purpose of this lab is to start your very own basic linear algebra subroutine (BLAS) library that we will continuously improve over the course with our knowledge of computer architecture. You will be given a starting framework and use this code to benchmark multiple machines.

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

The start of our BLAS library has only three operations defined. Open up `myblash.h` with `vim myblas.h`:

```c
void iaxpy( int length, int a, int *X, int *Y, int *Result ); 
float fdot( int length, float *X, float *Y );
void dgemm ( int length, double *X, double *Y, double *Result );
```

If you're curious about what these do, feel free to read about them using whatever resources at your disposal. If you haven't taken linear algebra yet, just understand that these are array operations that can be very costly:

* `iaxpy` - an operation called "A times X plus Y" abbreviated as "AXPY". The prefix `i` indicates it is meant for integers. Element-wise, it multiplies the scalar A times Xi and adds that to Yi. It is a linear cost operation. Carrying out an iaxpy operation with two large arrays will test the integer multiplication and addition operations of a processor. 
* `fdot` - an operation called dot product. The prefix `f` indicates it is meant for single-precision floating point values. Element-wise, it multiplies Xi and Yi, and cummulatively sums the result. Unlike the other operations, it returns a scalar. It is a linear cost operation. Carrying out an fdot operation with two large arrays will test the floating point multiplication operations of a processor. 
* `dgemm` - an operation called Double Precision Generic Matrix Multiplication (DGEMM). It performs a very specific linear algebra operation called a matrix multiplication. Not only does it test floating point operations, it has many rereferences of the same index and (unlike the other operations) should test the processor's cache as well.

Study `myblas.c` before proceeding. When you have a general understanding of what's going on, proceed. The idea for this lab is to create test programs for each of these operations that will initialize the appropriate inputs of a *very* arbitrary size. So large that it will test the performance of a processor. Take a look at `test_iaxpy.c`. It initializes variables to carry out an AXPY operation: 

```c
    // Vector is size arg[1] x 1
    const int N = atoi( argv[1] );    
    printf( "Running operation of size %d x 1", N );

    /* Create three N x 1 matrixes on the heap
     * using malloc, also create a scalar
     */
    int A = 13;                                      // Arbitrary value
    int *X = (int *) malloc( N * sizeof(int) );      // First vector
    int *Y = (int *) malloc( N * sizeof(int) );      // Second vector
    int *Result = (int *) malloc( N * sizeof(int) ); // Result vector
```

The size of the arrays is given at run-time as an argument via `atoi( argv[1] )`. `malloc` is used rather than defining an array the standard way via `TYPE[N]` because you cannot dynamically declare an array the later way. `malloc` will create an array for you dynamically provided that you specify the size of the array in bits. However, when allocating memory this way you must always free the memory via `free()`:

```c
free( X );
free( Y );
free( Result );
```

Before proceeding to the next section, study `test_iaxpy.c` and do the following:

* Create a test program for `fdot` from `test_iaxpy.c`, and make appropriate targets for it in the makefile.
* Repeat for `dgemm`

### Part 2 - Benchmarking

Now to the benchmarking. You can use the `time` command to time the performance of the benchmark. For the input size N, we want some arbitrarily large value so that we can really see the difference in run times for varying instruction types. When running any experiment, you want to run it *at least three times* and take the average, so we use a bit of scripting to call a timing operation on `./test_iaxpy` three times. Insert the following into the command line:

```shell
$ for i in {1..3}; do time ./test_iaxpy 200000000; done;
```

This command runs the command `time ./test_iaxpy 200000000`, which, out of the box will run a `iaxpy` operation on vectors of size 200000000x1. On my old Dell T5500 I get the following:

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

Recall from the text that real (wall) time includes the time that was spent by the operating system allocating memory and doing I/O. We want to focus on the user time. So, for iaxpy my old T5500 has an average of 1.423 seconds. You should run this benchmark operation on odin for each of the three operations. You should get faster results because I have a slower processor. To determine what processor you are running via the command line execute:

```shell
$ cat /proc/cpuinfo | grep "model name"
model name	: Intel(R) Xeon(R) CPU           E5606  @ 2.13GHz
```

you can also get the cache size with the following:

```shell
$ cat /proc/cpuinfo | grep "cache size"
cache size	: 25600 KB
```

You will get something different on Sleipnir. Carry out the a benchmark of AXPY for three operations on at least one more computer (other than odin). Some suggestions: the local machine you're using to ssh to odin on (if linux), Sleipnir, or your macbook.

## Check off

For check off, do the following:

* Show your version of the DGEMM test program to the instructor
* Compile your results into a table, and show your results to the instructor. It should look something like:


| Operation | `iaxpy` | `fdot` | `dgemm` |
| :--- | :--- | :--- | :--- |
| `odin.cs.csubak.edu` |  |  |  |
| Local machine |  |  |  |
| My linux laptop |  |  |  |
