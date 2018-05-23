# Relevant clauses
As shown in \ref{the-task-construct} there are some clauses which can be used to customize a \omp task.
In the following chapter it will be discussed how to transform these clauses when the code should run on a cluster and 
which clauses are not covered by the transformation and the runtime.
But first the task has to be initialized, this is done by commenting out the \omp task construct and instead adding 
a task struct which is defined in the tasking header\footnote{The header can be found in the appendix, chapter 
\ref{tasking-header}.}. 
Examples can be found at the end of this chapter.

## `if` clause transformation 
The \texttt{if} clause is an important clause\fref{if} because it directly impacts the control flow of the program 
and, because of this, has to be transformed and built into the runtime.
The expression is evaluated when the task is constructed and it is attached to the task as a field for use within the
runtime environment like this:

## `final` clause transformation 
The \texttt{final} clause is also important\fref{final} and thus transformed the same way as the \texttt{if} clause. 
Furthermore the \texttt{final} clause is inherited for all child tasks, but this information has to be provided by the
runtime environment as it is not known during the \gls{tp} phase.

## `untied` clause transformation
In the current iteration of the runtime tasks can not be suspended and thus the \texttt{untied} clause
\fref{untied} is not needed currently,
but it is parsed and stored non the less.

## `default` clause transformation
The \texttt{default} clause is evaluated during compile time\fref{default} and, because of this, it is not strictly
necessary to parse this clause. 
But it has some influence if a task has variables for which no sharing clause exists and is thus parsed as 
\texttt{Task.shared\_by\_default}.
On the other hand, if there is no sharing clause for a variable, it has to be shared in \omp for \cpp.

## `mergeable` clause transformation
The \texttt{mergeable} clause is also unused in the current iteration of the runtime, but it most likely will be used 
later on because scheduling new tasks has become even more expensive than in standard \omp.\fref{mergeable}

## Access clause transformation
All access clauses
\footnote{The \texttt{private}, \texttt{firstprivate} and \texttt{shared} clauses as defined in their
paragraphs \ref{private-clause}, \ref{firstprivate-clause} and \ref{shared-clause}}
are transformed the same way.
They are used as a modifier for the extraction of variable bindings which are described in \ref{variable-extraction}.

## `depend` clause transformation
The \texttt{depend} clauses are parsed into a dependency list using strings to store the \texttt{in}, \texttt{out}
\texttt{inout} dependencies of the task.
Those values are used according to the definition in paragraph \ref{depend-clause} to sort out dependencies between
tasks.

## `priority` clause transformation
The \texttt{priority} clause\fref{priority} is evaluated when encountered during task setup, but currently not used by
the runtime due to the lack of a scheduler which uses the priority value of a task.

\clearpage

## Example
This is a short example of the transformation of a task description, it should not be considered as a full example of
a transformation.
\newline 

\begin{lstlisting}[language=C++, caption=\omp task before transformation]
#pragma omp task untied mergeable if(i == 3) final(i == 5) depend(in: a)
\end{lstlisting}

This is transformed according to the explanation above into the following code.
A comment marker is inserted before the \texttt{omp} in the line above.
\newline 

\begin{lstlisting}[language=C++, caption=\omp task after transformation]
#pragma //omp task untied mergeable if(i == 3) final(i == 5) depend(in: a)
Task t(5960595027353748277ull);
t.if_clause = (i == 3);
t.final = (i == 5);
t.untied = true;
t.mergeable = true;
t.in.push_back("a");
\end{lstlisting}
 
# The Taskwait Construct Transformation
The \texttt{taskwait} construct is replaced by a call to the \texttt{taskwait} function of the tasking header
\footnote{See the header in the appendix \ref{tasking-header}.}

<!-- TODO num_threads -->

