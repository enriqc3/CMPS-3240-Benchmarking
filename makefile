# This is a comment

# Reasons to use a makefile:
#	1) Saves having to repeatedly enter the same command. e.g.
#		Instead of: gcc -o hello.out -c hello.o other.o ...
#		Type it once in a makefile and: make hello.out
#		Easier.
#	2) You can have some advanced scripting going on with a makefile

# The first part of a makefile should contain, by convention, any variables 
# you intend to use. You might wonder, why do I need variables in my makefile? 
# Because it provides a convienient place to change things. For example, let's 
# say you want to use clang instead of gcc. You would just create a compiler 
# variable and change it in this central place. *Don't actually change this to 
# clang, that was just an example. Leave it as gcc.*

# This is a variable called CC that will hold the compiler we intend to use
CC=gcc

# This is a variable called CFLAGS that holds the flags we want to pass to the 
# compiler. Note that when you declare a varaible that it counts white space, 
# so CFLAGS literally is "-Wall -O0"
CFLAGS=-Wall -O0
# See the lab manual for an explanation of what these flags do.

# This is the end of the variable part of the make file. What follows are 
# called targets. Targets in a makefile follow this syntax:
# <name>: \t <dependency 1> <dependency 2> <dependency 3>
# \t	<Command line operation>
# makefiles are white and tab space sensitive. The \t's are tabs. The <name> is 
# the name of the target. It is followed by a list of dependencies, and 
# dependencies are not required. When compiling a target, make will check to 
# make sure there have been no changes to the dependencies, and that the 
# dependencies exist. It will first look for a file by the name of the 
# dependency. If there is no change, it will execute the current target. If 
# there is a change, it try to look up another target by the name of the 
# dependency and execute that instead. The <command line operation> is executed 
# after first checking the dependencies. Algorithmically, this is a sort of 
# DFS-like checking, followed by the execution of the <command line 
# operation>
#
# make would then be executed via the command line, given the identifier of 
# the target you want to compile:
# $ make <name>
# 
# There are also the following short-hand variables to help when writing your 
# <command line operation>. These are called *automatic
# variables*:
#	$@: <name>
#	$*: <name>, except the file extension (if there is one) gets trimmed
#	$<: <dependency 1>, and only the first dependency
#	$^: All of the dependencies, duplicates removed
#	$+: All of the dependencies, with duplicates
#
# If make is called without any arguments, it will by default execute the first
# and only the first target in the file. By convention (that is, it is not 
# required but strongly recommended) this first target is called 'all' and it 
# should contain the targets to the executable files that should be compiled
# by default.
# 
# Here: myblas.o is a compiled 'library' of sorts and has no executable file. 
# 'test_iaxpy' is a binary that should be linked with 
# myblas.o.
all: test_iaxpy.out

# By convention, there should also be a 'clean' target which will delete all 
# .o and .out files. The -f flag is there to force the removal of files, and 
# will suppress warnings if clean is called before compiling anything.
clean:
	rm -f *.o *.out

# The first actual target. Note that there are two stages to 
# compiling a program: (1) compiling it and (2) linking it.
# Compiling it produces a .o file, and linking takes all the .o files to create a 
# binary file (.out in this case).
test_iaxpy.o:	test_iaxpy.c
	${CC} ${CFLAGS} -c $< -o $@
test_iaxpy.out:	test_iaxpy.o
	${CC} ${CFLAGS} -o $@ $<