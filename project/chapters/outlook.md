# Runtime improvements
## Support for multiple tasks
Currently the runtime only supports one task per worker and supporting more than one would require some more on the 
runtime node and the scheduler.
This is considered to be low hanging fruit because it will be a great benefit for the resulting applications and only
a moderate effort.
The only part missing here is to prevent the scheduler from scheduling too many tasks on a node because a node might 
report capacity as free which was already requested by the runtime for a task.
This can be also used to support internal \omp, which means \omp structs in offloaded tasks.
In order to make this work every request for threads have to be intercepted by the preprocessor and the worker capacity
reduced by the number of threads requested by the task.

## Deallocate finished tasks
Currently the memory captured when a task is created is not made available again because the worker is not signaled when
every child task is finished as and it is unclear how to only free memory the task really owns contrary to the memory
which is shared between potentially running tasks.
This would require the worker nodes when all children of a task finish and potentially the taskwait structure to be
supported in order to allow the developers to free the memory when all children which might use it have already
exited.
In order to achieve this some more design work is needed, especially since there is neither support for paused or waiting
tasks nor resuming of such tasks at the moment.
This leads to leaking some memory in parent tasks which should be avoided for a production ready code.

## Memory alignment
For some code it is beneficial to use vector instructions which require a certain alignment of the memory.
This is currently not considered and might thus lead to incorrect results for those applications.
In order to get this to work some research into memory allocation is needed and the routines in 
\texttt{Worker::request_memory} have to be adjusted  to that.

## Transfer non trivial memory
Currently only memory which somehow resolve to a trivially copyable type can be transmitted. 
This memory can be plainly copied and then cast to the original type without losing information.
In order to support more complex programs it might be needed to move structures with pointers in them between nodes
so some kind of support for them would be necessary.

# Scheduler improvements
Currently the clauses which are used to configure tasks are not transferred to the scheduler from the node on which the
task was created.
This would be a first step for the for a larger project, which would be to take all clauses and apply them as close as 
possible to the \omp standard.
It includes the dependency tracking between the tasks as well as how to initialize memory and when to execute a task
right away instead of submitting it to the scheduler.

# Conclusion and outlook
in order to have a full product, support for multiple tasks, see section \ref{support-for-multiple-tasks}, and 
deallocation of finished tasks, see section \ref{deallocate-finished-tasks} is a prerequisite.
Furthermore the dependency tracking part of section \ref{scheduler-improvements} is also needed in order to allow
simple non artificial programs to run with this work.
The rest of this section will also be needed for larger applications and for performance improvements.
Those points will most likely be addressed by future work on the topic first.
