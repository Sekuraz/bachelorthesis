# Motivation

\omp \cite{openmp45} is an API for relatively easy parallelisation of C, C++ and Fortran code. 
Because of this it is widely used in the scientific computing to speed up simulations.
One problem when using OpenMP is that it requires a shared memory environment.
This means \omp can only be used when the program is running on one node at a time and is limited to the resources
available on this single node.

Building large systems with shared memory is expensive because one needs specialized and expensive hardware.
Spending the same amount of money on a \gls{cluster} almost always yields better overall performance.
The problem is that programming for those \glspl{cluster} is usually much more complex due to differences
in the way threads and processes share data.
In \omp data common to all threads is shared implicitly because every thread can access the memory of the whole system.
That is the reason why \omp needs a shared memory system.
On the other hand there are \glspl{cluster} which consist of several mostly identical nodes with a communication
system between them.
There is no shared access to all of the memory and accessing memory on another node needs some form of communication.
Because of this moving to a \gls{cluster} would require to a redesign of the whole application because changed memory on
one node does not appear on another node and synchronizing memory is slow.

There have been several approaches to solve this problem before, some of which will be discussed later, and below you
can find a still actual graphic made by Intel for their solution from 2006.
\image{expenses.png}{Cost comparison by Intel in 2006}{https://software.intel.com/en-us/articles/cluster-openmp-for-intel-compilers}
This approach took \omp work packages and distributed it to other nodes.
However, the performance gain was apparently not sufficient and the project was discontinued \cite{comp}.

Other projects are still there and being actively developed, but all of them have their own drawbacks, which made 
them unfit for the intended purpose of this thesis.

# Requirements

The proposed system has to fulfill the following requirements which originate from the day to day work of a programmer
who wants to run his software on thousands of nodes but does not have the time or the resources available for a full
rewrite of the software with some distributed memory \gls{api}.

## Use \cpp as the base language \label{req:cpp}
A large part of today's high performance code is written in \cpp and rewriting it in a special \gls{hpc} language 
like Julia\footnote{A specialized language for \gls{hpc}, see \cite{julia} for more information about the language.}
or Regent\footnote{Another specialized language, see \cite{regent} for more information.} is not possible because the
code was written years ago by students or employees who are no longer available.
Because many of those programs already use distributed memory parallelism with \omp this lead to the next requirement.

## Use \omp as a frontend to the developer \label{req:omp}
\omp is widely known by the target audience of this project and has a stable \gls{api}.
There was also the idea to do no changes to the \omp \gls{api} because this would allow the program to compile for a
single node only with \omp without changes to the source.
\omp also allows incremental parallelisation of already existing \cpp source code which is extremely important for an
existing codebase. 

## Scalability \label{req:scale}
There are some solutions like XMP or ClusterSS which achieve almost all of the above but which do not scale on large
clusters.
That is why this requirement was introduced.
The solution has to scale up to thousands of nodes and it must allow to use the memory on those nodes independently and
thus not synchronize the memory from the main node nor use a virtual global address space.
This is the requirement which excluded the existing solutions from the list as discussed in \ref{cap:2}.

