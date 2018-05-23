# Variable Extraction
In order to run a task on a different node without a global address space, all memory used by the task has to be 
transferred to the other node prior to execution of the task.
But before the memory can be transferred it has to be located and the size has to be calculated.

## Determine the Location
During the \gls{tp} phase the variable name is known as is the amount of pointer dereferences needed to find the actual
memory.
There is code being emitted to calculate the location of the memory during run time and the access type
\footnote{See paragraph \ref{access-clause-transformation} for further details about access types and how they are used.}
is stored with it.
In order to save all this data there is a struct in the tasking header which stores the name of the variable