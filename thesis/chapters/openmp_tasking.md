In order to understand the steps which are needed to allow a task to be run on a different node one first has to 
understand \omp tasks in general. 
This chapter is about the tasking constructs as defined in the \omp manual \cite{openmp45}.

# The Task Construct
An \omp task is declared by a tasking directive (\texttt{\#pragma omp task}) and consists of the directive and the associated
code section which is either a single statement or a code block.
It can be further configured by clauses which are evaluated either at compile time or every time the control flow
encounters the task directive.
When the control flow of the program encounters a task directive, a task is generated and the encountering thread may 
execute it right at the time or defer it for later execution by another thread in current thread team. 
Task completion can be guaranteed by either ending the parallel section or by another directive, the taskwait directive
(\texttt{\#pragma omp taskwait}). <!-- TODO -->

In the following paragraphs those clauses and how to apply them on a cluster shall be discussed in the same order they
are defined in the \omp manual. \image{task_definition.png}{The definition of a OpenMP 4.5 Task}{\cite[p.~84]{openmp45}.}

## `if` clause
When a task with an if clause is encountered during execution, the scalar expression is evaluated and it's truth value
computed.
If the value is \texttt{true} nothing happens, but if the expression evaluates to \texttt{false} the current task is suspended until 
the new task is completed and execution continues with the new task.

## `final` clause
When a task has a final clause and the associated expression evaluates to \texttt{true} the task is a final task.
A final task only generates final and included tasks if it encounters a task construct.
Included tasks are executed immediately by the encountering thread and no task is generated at all.
This is especially useful in order not to generate tiny tasks in a recursion where at some point the overhead of 
creating and scheduling a task becomes the majority of time spent on the task.

## `untied` clause
A task can be suspended when it reaches a task scheduling point such as the creation of another task or a taskwait
construct.
Tasks which execution already started can only be resumed by the thread which first started them if there is no untied
clause present.
The untied clause thus allows the task to continue execution by another thread.
This might improve overall performance and responsibility of the program because one long running task can no longer 
block several other ones.

## `default` clause
This clause defines the default handling of captured variables, the variables used in the code section of the task.
Possible values are shared, which is the default and means that variables are shared between the task and the enclosing
scope, and none, which means that every variable which is used within the task has to appear in a private, 
firstprivate or shared clause.
If one is omitted it is a compiler error.

## `mergeable` clause
If a task has a mergeable clause and is an included task \footnote{See paragraph \ref{final-clause} for the definition.}
the task may be executed within the same data environment as the generating task.
This further reduces overhead of an included task.

## `private` clause
There is a new variable of the same kind for each task, but it is not specified how or whether it is initialized.
According to the specification it should always be considered uninitialized and after the construct it is not defined
which of the variables, the original one or one from the tasks, is referenced by the original reference.

## `firstprivate` clause
The variable is initialized once and then copied in order to obtain a private reference within each thread.
This can pose a problem if the task is untied, in this case access to this variable is non conforming and thus undefined
behaviour.

## `shared` clause
Variables which are defined to be shared are exactly this, every task gets the original reference to the data and the
programmer has to ensure that no race conditions may ever happen.
This behaviour is closely related to what happens when the programmer spawns threads on his own, because the references
this thread may have behave exactly this way.

## `depend` clause
With this clause dependencies between tasks can be defined. 
If a variable occurs in an 'in' or 'inout' depend clause and in an 'out' or 'inout' clause of a sibling task the task
can only be started if the sibling has already finished.

## `priority` clause
This clause is evaluated as a numerical clause and the number is a recommendation for the runtime system which tasks
to run first.
The higher the number the higher the priority for the task.
If this clause is omitted the priority is 0.
 
# The Taskwait Construct
This construct is used to wait for all tasks which were generated from the currently running code.
The full construct is \texttt{\#pragma omp taskwait}, there is no configuration possible.
One noticeable thing is that it does not wait for children of the tasks it waits for, so not the whole task hierarchy
has to complete before this construct returns.
It is also a task scheduling point because otherwise every thread encountering the construct would stall.
