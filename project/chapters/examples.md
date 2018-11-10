All the talk about current capabilities is best explained by a simple test program in the repository which shall be 
explained here.
Running a full stack, the preprocessor, then build the runtime into it requires some serious work before, so first there
will be some minor examples to show how the runtime and the scheduler work before a full example with the preprocessor
is given.

# The original source
The test program did not change from the previous work on the preprocessor, but for completeness it can be found below.
All the following examples are descending from this and use the output of various levels of preprocessing.

\bigskip
\lstinputlisting[language=C++, caption=Original source]{code/simple.cpp}

# One simple example
The following program is a copy and paste of all relevant files the preprocessor uses in order to build a full 
application.
It only supports one worker and one runtime node and was created when the new runtime did not support memory transfer, 
but it can be used to show how the preprocessor transformes files and how the control flow works.
This application ca be built by simply linking it to the runtime library.

\bigskip
\lstinputlisting[language=C++, escapechar=|, caption=Simple example]{code/run_many.cpp}

In lines \ref{1-start-tasking} to \ref{1-end-tasking} the task code is extracted and the function is made available 
in the global \texttt{tasking\_function\_map}. 
The lines \ref{1-start-memory} to \ref{1-end-memory} recreate the variable names and types from the input to the 
function in order to let the code in line \ref{1-code} access it the same way as if it was on shared memory.
In the following lines, from line \ref{1-start-task} to \ref{1-end-task}, the task is created according to the 
\omp declaration of the input in section \ref{the-original-source}.
The additional code in lines \ref{1-start-add} to \ref{1-end-add} only check whether the correct preconditions for a 
successful program run are given.
Finally, in the lines \ref{1-start-test} to \ref{1-end-test}, the correct execution of the tasks is checked.
The different main functions are used to contain the return at the end of the original main and the second wrapper is
used to free the worker and finish the main task properly.
The following code is the only thing needed to get this code to work, so it is not that difficult if the tasks are 
extracted.
The name \texttt{tdomp} refers to the runtime and is the abbreviation for Task Distribution \omp.

\bigskip
\lstinputlisting[caption=Simple example - build script]{code/run_many.cmake}

# A full example
Now the goal is to transform the code of section \ref{the-original-source} using all the parts put together.
This is done in the cmake file below.
Note that \texttt{simple.cpp} refers to the source of the example.
The preprocessor has to be built before.

\bigskip
\lstinputlisting[caption=Original source - build script]{code/simple.cmake}

Invoking \texttt{mpirun} on the final executable produced a running binary which executed in different processes and 
produced correct results during the write back.
All the additional clauses of the \omp task were ignored for this to work.