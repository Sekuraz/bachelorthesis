There is one main task construct in \omp, this is a 'pragma omp task' clause with it's clauses.

In \omp a task is generated when a program encounters a task directive. 
The goal of this thesis is to provide an environment and a \gls{tp} which automatically transfers such a task to another
node in a \gls{cluster}.
In order to do this correctly and consistently several clauses of an \omp task directive have to be adapted and 
transformed by the \gls{tp}.