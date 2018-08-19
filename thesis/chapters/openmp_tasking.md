In order to understand the steps which are needed to allow a task to be run on a different node one first has to
understand \omp tasks in general.
This chapter is about the tasking constructs as defined in the \omp manual \cite{openmp45}.
All terms used in the description of the internal mechanics od \omp are used the same way as in the manual.

# Overview
There are several tasking constructs in \omp, but not all of them are processed by the \gls{tp} because some are
rarely used or too complex at this point in time and are thus postponed to a later version of this project.
Most constructs can be configured with clauses, which are discussed later on for the relevant constructs.

All constructs start with \texttt{\#pragma omp} and the name, clauses are added at the end.
Some constructs modify the following block, others act more like a method call.
Which blocks are modified and how is detailed in the respective paragraphs.

## Task construct
This is the main \omp construct to start a new asynchronous computation.
It is declared by a pragma which affects the following block, which can be a simple expression, a call or a
code block.
When the control flow of the program encounters a task directive, a task is generated and the encountering thread enters
a task scheduling point where it can execute the new task or defer it for later execution by another thread in the
current thread team.
A Task construct can be modified by several clauses, for example one which demands immediate execution or another one
for specifying task local data.

As this is the most important tasking construct it is transformed by the \gls{tp} and has a separate chapter discussing
all the clauses and the transformation in general.

## Taskloop construct
A for loop modified by a taskloop construct spawns tasks for each loop iteration or a number of iterations combined.
This restricts those loops the the canonical loop form as on page 53 of \cite{openmp45}, which essentially says that
the loop has to have an initializer, an increment expression and a test expression.
There are even more clauses than on the task construct and supporting it would require even more work.
Furthermore it is rarely used in the example codes and thus was not considered worth the effort for this thesis.

## Taskloop simd construct
This construct distributes the loop according to the taskloop construct and the resulting tasks are modified to use
simd internally according to the simd constructs of \omp.
Because of the tremendous amount of complexity here it is not addressed at all in this thesis, as it also inherits
all clauses of taskloop and simd.

## Taskyield construct
The taskyield construct is a standalone construct and thus works like a function call.
When it is encountered during program execution the current task can be suspended and another task starts execution.
In the runtime there is currently no support for task suspension, so it is of no use to transform it and it is not
transformed yet.
In the future it might be replaced by a function call into the runtime.

## Taskwait construct
The taskwait construct is also a standalone construct, which waits for all direct child tasks to finish execution.
Note that it does not wait for grandchildren, so it has to be added in every task which has to make sure all children
terminate.
The current task is suspended and another task can be picked up for execution if there is any.
As it is an important way to synchronize and order tasks it is transformed into a function call.

## Taskgroup construct
In a taskgroup construct every new tasks binds to the group and has to end before the group statement or block returns.
The end of the block is also a task scheduling point where other tasks of the group can be picked up if they are not
executed at the time.
This construct would introduce a lot of complexity by exposing all internal bookkeeping of the runtime and is thus
postponed to a later stage where the runtime is more refined and stable.

# The Task Construct \label{task}
An \omp task is declared by a tasking directive (\texttt{\#pragma omp task}) and consists of the directive and the associated
code section which is either a single statement or a code block.
It can be further configured by clauses which are evaluated either at compile time or every time the control flow
encounters the task directive.

Task completion can be guaranteed by either ending the parallel section or by a taskwait construct.

In the following paragraphs those clauses and how to apply them on a cluster shall be discussed in the same order they
are defined in the \omp manual.

\image{task_definition.png}{The definition of an \omp 4.5 Task}{\cite[p.~84]{openmp45}.}

The task structure mentioned in the following paragraphs is the one from the tasking header.
\footnote{The tasking header can be found at section \ref{tasking-header}.}

## `if` clause
When a task with an if clause is encountered during execution, the scalar expression is evaluated and it's truth value
computed.
If the value is \texttt{true} nothing happens, but if the expression evaluates to \texttt{false} the current task is suspended until
the new task is completed and execution continues with the new task.

As this is directly impacts the control flow of the program it has to be transformed and built into the runtime.
The expression is stored and evaluated when the task structure is constructed.

## `final` clause
When a task has a final clause and the associated expression evaluates to \texttt{true} the task is a final task.
A final task only generates final and included tasks if it encounters a task construct.
Included tasks are executed immediately by the encountering thread and no task is generated at all.
This is especially useful in order not to generate tiny tasks in a recursion where at some point the overhead of
creating and scheduling a task becomes the majority of time spent on the task.

Thus the final clause is transformed and stored the same way as the if clause.
As the \texttt{final} clause is inherited for all child tasks, this information has to be provided by the
runtime environment as it is not known during the \gls{tp} phase.

## `untied` clause
A task can be suspended when it reaches a task scheduling point such as the creation of another task or a taskwait
construct.
Tasks which execution already started can only be resumed by the thread which first started them if there is no untied
clause present.
The untied clause thus allows the task to continue execution by another thread.
This might improve overall performance and responsibility of the program because one long running task can no longer
block several other ones.

In the current version of the runtime there is no support for suspending tasks, but the value is parsed and stored none
the less.

## `default` clause
This clause defines the default handling of captured variables, the variables used in the code section of the task.
Possible values are shared, which is the default and means that variables are shared between the task and the enclosing
scope, and none, which means that every variable which is used within the task has to appear in a private,
firstprivate or shared clause.
If one is omitted it is a compiler error.

As this clause is evaluated during compile time and, because of this, it is not strictly
necessary to parse this clause.
But it has some influence if a task has variables for which no sharing clause exists and is thus parsed as
\texttt{Task.shared\_by\_default}.
This allows shortcuts in the memory sharing logic of the runtime.

## `mergeable` clause
If a task has a mergeable clause and is an included task \footnote{See section \ref{final-clause} for the definition.}
the task may be executed within the same data environment as the generating task.
This further reduces overhead of an included task.

This clause is also unused at the moment due to lack of support by the runtime but is still parsed and stored, as it
will become relevant when special handling of final tasks is improved.

## Access clauses
These clauses give detailed information about how variables are shared and they might occur several times with different
variable names.
They are parsed into an enumeration in the extraction of variable bindings which is described in section \ref{variable-extraction}.

### `private` clause
There is a new variable of the same kind for each task, but it is not specified how or whether it is initialized.
According to the specification it should always be considered uninitialized and after the construct it is not defined
which of the variables, the original one or one from the tasks, is referenced by the original reference.

### `firstprivate` clause
The variable is initialized once and then copied in order to obtain a private reference within each thread.
This can pose a problem if the task is untied, in this case access to this variable is non conforming and thus undefined
behaviour.

### `shared` clause
Variables which are defined to be shared are exactly this, every task gets the original reference to the data and the
programmer has to ensure that no race conditions may ever happen.
This behaviour is closely related to what happens when the programmer spawns threads on his own, because the references
this thread may have behave exactly this way.

## `depend` clause
With this clause dependencies between tasks can be defined.
If a variable occurs in an 'in' or 'inout' depend clause and in an 'out' or 'inout' clause of a sibling task the task
can only be started if the sibling has already finished.

The values of this clause are parsed into a list of strings for each type and are then used by the runtime to decide
determine whether a task can run already or whether it has to wait for another task to finish.

## `priority` clause
This clause is evaluated as a numerical clause and the number is a recommendation for the runtime system which tasks
to run first.
The higher the number the higher the priority for the task.
If this clause is omitted the priority is 0.

It is extracted and then evaluated when the task is constructed, but due to the lack of a proper scheduler it is not used
at the time of writing.

# Other transformations
## Taskwait Construct transformation
The taskwait construct from section \ref{taskwait-construct} is transformed to a call to the \texttt{taskwait}
function provided by the tasking header.
\footnote{This function is not implemented yet, as it is a feature of the runtime which is not covered by this thesis.}
This function then uses the runtime in order to find all children and wait for their termination.

## Number of threads
In \omp it is possible for an application the specify how many threads it wants to have at a certain point in the current
region.
This is usually done by calling \texttt{omp_set_num_threads} or by adding a \texttt{num_threads} clause to an \omp
parallelization construct.
In order to fully utilize a node of a cluster it might be advantageous to allocate several tasks to this node.
Therefore it is necessary to know how many cores of the node are currently in use.
Furthermore is the number of threads lost when a task is transferred to another node, which might pose some issues,
for example if there should only be two threads for interleaving execution of a loop or something like that.
In order to deal with the complex mechanics of this, whenever such a function call or clause is discovered it will be
replaced by a push and pop mechanism on a claim stack.
From there a task on a new node can restore the previous settings and the runtime can take advantage of that.
This mechanic is in the early prototype stage so since runtime support is still lacking.