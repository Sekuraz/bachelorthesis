In order to put this all together here is an example of a full \gls{tp} run.
This is only a simple one file program in order to avoid all the shortcomings of the current version.

\bigskip
\lstinputlisting[language=C++, caption=The example input]{code/test.cpp}

It becomes the following output after processing, note the incorrect indentation.
In order to indent the inserted code correctly one would have to determine the current indentation and whether it is
one, two, four or eight spaces per indentation level.
This effort was not deemed worth the necessary time and thus the indentation is a bit odd.

\bigskip
\lstinputlisting[language=C++, caption=The example output]{code/out.cpp}

As you can see the a task instance is created, the associated code id is handed to the task and all the clauses are
transformed as specified in section \ref{task}.
Furthermore the tasking header is added to the include list of the source file and the setup and teardown functions are
added before the first instruction in main and before the return statement, if there is one.

\bigskip
\lstinputlisting[language=C++, caption=The example output header]{code/out.cpp.hpp}

In this file the \texttt{tasking_function_map} is defined, the associated code block of the task is used inside of a
function boilerplate which extracts all the packed variables and then executes the original code.
On top pf that the \texttt{tasking_function_map} is also populated using static evaluation and thus making the map
available to use.

The last file is the header to put it all together, but in this example there is only one file to include.

\bigskip
\lstinputlisting[language=C++, caption=The example output all header]{code/all.hpp}



