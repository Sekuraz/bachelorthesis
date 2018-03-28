# Motivation

OpenMP \cite{openmp45} is an API for relatively easy parallelisation of C, C++ and Fortran code. 
Because of this it is widely used in scientific computing to speed up simulations.
One problem when using OpenMP is that it requires a shared memory environment.

Building large systems with shared memory is expensive because one needs specialized and expensive hardware.
Spending the same amount of money on a \gls{cluster} almost always yields more overall performance.
The problem is that programming for those \glspl{cluster} is usally much more complex due to differences
in the \gls{api}.
On top of that moving to a \gls{cluster} would require to redesign the whole application because many algorithms 
must be changed when they are no longer used in a shared memory environment.
This poses challenges for scientific code especially because much of it is rather old and the original developer
might not be around anymore and many scientists are not experienced in writing highly parallel code for \glspl{cluster}.

In order to address some of the problems which where presented, this thesis aims to provide a simple, yet powerful
way to harness some of the benefits of \glspl{cluster} and avoid as many drawbacks as possible by offloading OpenMP
tasks to other nodes in a \gls{cluster}. 
OpenMP tasks are similar to threads and thus should be able to be transferred to another node in a \gls{cluster} 
easily and with limited effort.
This approach might provide a large speedup and poses no risk because the program can still be run on a shared memory
system without any drawbacks. 

# Requirements

The proposed system has to fulfill some requirements.
Some were proposed by our supervisor, others came up during the development process.
The most important ones were:
- The solution should scale for large \glspl{cluster} with 10000 nodes and more.
- The syntax should stay as close as possible to the default OpenMP tasking syntax.
- The application should be able to allocate more memory than there is on a single node.
- Transfer the needed memory to the other node and keep the alignment of the data.

<!-- TODO -->

# Implementation Ideas

There are several ways how to transfer or offload a task to another node in a \gls{cluster} and fulfill the requirements
from above.
All of which require some form runtime to keep track of all the tasks in the system.

## Extend the OpenMP task \gls{api}
The straight forward approach to add something to OpenMP would be to write our own extension to the OpenMP standard and
implement it. 
There are some problems with this approach, the larges one is that there is no canonical implementation of the standard
because every compiler which supports OpenMP has it's own implementation of it.
This would limit the use of this work to only those programs which are compiled with this compiler.
Furthermore writing compiler extensions is not an easy task and is thus out of scope for a bachelor thesis.

## Build a \gls{tp}
A \gls{tp} takes the source code of a program and changes some parts.
One example would be a translation between JavaScript and Python or in this case from OpenMP tasks into something 
the runtime can use to run the same code on another node. 

There are two different approaches to this problem.
The first one was to use a python script to transform the task constructs, but this approach soon proved to be unable
to deal with some C++ specific issues.
The first one was macro expansion which could add unbalanced braces and thus rendering code extraction impossible.
Another one is the fact that the amount of valid C++ programs is defined to be all the programs a valid C++ compiler
compiles.
Thus it is almost impossible to reliably determine which parts of the program should be transformed without writing a 
full C++ parser in python. 

<!--
ist OpenMP?
Wozu wird es verwendet?

- parallel computing
  - loops
  - tasks
  - GPGPUs/Accelerators
- easy to use
- shared memory

Limitations of shared memory systems

- Number of CPU cores/FLOPs is limited or very expensive
-->