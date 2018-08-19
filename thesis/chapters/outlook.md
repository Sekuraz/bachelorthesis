# Scheduler
Introducing a scheduler will benefit the runtime because tasks which need the same memory can be scheduled together,
dependency tracking can be improved and several task which do not use all threads on a node can be efficiently executed
concurrently.
This is especially relevant for tasks which do not use internal \omp because then almost every core of a node is idle.
A proper scheduler could also reduce the number of nodes needed for a program if it utilises the remaining nodes better.
Furthermore a scheduler could provide metrics and insights into a running program or statistics for optimisation.

# Internal \omp
Using \omp within a task is possible but there is no support for it at the moment.
There are several points here, the first one is reserving cores which are used by \omp directives in order not to have
more than one task which tries to execute on those cores concurrently.
One more is to report the used cores to the scheduler in case the code for the task is executed several times.
This would further increase the level of parallelism and improve the utilisation of the nodes used to run the program.
It would also allow for a more \omp like use and feel for the programmer because he is no longer restricted to tasks
only.
In the end this idea combined with the scheduler can lead to large performance gains if the hardware utilisation
increases.

# Memory alignment
Currently the assumption is that memory is always allocated in 32 byte chunks and because of this the memory is also
aligned that way after it is transferred.
If there are pointers which point to the middle of an alignment the alignment is currently destroyed because the
transferred memory starts at the pointer and is aligned.
This is important for vector instructions and inline assembler because they require memory to be loaded in chunks
at the right borders otherwise the program crashes or delivers incorrect results.
There was no solution on how to get the current alignment of the data, but there might be a one in the future.

# Transfer structures with pointers
Currently only one level of pointers can be transferred because there is no difference between data and pointers once
the capture is calculated.
This also leads to problems with multidimensional arrays or structs and classes which contain pointers.
In order to solve this issue there has to be something similar to a virtual global address space, otherwise those stored
pointers would have to be rewritten or become dangling.
A virtual global address space introduces many more issues like where to store the stack of the currently executing task
and how to keep track of changed memory.
A solution similar to the one in \comp might work but is out of scope for this thesis.

# Handle open files and other system resources
Because a task can be offloaded to another node with another kernel, kernel resources like open files can not be
transferred.
Any attempt to access a file or window handle on a different node will lead to a segmentation fault or undefined
behaviour.
Currently these operations are not supported and prohibited for a correct program.
Future implementations might support this in order to allow a more natural look and feel for a programmer.
This is a very complex topic because some resources might not be available on other nodes or have a different name.
Because of this it is the last item on this list and might not be implemented at all.