
There are several ways how to transfer or offload a task to another node in a \gls{cluster} and fulfill the 
requirements\footnote{The requirements can be found in paragraph \ref{requirements}.}.
Somehow the source code of the application has to be interpreted differently than in a standard compiling approach 
and there has to be some runtime to keep track of all the tasks in the system.

# Extend the OpenMP task \gls{api}
The straight forward approach to add something to OpenMP would be to write our own extension to the OpenMP standard
and implement it. 
There are some problems with this approach, the larges one is that there is no canonical implementation of the standard
because every compiler which supports OpenMP has a specialized implementation of it.
This would limit the use of this work to only those programs which can be compiled with this compiler.
Furthermore writing compiler extensions is not an easy task and would require a complicated setup of the compiling
infrastructure, at least for the evaluated compilers, \footnote{The evaluated compilers were gcc and clang.}
and is thus out of scope for a bachelor thesis.

# Build a simple \gls{tp}
A \gls{tp} takes the source code of a program and changes some parts.
One example would be a translation between JavaScript and Python or in this case from OpenMP tasks into something 
the runtime can use to run the same code on another node. 

The simple approach was to use a python script to transform the task constructs, but this approach soon proved to be
unable to deal with some C++ specific issues.
The first one was macro expansion which could add unbalanced braces and thus rendering the extraction of the associated
code for a task impossible.
Another problem was that dealing with comments and line continuation is a tedious task.
Thus it is almost impossible to reliably determine which parts of the program should be transformed without using a 
full C++ parser in python. 
When this was clear, evaluation of clang as a parser was the way to go because it provided at least some kind of python
bindings, but they proved not to be sufficient.

# A clang tooling based \gls{tp}
In order to circumvent all those problems it was decided to use one of the existing C++ parsers and, because it is
relatively easy to build programs with it, clang and the llvm backend 
\footnote{More information about clang and llvm can be found at \cite{llvm}.}
was chosen.
There one can hook into the code parsing and rewrite parts of the code on the fly.
In this code there is a method which is called whenever the traversed code encounters a \omp task directive and then
the necessary headers are pulled in and the task is rewritten.
Furthermore the main method is rewritten in order to set up and tear down the runtime properly.
The code associated with the task is extracted and stored in a globally accessible map in order to let the runtime
find the code again later. 
Task clauses are either evaluated or prepared for evaluation and then attached to a task struct which is defined in the
header\footnote{The header can be found in paragraph \ref{tasking-header}.}.

In order to transfer a variable to another node in a \gls{cluster} one has to determine the size of the memory the 
variable references.
This should be done during the transpiler phase, because determining the size of a variable at runtime is a error
prone and not intended in C++,
especially if the variable is a pointer or an array, or even worse a pointer into an array.
In the end this also leads to a real drawback, no struct, object or array which contains pointers can be sent using
the current method.
It was not possible to mirror all memory on every node, because this would violate the scalability requirement as
detailed in \ref{req:scale}.

Currently the \gls{tp} uses two pass evaluation, on the first pass the rewriting of the source code takes place, the 
second pass is used for all task extraction routines.
This split is made because all constructs are traversed in order, so the associated code of a task is extracted before
it can be processed itself.

