# CMPS 3240 Lab: Introduction to benchmarking

## Objectives

During this lab you will:

* Benchmark a processor with arbitrary subroutines that you have coded yourself,
* Compare several machines according to the benchmarks, and
* Code in Make and C language

using any machine (you may use your own POSIX laptop).

## Prerequisites

This lab assumes you have read or are familiar with the following topics:

* Chapter 1.6
* Knowledge of linux command line interface (CLI) and `gcc`
* Clock rate, CPI and program performance
* Difference between execution time (also known as wall time), user time and system time
* Linear algebra operations: dot product, matrix multiplication (book has C code, calls it `dgemm()`), and AXPY
* Some experience with C language
** Heap allocation via `malloc()`
* Some experience with `make`

Please study these topics if you are not familiar with them so that the lab can be completed in a timely manner.

## Requirements

The following is a list of requirements to complete the lab. Some labs can completed on any machine, whereas some require you to use a specific departmental teaching server. You will find this information here.

### Software

This lab requires the following software:

* `gcc`
* `make`
* `git`

`odin.cs.csubak.edu` has these already installed. If you're on Ubuntu/Debian, run: ```sudo apt-get install build-essential && sudo apt-get install git```  which should install these three things. 

This course will use Makefiles to automate compiling your code. For this lab manual I'm assuming you've worked with Make before. If you haven't, the `makefile` included in this repository is simple enough to learn from. Take time to read the comments if this is your first time with Make.

### Compatability

| Linux | Mac | Windows |
| :--- | :--- | :--- |
| Yes | Yes | Untested<sup>*</sup> |

<sup>*</sup>Untested, but assuming that you have `gcc`, `make` and `git` installed it should work. With Windows, I could never get `gcc` to work with Cygwin, so you're on your own there. This lab requires you to test a benchmark program across multiple environments (PCs), so you're encouraged to try this on machines with different configurations.

#### Mac

Mac is a POSIX operating system and should be most compatible with the labs, which were created on Debian 6.3.0 with GCC version 6.3.0. However, Mac actually uses `clang` compiler, which is different from `gcc`. They alias `gcc` to `clang`, so even if you call `gcc` you are not using it. In the future, there will be labs that look at assembly code, and the resulting assembly mnemonics will be wildly different between `gcc` and `clang`. You should be OK using your Mac for this lab. If you plan to use your laptop, you should install xcode which will contain `clang`/`gcc`:

```shell
$ xcode-select --install
```

Verify with the following:

```shell
$ gcc --version
Configured with: --prefix=/Library/Developer/CommandLineTools/usr --with-gxx-include-dir=/Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk/usr/include/c++/4.2.1
Apple LLVM version 10.0.0 (clang-1000.10.44.4)
Target: x86_64-apple-darwin18.2.0
Thread model: posix
InstalledDir: /Library/Developer/CommandLineTools/usr/bin
```

You will get something similar depending on what version of Mac OS version you are using. If you're getting errors with `gcc` in Mac, sometimes doing this will help:

```shell
$ xcode-select --reset
```

This is not a complete manual for how to install a C compiler on your machine and if things get off the rails you may want to just use a departmental computer/server.

#### Windows

TODO: Windows.

## Background

Processors can vary quite a bit in their clock rate, CPI and program performance. Most PC-builders often only pay attention to the clock rate of a CPU but this is just the tip of the iceberg. Two AMD and Intel microprocessors may have the same clock rate, but the number of clock cycles it takes to execute a single instruction can vary wildly. This is due to the following:

* Instructions types (such as branching, arithmetic, floating point) take a varying amount of clock cycles
* How the instruction is implemented in hardware (via logic gates) will cause a varying amount of clock cycles per instruction 
* A varying number and type of instructions for a program

Thus, comparing microprocessors just on clock rates is a flawed way to compare them; especially when a benchmark program is not brought up, which would define the number and types of instructions.

The execution time of a program is calculated as (instr./program)x(cycles/instr.)x(s/cycle) where (s/cycle) is the inverse of the clock rate. The (cycles/instr.) and (s/cycle) vary from microprocessor to microprocessor. However, when comparing two processors a common practice is to fix program thus fixing the (instr./program). You may have done this before by running a benchmark program on your machine. There are industry and commercial standard benchmarks:

* https://www.spec.org/benchmarks.html
* https://www.passmark.com/
* https://www.eembc.org/

Benchmarks that individuals have created that are commonly used (but not standard):

* Dhrystone
* Whetsone

And some benchmarks were not intended for benchmarking at all. But, they are so computationally intensive that computing enthusiasts use them a relative test of performance:

* Folding@home
* SETI@home
* Jack the ripper
* Prime95<sup>a</sup>

The purpose of this lab is to start your very set of benchmark programs. Some of these subroutines will be revisited and continuously improved over the course with our knowledge of computer architecture. You will be given a starting framework and use this code to benchmark multiple machines.

## Approach

### Part 0 - Verify `git`, `make` and `gcc`

If you're on `odin.cs.csubak.edu`, skip this step because `git`, `make` and `gcc` should be installed. The following instructions run through executing each one of these programs and will indicate if your non-odin local machine needs these things to be installed. Download this repository:

```shell
$ git clone https://github.com/DrAlbertCruz/CMPS-3240-Benchmarking.git
$ cd CMPS-3240-Benchmarking
```

Running `make all` will compile the benchmark program into `test_iaxpy.out`, as well as its pre-linked binary `test_iaxpy.o`.

```shell
$ make
gcc -Wall -O0 -c test_iaxpy.c -o test_iaxpy.o
gcc -Wall -O0 -o test_iaxpy.out test_iaxpy.o
```

By default Make will execute the first target in the file. The `-Wall` flag enables all warnings from the compiler. The `-O0` flag prevents the compiler from performing any optimizations under the hood. In time, we will implement our own optimizations, and when we do so we will want to make sure that only *our* optimizations are being carried out, and not ones done automatically by the compiler on our behalf.<sup>b</sup> If you got to this point without any issues, you are clear to proceed to the next part of the lab. 

### Part 1 - AXPY

The goal of this lab is to implement three different benchmark programs:

```c
void iaxpy( int length, int a, int *X, int *Y, int *Result ); 
float fdot( int length, float *X, float *Y );
void dgemm ( int length, double *X, double *Y, double *Result );
```

If you're curious about what these do, feel free to read about them using whatever resources at your disposal. If you haven't taken linear algebra yet, just understand that these are array operations that can be very costly:

* `iaxpy` - an operation called "A times X plus Y" abbreviated as "AXPY". The prefix `i` indicates it is meant for integers. Element-wise, it multiplies the scalar A times Xi and adds that to Yi. It is a linear cost operation. Carrying out an iaxpy operation with two large arrays will test the integer multiplication and addition operations of a processor.<sup>c</sup> 
* `fdot` - an operation called dot product. The prefix `f` indicates it is meant for single-precision floating point values. Element-wise, it multiplies Xi and Yi, and cummulatively sums the result. Unlike the other operations, it returns a scalar. It is a linear cost operation. Carrying out an fdot operation with two large arrays will test the floating point multiplication operations of a processor. 
* `dgemm` - an operation called Double Precision Generic Matrix Multiplication (DGEMM). It performs a very specific linear algebra operation called a matrix multiplication. Not only does it test floating point operations, it has many rereferences of the same index and (unlike the other operations) should test the processor's cache as well. This is a polynomial n^2 cost operation.

Study `text_iaxpy.c` before proceeding. When you have a general understanding of what's going on, proceed. The idea for this lab is to create test programs for each of these operations that will initialize the appropriate inputs of a *very* large size. So large that it will test the performance of a processor. Take a look at `test_iaxpy.c`. It declares and defines `iaxpy()`:

```c
void iaxpy( int length, int A, int *X, int *Y, int *Result ) {
  for( int i = 0; i < length; i++ )
    Result[i] = A * X[i] + Y[i]; 
}
```

It then allocates some test arrays dynamically on the heap:

```c
const int N = 200000000;
printf( "Running IAXPY operation of size %d x 1", N );

int A = 13;                                     
int *X = (int *) malloc( N * sizeof(int) );      
int *Y = (int *) malloc( N * sizeof(int) );      
int *Result = (int *) malloc( N * sizeof(int) ); 
iaxpy( N, A, X, Y, Result );
```

The size of the arrays is defined at compie-time as `N`. `malloc()` is used rather than defining an array the standard way via `TYPE[N]` because you cannot dynamically declare an array the later way. `malloc()` will create an array for you dynamically provided that you specify the size of the array in bits. However, when allocating memory this way you must always free the memory via `free()` when done:

```c
free( X );
free( Y );
free( Result );
```

We also want to use `malloc()` because there are limits to the size of an array declared in the traditional way via `TYPE[N]`--due to system limitations of the size of an arrays that can be allocated on the stack, and we will definitely be exceeding this limit. Before proceeding to the next section, study `test_iaxpy.c`. Do the following:

* Create a test program for `fdot` from `test_iaxpy.c`, and make appropriate targets for it in the makefile.
* Repeat for `dgemm`. Note that when allocating the arrays for `dgemm` that it is n^2 so your need to modify your allocation as follows: `(double *) malloc( N * N * sizeof(double) )`.

### Part 2 - Benchmarking

Now to the benchmarking. You can use the `time` command to time the performance of the benchmark. For the input size N, we want some arbitrarily large value so that we can really see the difference in run times for varying instruction types. When running any experiment, you want to run it *at least three times* and take the average, so we use a bit of scripting to call a timing operation on `./test_iaxpy.out` three times. Insert the following into the command line:<sup>d</sup>

```shell
$ for i in {1..3}; do time ./test_iaxpy.out; done;
```

This command runs the command `time ./test_iaxpy.out 200000000`, which, out of the box will run a `iaxpy` operation on vectors of size 200000000x1. On my own Dell Latitude E5470 laptop I get the following:

```shell
$ for i in {1..3}; do time ./test_iaxpy.out; done;
Running IAXPY operation of size 200000000 x 1
real    0m1.397s
user    0m0.937s
sys 0m0.434s
Running IAXPY operation of size 200000000 x 1
real    0m1.398s
user    0m0.884s
sys 0m0.504s
Running IAXPY operation of size 200000000 x 1
real    0m1.365s
user    0m0.887s
sys 0m0.476s
```

Recall from the text that real (wall) time includes the time that was spent by the operating system allocating memory and doing I/O. We want to focus on the user time. So, for `iaxpy` my Dell Latitude E5470 has an average of ~0.9 seconds. You should run this benchmark operation on `odin.cs.csubak.edu` for each of the three operations. *This means you must make benchmark programs for `fdot` and `dgemm` because they are not provided with the repo.* You should get faster results because I have a slower processor. You want to run the experiment many times because factors out of your control may skew you measurement. For example, there may be too many people running the same benchmark at that exact moment.

To determine what processor you are running via the command line execute:

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

### Mac

On Mac, `cat /proc/cpuinfo` does not work. To get the cpu information from the command line execute:

```bash
$ sysctl -n machdep.cpu.brand_string  
Intel(R) Core(TM) i7-5557U CPU @ 3.10GHz
```

## Check off

For check off, do the following:

* Show your version of the DGEMM test program to the instructor
* Aggregate your results into a table, and show your results to the instructor. It should look something like:

| Operation | `iaxpy` | `fdot` | `dgemm` |
| :--- | :--- | :--- | :--- |
| Albert's Dell Latitude E5470 w/ Intel Core i5-6440HQ | 0.771 | 0.790 | 4.110 |
| Albert's 2014 Macbook Pro w/ Intel Core i7-5557U | 0.925 | 0.836 | 12.776 |
| `odin.cs.csubak.edu` w/ Intel Xeon E5-2630 v4 |  |  |  |
| Local machine |  |  |  |
| My linux laptop |  |  |  |

Etc. etc.

## Footnotes

<sup>a</sup>Often used for thermal testing to see if you attached your heatsink properly.

<sup>b</sup>Actually, in some environments, if you enable optimization flags, the benchmark code will be audited out of the program entirely because the `for` loop is doing work on arrays that were never initialized, and the result is never used. However, we want the CPU to do the work. We do not care for the result because we are just measuring the arbitrary amount of time it takes a CPU to do the work.

<sup>c</sup>Integer arrays are important to test, even if unsophisticated, because they are 'normal' work, and designs have tended to favor floating point optimization at the expense of regular arithmetic.

<sup>d</sup>It is possible for you to do this within Make as well.
