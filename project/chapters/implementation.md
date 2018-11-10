
# Build system
In order to properly build the complex integration from section \ref{runtime} the build system for both existing parts
had to be modified, the preprocessor could not be built outside of the clang source tree and the runtime used a Makefile
for in source builds.
This system was not capable of a such tight integration, so a switch to cmake was made in order to control the behaviour
of the build system and allow for more modules than before.

## Preprocessor build system
The build in the clang tree was not abandoned because it had some advantages, namely the headers were there and all the
compiler macros which were needed to build it are present in the clang tree.
Furthermore there are some clang versions with which the preprocessor refused to build. 
So a working clang version was added as a dependency to the source tree\footnote{The version is the git commit with the
hash 537ae129b767ac40785b17328ba1aaca7e5f5ace from the $31^{st}$ of October 2018 in the official clang mirror repository
on github, \cite{clang-repo}.}.

The source directory of the scheduler is linked to the correct place within the clang tree by cmake and a normal build
of the whole clang tree is thus part of a normal build of the preprocessor.
After the clang build finished the resulting application is copied to the output directory of the cmake build.
The limitation here is that a reload of the cmake project requires a full rebuild of clang.
This has to be addressed in future work because there were much more important issues at hand, e.g. writing the 
scheduler or the runtime, and it can be mitigated by using \texttt{ccache} or a other caching solution.

## Scheduler build system
The scheduler is a component which should be easily changed depending on the system the code is about to execute.
In order to achieve this form of modularity it is built into a separate shared library, even if the sources are in the
same directory as the ones of the runtime.
Currently this library is not path agnostic, so it is hard to find it when it is not in the path where it was built.
This issue is shared with the runtime, but as mentioned earlier the build system was not the main target of this work,
it was only needed in order to deal with the complexity.

## Runtime build system
The runtime is built as a shared library with cmake and links together all the parts, it pulls in MPI and the
scheduler library.


# Runtime
The only two things which were planned to be done with the runtime were to add the memory transfer and writeback and the
integration with the preprocessor and the scheduler.
This soon proved not to work, after the implementation of arbitrary tasks the runtime did not work anymore.
After all debugging efforts were did not yield any tangible result and the original author of the code was unavailable
it was decided by the current author to write a new runtime.
Many parts from the old runtime were incorporated in the new design to speed up the development process, especially the
design was copied almost completely.

## Worker nodes
Worker nodes execute tasks on request by the runtime.
The memory required in order to run them is requested from the node on which the tasks was created and the difference is
written back to this node.
TODO

## Runtime nodes
There are not much changes from the original runtime nodes, in short the runtime still handles task creation and
distribution, the only new feature is the scheduler integration.
On the down side the rewrite of the runtime showed that running more than one task per node, which would mean a capacity
greater than \texttt{1} requires much more synchronization and locking so the capacity was set to \texttt{1} for each
worker node for now.
The implementation of taskwait was also not working and was scrapped in favour of memory transfer and write back.

## Communication
The new runtime system uses one task class on all endpoints, so there is no type conversion between the runtime, the 
workers and the preprocessed program anymore.
Some component of the task are not present on every endpoint though, some are hard to serialize, like the dependencies,
others maker no sense to synchronize, for example the thread in which the task is executing.

This communication in implemented using MPI and uses MPI tags to distinguish between different types of messages which
then lead to different handlers being called by the receiving routine of the runtime or worker nodes.
Every worker also has an associated runtime node where tasks are created and from which task runs are requested.
Incoming tasks on the runtime are handed over to the scheduler which is also running within each runtime node.

# Scheduler
Currently the scheduler is only a list of work items to do and due to the rewrite of the runtime not much work could be
put into a sophisticated scheduler.
The current algorithm searches for the worker with the most free capacity and schedules new tasks there.
For the test cases\footnote{See section \ref{the-original-source} for those.} the algorithm uses all capacity of a single node test
machine.
One issue with the scheduler is, that dependencies are not taken into account because they are most likely evaluated,
for example array indices are calculated and then applied.
Such a dependency might look like this \texttt{depends(out: A[i][j])} where \texttt{i} and \texttt{j} are 
variables in the outer scope. 
So this is a point which might also be a starting of future work.

# Integration
Due to the fact that the whole runtime system now uses one task representation the integration with the preprocessor
was rather easy.
The only change that was made were some adjustments to variable types and the handling of the main method, which was
completely changed.
Those variable types had to be changed because \texttt{void} pointers were used and indices of void pointers are not
defined by the C++ standard.

The main function of the old program is now not amended but extracted into a function named \texttt{__main__1}. 
Furthermore there is a function called \texttt{__main__}, it is executed as a task and is another function in order
to allow a proper freeing of the task resources even if the developer opted for a early return from his main function.
This special first task is created by the runtime itself during the setup and then scheduled as soon as it is running.

# Memory transfer
When a new task for execution arrives at a worker node, the first thing is to check it for the id of the
node on which the task was created.
If it is the same node as the one where the execution is about to take place only the pointer structure\footnote{The 
extracted tasks use a single \texttt{size_t**} as input and there is some code emitted to cast all of this to the
appropriate types used in the extracted task.} needed for the extracted task function is created,
otherwise a request for the memory is issued to the origin node which then responds with the number of
variables and their sizes.
Then they are transmitted in the same order as they were created during the preprocessor run.
Upon receiving the memory a backup is made in order to only write back the changes.
At last a array containing pointers to all transferred memory is created and then put into the created function for the
task.

This all means that the memory captured by other tasks any task must not be freed, otherwise it would not 
be available for the created children tasks.
This is a problem because taskwait instruction with which a task could wait for all its children are currently not 
supported.

After a task finishes the changes are calculated using the backup made during the transmission and then the changes
are sent to the origin node of the task.
The algorithm to write back the changes currently requires \texttt{16} bytes per changed byte because MPI only allows
for one datatype in one message so each address, which might be up to \texttt{8} bytes long, is joined with another
\texttt{8} bytes which only contain \texttt{1} byte payload.
So this is one point for future work, ideas here are first and foremost a much more efficient data transfer algorithm
and further down the line a copy on write approach for all transferred data so there is no need to keep it in memory
twice.

During the implementation of the memory transfer it was discovered that some memory might change rapidly, which means
it is gone or changed before it is requested by the node on which the task will run.
In order to mitigate this risk every variable which can be trivially copied is copied out. 
This applies mostly to loop counters and other simple types where the address is reused in every iteration.
