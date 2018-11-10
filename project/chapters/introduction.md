
# Motivation
Writing software for only one node is become increasingly pointless, especially in \gls{hpc} applications,
because processor clock speeds are increasing slower and slower and the number of cores per chip is also limited.
Thus waiting a few years or adding cores to one node is not as rewarding as it used to be several years ago.
In order to overcome this problem one can add more nodes.
More nodes usually requires introducing MPI\footnote{MPI, the short name for Message Passing Interface, is a library
and runtime to allow passing messages and memory between shared memory systems. More information can be found
at \cite{mpi}.} to the program and eventually a redesign of the whole application.
Furthermore it is not trivial to write a correct MPI program opposed to one which only uses \omp.

One idea to solve this problem is to take tasks from \omp programs and find a way to execute them on a different node.
So the target of this work is to present a preprocessor, a runtime and a scheduler to transform shared memory
\omp tasks to distributed memory tasks.
The former two already exist so the focus was on finishing those and writing a scheduler.

This work is a follow up on the two bachelor theses of Johannes Erwerle, \cite{jo}, and myself, Markus Baur, 
\cite{me}, where the target was to create a runtime and a preprocessor in order to distribute \omp tasks to
several nodes.
In order to bring it all together and to write a simple scheduler more work was needed, so the target of this
paper is to finish it up and bring all pieces together.

The final deliveries of the previous work contained a preprocessor which was able to rewrite \omp tasks into
a structure usable for offloading the work to another node, but the runtime lacked support for running arbitrary
tasks as well as moving memory between nodes.
Those two parts as well as the integration are the main targets of this work.

# State of earlier work
## Preprocessor state
The already existing preprocessor was in a somehow usable state, some types were not consistently applied throughout
the project.
On top of that the extraction and rewriting of the main function proved to be insufficient for integration with the 
runtime.
There was one more problem with the preprocessor, it can only be built within the clang\footnote{Clang is used to parse
the C++ code, provide the lexer and other tools to work with C++ source code. Further information about clang can be 
found at \cite{clang}.} source tree, which made it difficult to change the code and fix bugs.

## Runtime state
The runtime worked well with it's built in test cases, but it was difficult and error prone to add functionality.
Furthermore it lacked support for moving memory between nodes, and, on top of the one task representation from the 
preprocessor it contained two more, one for the runtime node and one for the worker nodes respectively.
It also used in tree builds with a make file which made it hard to tweak the build system for modularity and keep the
source directory clean.

During the course of this work the runtime proved to be not extendable, at least by the author of this work, so it was 
rewritten from scratch but copying most algorithms and the layout from the existing runtime.

# Targets
The targets for this work are outlined again below, some of the targets could not be met due to time constraints as well as
complexity issues.

## Scheduler targets
The runtime had no scheduler, the demonstration code executed one task and then exited. 
So there was a need for a scheduler to track the state of all tasks and decide which tasks are able to run and on which
node they should execute.
Some things which were mentioned here are dependency tracking for tasks the same way \omp does it and memory locality, 
which means scheduling tasks on nodes where the memory needed for them is already present.
During the course of this work it was decided that turning the scheduler in a separate library would benefit the 
extendability of it, so this was added to the things wanted for the end result.

## Runtime targets
The target for the runtime was to allow memory transfers across nodes and support for the scheduler from above.
It should also link with the applications created from a preprocessed source in order to allow a ordinary C++ source
file with \omp tasks to be preprocessed, compiled and ran with a as close as possible experience to a shared memory
run of the same unprocessed source file.