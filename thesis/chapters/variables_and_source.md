# Variable Extraction
In order to run a task on a different node without a global address space, all memory used by the task has to be
transferred to the other node prior to execution of the task.
But before the memory can be transferred it has to be located and the size has to be calculated.

## Determine the Location
During the \gls{tp} phase the variable name is known as is the amount of pointer dereferences needed to find the actual
memory.
There is code being emitted to calculate the location of the memory during run time and the access type
\footnote{See paragraph \ref{access-clauses} for further details about access types and how they are used.}
is stored with it.
In order to save all this data there is a struct in the tasking header which stores the name of the variable, the pointer
to the beginning of the associated memory on the local machine, the access type and the size of the memory location
on the local machine.
All this data is used by the runtime to transfer memory and for dependency tracking of tasks.
The size is either known at the \gls{tp} phase, e.g. for objects, structs and builtin types or calculated at runtime,
which will be discussed in the next chapter.
To mark variables for runtime size calculation the size should be given as \texttt{0}.
There is a \texttt{prepare} method for the task which iterates over all variables and calculates their size if they are
marked that way.

## Determine the Size
Some sizes can be determined at the \gls{tp} phase as detailed before.
But some others can not, especially dynamically allocated arrays, which are an important part of matrices and other data
structures often used in \glsfirst{hpc}.
Currently pointers are treated the same way as arrays because it is not certainly known to the \gls{tp} whether or not they refer
to a single instance or to an array.
Furthermore memory can be allocated on the heap and on the stack of the currently running program.

### Heap
Memory allocated on the heap of a program is usually allocated by using some variant of \texttt{malloc}.
In order to determine the size of the allocation one has to rely on the information provided by the allocator and thus
we assume the default \texttt{glibc} allocator is used.
There is a function within \texttt{glibc}\footnote{See \cite{glibc} for further information about \texttt{glibc}.}
to retrieve the usable size of a given pointer, but naturally it only works
with pointers allocated on the heap and only with those returned by \texttt{malloc}.
Other pointers might lead to a return value of \texttt{0} or a segmentation fault.
\image{memory_layout.png}{The memory layout of the heap with the default allocator of glibc}{\cite[malloc/malloc.c~l.1088f]{glibc}}
So the algorithm first determines whether or not the variable is on the heap and if it is, it tries to call
\texttt{malloc\_usable\_size}, the function mentioned above, in order to get the actual size.
If the pointer points into the middle of an array the return value is \texttt{0} and the pointer is decreased by
\texttt{1} and the call is repeated until either the size is returned or an error occurs.

### Stack
If a variable is within the stack the stack size limit of the platform is used as the allocated size.
Variables on the stack have no size what so ever and thus the whole stack frame is transferred to the other node.
This happens even if some variables from the same stack frame are used and thus the same frame might be transferred
several times.
The surplus memory should not be an issue because the code should only access valid memory, it has no reference
to the surplus memory and if it runs into it, it would also run into problems in a single node scenario.

The actual code which is used in the project can be found in the tasking header in chapter \ref{tasking-header}, the
function is called \texttt{get\_allocated\_size}.

# Source Code Extraction
There is one thing missing from the parsing side, the source code extracted from a task.
The location of this source code also forms the so called code id, the identifier of the code associated with a task.
It is hashed and then used to look up a generated function which unpacks the variables and contains the extracted source
code of the application.
This source code itself might have been changed previously, but contained tasks are currently not properly transformed
due to the way the source code is traversed in clang.
This code is then stored in a temporary file in the \texttt{/tmp/tasking\_functions} directory with in one file
per processed source file.
In order to include this in the final application there is one header to collect all of it, which is named
\texttt{/tmp/tasking\_functions/all.hpp}.
This is in turn included by the tasking header.
One the one hand this architecture is simplistic and easy to use, it poses huge problems for large applications on the other
hand.
It pulls all tasks with every compilation unit and it might lead to naming conflicts if there are several objects files
being linked together.

## `tasking_function_map`
In order to use all of the extracted sources they have to be found during the run of the application.
Thus all the headers mentioned earlier generate the \texttt{tasking_function_map}, which is a global map from code ids
to the actual function.
It is populated using static evaluation before the main method begins in order to make all functions available for the
runtime.
