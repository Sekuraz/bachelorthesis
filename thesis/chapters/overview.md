# Implementation Ideas

There are several ways how to transfer or offload a task to another node in a \gls{cluster} and fulfill the 
requirements\footnote{The requirements can be found in paragraph \ref{requirements}.}.
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

# Actual implementation

## \gls{tp}
In order to circumvent all those problems it was decided to use an existing C++ parser and, because it is relatively
easy to build programs with it, clang and the llvm backend \cite{llvm} was chosen.
There one can hook into the code parsing and rewrite parts of the code on the fly.
In this code there is a method which is called whenever the traversed code encounters a \omp task directive and then
the necessary headers are pulled in and the task is rewritten.
Furthermore the main method is rewritten in order to set up and tear down the runtime properly.
The code associated with the task is extracted and stored in a globally accessible map in order to let the runtime
find the code again later. 
Task clauses are either evaluated or prepared for evaluation and then attached to a task struct which is defined in the
header\footnote{The header can be found in paragraph \ref{tasking-header}.}.

## Determine the size of variables
In order to transfer a variable to another node in a \gls{cluster} on has to determine the size of the memory the 
variable references.
This should be done during the transpiler phase, because determining the size of a variable in C++ is a tedious task,
especially if the variable is a pointer or an array, or even worse a pointer into an array.
In the end this also leads to a real drawback, no struct, object or array which contains pointers can be sent using
the current method.
It was not possible to mirror all memory on every node, because this would violate the scalability requirement as
detailed in \ref{req:scale}.
