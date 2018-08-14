There are several limitations of this prototype which are mentioned scattered throughout the whole thesis.
They shall be collected here for a central reference.

# The runtime is not yet completed
As of this writing the runtime is not completed and thus no performance figures can be shown.
This is due to the cancellation of the partner thesis and will most likely addressed in another thesis.
For a code base with high usage of tasks a speedup should be observable.

# Nested pointers
The memory transfer works by identifying the memory which a pointer or a variable name references. If this memory contains
other pointers the memory they reference is not transferred.
This is caused by the way memory transfer is implemented, a explicit transfer can not include nested pointers, because
it is really hard to tell values and pointers apart in \cpp.

# Nested \omp tasks
Currently one \omp task per code block is allowed, because only the first one is extracted correctly.
So if there is an \omp task in the code block of another one, it is encountered after the extraction of the source code 
happened for the first task. 
This leads to the situation where the original code is still in use inside the first task and the second task is not 
scheduled for execution by the runtime at all.
This can be fixed by altering the traverse order in clang or changing the logic by which tasks are handled completely.
Thus there should not be any directly nested tasks, nesting them via functions is although supported.

# Stack limit might not be correct
The stack limit can be changed during runtime and it is thus not absolutely reliable for a variable size in the transfer.
But apparently there is no other way to get a fallback value for the stack size at all.
Another problem is that the stack size can be set to unlimited in which case every time all of the node's memory is
transferred.
One last problem with this approach is, that the page after the stack is usually protected and thus can not be accessed
without a segmentation fault, which needs to be handled somehow.



