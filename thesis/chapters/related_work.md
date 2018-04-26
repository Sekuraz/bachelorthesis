Running OpenMP programs on a \gls{cluster} is not a new idea.
It has been tried with various success several times in the past.
On the following pages the focus shall be on Cluster OpenMP \cite{comp}, ClusterSs \cite{clusterss}, 
OmpSs \cite{ompss} and XcalableMP \cite{xmp}.
All of those examples try to extend OpenMP with the capability to use more than one node of a \gls{cluster}.

# \comp
This was an approach to solve the exactly same problem as this thesis by expanding the intel compiler.
There is one major difference, in 2995, when \comp was designed, tasks were not yet a part of \omp.

## Execution model
\comp extends \omp by explicitly sharing memory with other nodes and then offloading chunks of \omp loops to those nodes.
A list of nodes and the number of processes on those nodes has to be supplied during startup.
The \omp standard is extended by the functions needed for the memory sharing and a \#pragma intel omp sharable
which also marks a variable for allocation in shared memory.

## Memory model
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

##  Discussion
In the end \comp is a system which provides a virtual global address space with some optimisations for \omp.
This conflicts with the scalability requirement because all processes have to communicate with each other on
barriers and if pages which are no longer in use are not reclaimed fast enough they can take up all the memory on
one node.
It also needs some porting effort because every malloc or free call has to be replaced and checked for issues.
Furthermore the cluster has to specifically set up for \comp in order to run a \comp program on it.