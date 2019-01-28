# CMPS-3240-Benchmarking
Benchmarking linear algebra subroutines

## Objectives

* Learn how to benchmark a processor given an arbitrary subroutine
* Benchmark several machines and compare performance
* Gain some exposure to makefiles and C language

## Prerequisites

* Read Chapter 1.6
* Understand that microprocessors have a varying clock rate, CPI and program performance
* Understand difference between execution time (also known as wall time), user time and system time
* Familiarize yourself with the following linear algebra operations: dot product, matrix multiplication (book has C code, calls it `dgemm()`), and AXPY

## Requirements

### General

* Knowledge of linux command line interface (CLI) and `gcc`
* Some experience with C language heap allocation via `malloc()`
* Some experience with `make`

### Software

This lab requires the following software:

* `gcc`
* `make`
* `git`

`odin.cs.csubak.edu` has these already installed. If you're on Ubuntu/Debian, run: ```sudo apt-get install build-essential && sudo apt-get install git```  which should install these three things. *If using `odin.cs.csubak.edu` you should not need to install additional programs.* There is a makefile that will help with compiling the code. For this lab manual I'm assuming you've worked with make files before. If you haven't, the `makefile` included in this repository is simple enough to learn off of. Take the time to read the comments if this is your first time. 

### Compatability

| Linux | Mac | Windows |
| :--- | :--- | :--- |
| Yes | Maybe<sup>*</sup> | Maybe<sup>*</sup> |

<sup>*</sup>Untested, but assuming that you have `gcc`, `make` and `git` installed it should work. With Windows, I could never get `gcc` to work with Cygwin, so you're on your own there. This lab requires you to test a benchmark program across multiple environments (PCs), so you're encouraged to try this across multiple environments.

## Background

Processors can vary quite a bit in their clock rate, CPI and program performance. Most PC-builders often only pay attention to the clock rate of a CPU but this is just the tip of the iceberg. Two AMD and Intel microprocessors may have the same clock rate, but the number of clock cycles it takes to execute a single instruction can vary wildly. This is due to the following:

* Instructions types (such as branching, arithmetic, floating point) take a varying amount of clock cycles
* How the instruction is implemented in hardware (via logic gates) will cause a varying amount of clock cycles per instruction 
* A varying number and type of instructions for a program

Thus, comparing microprocessors just on clock rates is an invalid way to compare them; especially when a benchmark program is not used for comparison, which would define the number and types of instructions.

The execution time of a program is calculated as (instr./program)x(cycles/instr.)x(s/cycle) where (s/cycle) is the inverse of the clock rate. The (cycles/instr.) and (s/cycle) vary from microprocessor to microprocessor. However, when comparing two processors a common practice is to fix program thus fixing the (instr./program). You may have done this before by running a benchmark program on your machine. There are industry and commercial standard benchmarks:

* https://www.spec.org/benchmarks.html
* https://www.passmark.com/
* https://www.eembc.org/

Benchmarks individuals have created that commonly used (but not standard):

* Dhrystone
* Whetsone

And some benchmarks were not intended for benchmarking but are so computationally intensive that computing enthusiasts use them a relative test of performance:

* Folding@home
* SETI@home
* Jack the ripper
* Prime95 (for thermals)

The purpose of this lab is to start your very own basic linear algebra subroutine (BLAS) library that we will continuously improve over the course with our knowledge of computer architecture. You will be given a starting framework and use this code to benchmark multiple machines.

## Approach

### Part 0 - Verify `git`, `make` and `gcc`

If you're on `odin.cs.csubak.edu`, skip this step because `git`, `make` and `gcc` should be installed. The following instructions run through executing each one of these programs and will indicate if your non-odin local machine needs these things to be installed. Download this repository:

```shell
$ git clone https://github.com/DrAlbertCruz/CMPS-3240-Benchmarking.git
$ cd CMPS-3240-Benchmarking
```

Running `make all` will compile the BLAS library into `myblas.o`, as well as compile a test program located in `myblas.c`.

```shell
$ make all
gcc -c myblas.c -Wall -O0
gcc -c test_program.c -Wall -O0
gcc test_program.o myblas.o -o test_program -Wall -O0
```

The `-Wall` flag enables all warnings from the compiler. The `-O0` flag prevents the compiler from performing any optimizations under the hood. In time, we will implement our own optimizations, and when we do so we will want to make sure that only *our* optimizations are being carried out, and not ones done automatically by the compiler. If you got to this point without any issues, you are clear to proceed to the next part of the lab.

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
* `dgemm` - an operation called Double Precision Generic Matrix Multiplication (DGEMM). It performs a very specific linear algebra operation called a matrix multiplication. Not only does it test floating point operations, it has many rereferences of the same index and (unlike the other operations) should test the processor's cache as well. This is a polynomial n^2 cost operation.

Study `myblas.c` before proceeding. When you have a general understanding of what's going on, proceed. The idea for this lab is to create test programs for each of these operations that will initialize the appropriate inputs of a *very* large size. So large that it will test the performance of a processor. Take a look at `test_iaxpy.c`. It initializes variables to carry out an AXPY operation: 

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

The size of the arrays is given at run-time as an argument via `atoi( argv[1] )`. `malloc()` is used rather than defining an array the standard way via `TYPE[N]` because you cannot dynamically declare an array the later way. `malloc()` will create an array for you dynamically provided that you specify the size of the array in bits. However, when allocating memory this way you must always free the memory via `free()` when done:

```c
free( X );
free( Y );
free( Result );
```

We also want to use `malloc()` because there are limits to the size of an array declared in the traditional way via `TYPE[N]`--due to system limitations of the size of an arrays that can be allocated on the stack, and we will definitely be exceeding this limit. Before proceeding to the next section, study `test_iaxpy.c`. Do the following:

* Create a test program for `fdot` from `test_iaxpy.c`, and make appropriate targets for it in the makefile.
* Repeat for `dgemm`. Note that when allocating the arrays for `dgemm` that is is n^2, so your need to modify your allocation as follows: `(double *) malloc( N * N * sizeof(double) )`.

### Part 2 - Benchmarking

Now to the benchmarking. You can use the `time` command to time the performance of the benchmark. For the input size N, we want some arbitrarily large value so that we can really see the difference in run times for varying instruction types. When running any experiment, you want to run it *at least three times* and take the average, so we use a bit of scripting to call a timing operation on `./test_iaxpy.out` three times. Insert the following into the command line:

```shell
$ for i in {1..3}; do time ./test_iaxpy.out 200000000; done;
```

This command runs the command `time ./test_iaxpy.out 200000000`, which, out of the box will run a `iaxpy` operation on vectors of size 200000000x1. On my own Dell Latitude E5470 laptop I get the following:

```shell
Running operation of size 200000000 x 1
real    0m1.230s
user    0m0.791s
sys     0m0.438s
Running operation of size 200000000 x 1
real    0m1.228s
user    0m0.789s
sys     0m0.439s
Running operation of size 200000000 x 1
real    0m1.220s
user    0m0.733s
sys     0m0.484s
```

Recall from the text that real (wall) time includes the time that was spent by the operating system allocating memory and doing I/O. We want to focus on the user time. So, for `iaxpy` my Dell Latitude E5470 has an average of 0.771 seconds. You should run this benchmark operation on `odin.cs.csubak.edu` for each of the three operations. *This means you must make benchmark programs for `fdot` and `dgemm` because they are not provided with the repo.* You should get faster results because I have a slower processor. To determine what processor you are running via the command line execute:

```shell
$ cat /proc/cpuinfo | grep "model name"
model name	: Intel(R) Core(TM) i5-6440HQ CPU @ 2.60GHz
```

you can also get the cache size with the following:

```shell
$ cat /proc/cpuinfo | grep "cache size"
cache size	: 6144KB KB
```

You will get something different on `odin.cs.csubak.edu`, `sleipnir.cs.csubak.edu` and the other machines you intend to benchmark. Carry out the a benchmark of the three operations:

* `iaxpy` - For N = 200000000
* `fdot` - For N = 200000000
* `dgemm` - For N = 1024. Do not try to run this for N = 200000000 the operation is too large to run even on `odin.cs.csubak.edu`.

each on at least one more computer (other than odin). Some suggestions: the local machine you're using to ssh to `odin.cs.csubak.edu` on (if linux), `sleipnir.cs.csubak.edu` (if you have a login for that), your macbook, etc.

## Check off

For check off, do the following:

* Show your version of the DGEMM test program to the instructor
* Aggregate your results into a table, and show your results to the instructor. It should look something like:


| Operation | `iaxpy` | `fdot` | `dgemm` |
| :--- | :--- | :--- | :--- |
| Dell Latitude E5470 w/ Intel Core i5-6440HQ | 0.771 | 0.790 | 4.110 |
| `odin.cs.csubak.edu` w/ Intel Xeon E5-2630 v4 |  |  |  |
| Local machine |  |  |  |
| My linux laptop |  |  |  |
