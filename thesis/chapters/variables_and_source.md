# Data structures
In this thesis there are two major points where data is stored.
One point is the \gls{tp} itself, which has to keep the state of the currently processed source file, the other is
the time when the transformed program gets executed and needs values computed during the \gls{tp} phase.
Those values are stored in structs which have been designed during this thesis and are being introduced in the next
sections, they are then written to the source code of the transformed program where they are compiled into the final
application.

## Task struct
The main data structure within this thesis is the one representing a transformed task, the data structures representing 
the queue of scheduled tasks and the representation of nodes and running tasks is part of the runtime.
In order to store all relevant information about a \omp task and allow passing tasks between nodes there has to be 
a structure representing a task\footnote{The exact source code can be found in section \ref{tasking-header}
at line 93.}.
It contains the id of the code section associated with the task, the values for the clauses and the prepare and schedule
method which are used to make sure that the task is prepared for transferring to another node and is scheduled for
execution respectively.
The id of the code section, which is the hash of the file name, the line number and the location of the \omp
task directive hashed together, is used as the "code id" in the remainder of this thesis.

## Var struct
Variables used within a task are represented in another data structure, the Var struct\footnote{The exact source code
can be found in section \ref{tasking-header} at line 86.}.
It contains the variable name, it's address, the access type\footnote{Either \texttt{shared, firstprivate, private} or
\texttt{default}, see section \ref{access-clauses} for more information about variable access clauses.} 
and the size in bytes of the variable.
The size is set to \texttt{0} if the size is unknown during the \gls{tp} phase, the size is then calculated during the
prepare method of a task by the method explained in section \ref{determine-the-size}.

## `tasking_function_map`
In order to use all the extracted tasks, or better the code they contain, they have to be found during the run of the
application.
Thus all the code is extracted and a global map called \texttt{tasking\_function\_map} is created which associates the
code id and the actual instructions. 
This is done as a easy method of making that code available on other nodes and thus allow the tasks to roam and use
an identifier to find the relevant code to execute.
The map is populated using static evaluation before the \texttt{main} method of the transformed program begins in order
to make all functions easily available for the runtime.

# Clang \glsentrytext{api}
Within clang tooling there is an \gls{api} for source tree traversal using the clang lexer and parser.
But first one has to create a \texttt{CompilerInstance} and determine the flags with which it was called.
This step is currently implemented in a way that the \gls{tp} parses arguments after a double dash as if they were given
to the clang compiler.
This feature also allows the use of the \gls{tp} in larger projects where not all needed files, e.g. headers, are trivial
to find.
So if you run \texttt{make} with the compiler set to the \gls{tp} created during this thesis followed by a double dash
and then run make again with the actual compiler it should be able to transform all the source code.
The next thing to do is to find the file which should be transformed.
It is either found as the first parameter to the \gls{tp} or pulled from the invocation line mentioned earlier.
Afterwards the file is parsed into a syntax tree which is then traversed by two different visitors.
The first one is rewriting the source, the second one is extracting and inserting C++ statements.
Rewriting is done before extraction because the nodes in the syntax tree are visited from top to bottom so code which
is within a changed block could otherwise be extracted before a proper rewriting took place.

# Extraction of tasks
\image{AST.png}{The syntax tree representation of the \omp task in the test program.}
{See section \ref{test.cpp} for the example code.}
In the clang \gls{api} there are methods which can be implemented for different kinds of tokens.
The main work is done in the \texttt{VisitOMPTaskDirective} method of the \texttt{ExtractorVisitor}, there all the
clauses and variable captures are extracted and a first try to determine the size of those variables is made.
In figure \ref{fig:AST.png} the textual representation of a task with all clauses can be seen.
The \texttt{CapturedStmt} represents the code within the task and which should be offloaded to another node.
Clang also allows to go back to the source from this syntax tree representation and shows the variables captured within 
the code block.

## Extraction of clauses
The values for some clauses are evaluated when a \omp task is encountered during the run of the program\footnote{The 
\texttt{if}, \texttt{final} and \texttt{priority} clauses.}.
This means they can not have a value assigned to them during the \gls{tp} phase, so the statement string within the clause
is extracted and written into the transformed program in order to be evaluated when the task struct is created during 
the run of the program.
Another clause exists in order to track dependencies between tasks, in the \texttt{depend} clause, which was discussed in 
section \ref{depend-clause}, input and output dependencies are added as a list of variable names.
These names are used by the runtime to track dependencies between tasks.

In the following lines there is a small \omp task definition, with which the generation of task structures is shown.
The code within the task is not handled yet.
 
```c++
#pragma omp task\
    untied\
    mergeable\
    if(i == 3)\
    final(i == 5)\
    depend(in: a)
```
The following code section is the transformation of the original source above. 
```c++
Task t(7574561021973165946ull);
t.if_clause = (i == 3);
t.final = (i == 5);
t.untied = true;
t.mergeable = true;
t.in.push_back("a");
t.schedule();
```
All clauses are reordered according to the order in which they are introduced in the \omp specification and then added to
the task struct.

## Variable extraction
In order to run a task on a different node without a global address space, all memory used by the task has to be
transferred to the other node prior to execution of the task.
But before the memory can be transferred it has to be located and the size has to be calculated.
As already said some part of the calculation is done during the run of the transformed program, this is addressed in 
section \ref{variables-at-runtime}.
During the \gls{tp} phase the only task is to discover all used variables, which clang takes care of and determining
how many dereference operators are needed in order to get the actual data.
This is done by undoing all \texttt{typdef} type declarations to get the native type and how often a it has to be
dereference in order to get the actual data.
```c++
int* a = new int[AS];
int b[AS];
int i = 0;
int* p = &i;
```
The variables in the example code above are transformed into the following code at task creation.
```c++
Var a_var = {"a", &(*a), at_firstprivate, 40000};
t.vars.push_back(a_var);

Var i_var = {"i", &(i), at_firstprivate, 4};
t.vars.push_back(i_var);
    
Var p_var = {"p", &(*p), at_firstprivate, 0};
t.vars.push_back(p_var);
```
Here one can see the different amount of dereference operators according to the type of the variable.
Variables are then wrapped in the correct amount of references again before the extracted code starts executing.
The code which unpacks the variables above can be found below.
```c++
void * a_pointer_1 = arguments[0];
void * a_pointer_0 = &(a_pointer_1);
int * a = *((int **) a_pointer_0);
void * i_pointer_0 = arguments[1];
int i = *((int*) i_pointer_0);
void * p_pointer_1 = arguments[2];
void * p_pointer_0 = &(p_pointer_1);
int * p = *((int **) p_pointer_0);
```

## Source code extraction
There is one thing missing from the parsing side, the source code extracted from a task.
The code is extracted by the preprocessor from the code during the \gls{tp} phase.
The location of this source code also forms the so called code id, the identifier of the code associated with a task.
It is hashed and then used to look up a generated function which unpacks the variables and contains the extracted source
code of the application.
This source code itself might have been changed previously, but nested tasks are currently not properly transformed
due to the way the source code is traversed by the clang library\footnote{It uses a top-down approach, so the outer 
task is encountered and transformed before the preprocessor knows about the inner one.}.
This extracted code is then stored in a temporary file in the \texttt{/tmp/tasking\_functions} directory in one file
per processed source file.
In order to include this in the final application there is one header to collect all of it, which is named
\texttt{/tmp/tasking\_functions/all.hpp}.
This is in turn included by the tasking header.
One the one hand this architecture is simplistic and easy to use, on the other hand it does not scale really well because
every source file with a \omp task pulls in the transformed code of every task within the whole application.
This has to be optimized away by the linker currently, but it might be an easy target for future work on the architecture. 

# Add the runtime to the original code base
In order to run the tasks on other nodes the runtime has to be set up on all those nodes and all but one of the nodes
have to go into a execution or management role which is not implemented in the original application.
This is done by inserting a function call for setup before the first instruction in the \texttt{main} method of the 
original program and one for teardown after the last statement of the \texttt{main} method or, if the last statement is 
the return statement, before the return statement.
Those functions should set up the runtime and should only return for one node, the others should change into runtime or
worker nodes.
Worker nodes are the ones executing tasks, the runtime nodes keep track of the distributed tasks, handle completion and
schedule the tasks for execution.

There are two other functions with which the runtime and the transformed program interact, the first is the \texttt{schedule}
method of the task struct and the \texttt{taskwait} function.
All those functions are currently not implemented, implementing them would be the task of the runtime programmer because
they are only called from the transformed program and integrate much tighter into the runtime than into the \gls{tp}.

# Variables at runtime
In section \ref{variable-extraction} the way variables are extracted from \omp tasks is discussed.
The largest impact of those variables is during the run of the program though, the address or location of the variable 
as well as the size can not be determined before execution because some data structures can be allocated according to
input to the program and their size can thus not be calculated during the \gls{tp} phase.
Furthermore the length of arrays defined in other source files is not known in C++.

## Determine the location
During the \gls{tp} phase the variable name is known as is the amount of pointer dereferences needed to find the actual
memory.
There is code being emitted to calculate the location of the memory during run time and the access type
\footnote{See section \ref{access-clauses} for further details about access types and how they are used.}
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

## Determine the size
Some sizes can be determined at the \gls{tp} phase as detailed before.
But some others can not, especially dynamically allocated arrays, which are an important part of matrices and other data
structures often used in \glsfirst{hpc}.
Currently pointers are treated the same way as arrays because it is not certainly known to the \gls{tp} whether or not they refer
to a single instance or to an array.
Furthermore memory can be allocated on the heap and on the stack of the currently running program.

### Heap
Memory allocated on the heap of a program is usually allocated by using some variant of \texttt{malloc}.
In order to determine the size of the allocation one has to rely on the information provided by the allocator and thus
we assume the default \texttt{glibc} allocator is used \footnote{The original \texttt{malloc} implementation details can
be found at \cite{malloc}.}.
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

The actual code which is used in the project can be found in the tasking header in section \ref{tasking-header}, the
function is called \texttt{get\_allocated\_size}.