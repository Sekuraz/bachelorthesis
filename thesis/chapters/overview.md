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