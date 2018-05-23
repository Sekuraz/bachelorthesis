Running OpenMP programs on a \gls{cluster} is not a new idea.
It has been tried with various success several times in the past.
On the following pages the focus shall be on Cluster OpenMP \cite{comp}, ClusterSs \cite{clusterss}, 
OmpSs \cite{ompss} and XcalableMP \cite{xmp}.
All of those examples try to extend OpenMP with the capability to use more than one node of a \gls{cluster}.
The execution and memory models are taken from the sources given in the introduction chapter of each example.

# Cluster OpenMP
This was an approach to solve the exactly same problem as this thesis by expanding the intel compiler
\footnote{The manual and other references to \comp can be found at \cite{comp}.}.
There is one major difference, in 2006, when \comp was designed, tasks were not yet a part of \omp.

### Programming model
\comp extends the \omp syntax slightly and can be used like the standard C and C++ \omp.

### Execution model
\comp extends \omp by explicitly sharing memory with other nodes and then offloading chunks of \omp loops to those nodes.
A list of nodes and the number of processes on those nodes has to be supplied during startup.
The \omp standard is extended by the functions needed for the memory sharing and a \#pragma intel omp sharable
which also marks a variable for allocation in shared memory.

### Memory model
Memory in \comp is either shared between all nodes or private to a node.
Shared memory has to be created by calling a specialized malloc function provided by \comp.
This marks all touched memory pages as shared and allows other processes to access these memory pages.
The pages are mapped in all processes at the same virtual memory location and are protected against reading and writing
there.
If the program tries to access one of those protected pages \comp catches the resulting segmentation fault and copies
the most recent version to that location.
Writes to the page are recorded and saved in write notices.
Every time the program has to synchronize the state of the pages is exchanged and when the log of write notices gets
too long the processes agree on which version of the page is the most recent one.
This version is kept and all others are discarded and protected again.

###  Discussion
In the end \comp is a system which provides a virtual global address space with some optimisations for \omp.
This conflicts with the scalability requirement because all processes have to communicate with each other on
barriers and if pages which are no longer in use are not reclaimed fast enough they can take up all the memory on
one node.
It also needs some porting effort because every malloc or free call has to be replaced and checked for issues.
Furthermore the cluster has to specifically set up for \comp in order to run a \comp program on it.
On the other hand this system poses almost no restrictions on the user and thus is not really difficult to adopt.

# ClusterSs
ClusterSs is a programming model which makes use of tasks.
It was designed in  2011 at the Barcelona Supercomputing Center.
More details about the system can be found at \cite{clusterss}.

### Programming model
Java is the language ClusterSs was designed for and it needs annotations to those java sources, mainly all parameters
are annotated as well as some methods and even classes.
There are bindings for the X10 language\footnote{X10 is a language specifically designed for \gls{hpc}.} too.

### Execution model
ClusterSs uses a main node where the application itself is run and a set of worker nodes which run tasks.
Tasks in ClusterSs are functions with a special designation which marks them for being run on a different node.
The dependency graph is built during runtime and tries to imitate a sequential run of the program.

### Memory model
Memory is transferred to the worker running a task on request, which means the workers collect the needed data from 
the main node or other workers.
In order to make more memory available on the main node, variables can be allocated on nodes.
The node with the most data available in it's cache is chosen to run any given task in order to decrease the amount of
memory transfers needed to run a task.
Writing always takes place on copies of the data.

### Discussion
At the first glance this seems to be exactly what we want, but there is one major drawback to this model.
Most \gls{hpc} programmers are not used to write programs in java and especially not with annotations in java.
Furthermore most existing \gls{hpc} codebases are not written in java and can not be rewritten within a acceptable time.
Besides that this example provided some insight into how this thesis might evolve, especially when it comes to 
scheduling and automatic memory transfer.
On the other hand writing only to copies of the data might be a problem if the copy is a large structure.
But for the runtime for this project no other solution could be found.

# OmpSs
OmpSs was created by the same group as ClusterSs, the Barcelona Supercomputing Center.
It is a improvement over ClusterSs and it uses \omp syntax extensions to reach it's goals.
Further information can be found at \cite{ompss}.

### Programming model
OmpSs is a syntax extension to \omp and compatible with the C, C++ and Fortran \omp versions.
It uses a transpiler which translates the new directives into something a standard compiler understands.
Annotations like \texttt{\#pragma omp task} can not only apply to code blocks but can also be used to indicate that
functions should always run in a task by annotation of the function declaration.

### Execution model
This example uses a model similar to the one which will be presented in this thesis. 
A thread starts as the master thread in which the first tasks are created. 
Later on other tasks may itself create tasks which might be executed on different nodes.


### Memory model
Memory is seen as a partitioned space and only local memory can be used by a node.
It is transferred when a new task is beginning execution and no deep copies are made. 
Because of this no pointers are allowed in the offloaded data.

### Discussion
This example almost fulfills the requirements, but the vast amount of syntax extensions was deemed too much to neatly
fit with the standard \omp requirement.
For example every variable which enters into a task has to occur in a dependency clause to the task construct and other
requirements which do not allow a standard \omp program to directly run on a cluster.

# XcalableMP
\cite{xmp}
