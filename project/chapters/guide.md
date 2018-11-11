
# Add new examples
## How to add another example
In the source code there is one directory called test, add the new source files in that directory and copy the relevant
sections from the \texttt{CMakeLists.txt} in the same directory and modify them to include the new files.
Note that you have to run the preprocessor on every file which contains \omp tasks as well as the file which contains
the main function.
After invoking \texttt{cmake} to build the new target wait for it to finish. 
Because it builds the whole clang frontend it may take more than 20 minutes for the first time on a standard workstation.
It is also recommended to install \texttt{ccache} if frequent changes to any \texttt{CMakeLists.txt} are made because such a 
change currently triggers a full rebuild of clang on the next build.

## How to run resulting applications
Currently all applications and libraries use the full path in which they were created as a link target, so running them
somewhere else requires setting the \texttt{LD\_LIBRARY\_PATH} environment variable to the directory in which they are
located.
Furthermore all applications created with the current library require a execution by \texttt{mpirun}, otherwise they
only display an error message and exit.
On top of that the parameter \texttt{worker\_per\_runtime} limits the number of usable workers to \texttt{100} because
support for multiple runtime nodes is not finished at the moment.

# Distribute another program
## Build preparations
After receiving a copy of the full source code of this project one should create a new folder within this project and
place the sources of the application which is about to be distributed in this folder. 
The recommended way to build it is to add the new folder to the top level \texttt{CMakeLists.txt} of this project
and add dependencies to \texttt{tdomp}, the runtime library, and \texttt{processor}, the preprocessor, in the 
\texttt{CMakeLists.txt} of your project.
Due to the complex dependency tracking the only build system currently supported is \texttt{cmake}.
Then add a preprocessor run to every source file from which tasks should be extracted to the applications build steps.
An example how this step might look like can be found in the file \texttt{test/CMakeLists.txt} in this project.
If the dependencies between the generated files and the original target are set up correctly, for example by specifying
them as the sources for the original binary, a full \texttt{cmake --build} call is enough to build the preprocessor,
the runtime and the target application with the preprocessed sources.
The first build might take very long, and, because the long build is done every time any \texttt{CMakeLists.txt} is 
changed, it is recommended to use \texttt{ccache} or another caching mechanism.
It is also recommended to delete \texttt{/tmp/tasking\_functions} before a build in which filenames are changed.
In this folder support files are created which are becoming outdated after a filename is changed.

## Running the resulting application
After the build finishes the resulting application can no longer be ran by invoking it directly, because one node, the
first one, is reserved for the runtime.
In order to run the resulting application \texttt{mpirun} has to be used. 
Currently there is support for one task per worker process, so one might opt into running multiple mpi processes per 
node.

## Debugging the code base
Due to the nature of the preprocessor compiler errors show up on the preprocessed file.
Make sure to edit the original file and not the preprocessed ones, as they will be overwritten if the original files
change.
Please keep in mind that none of the \omp task clauses and neither taskwait nor taskloop instructions are currently
supported by the runtime.

In order to debug the resulting application it is recommended to take a look at \cite{mpi-debug}, there is a list of
several debugging methods for parallel programs using mpi.
The approach that worked best during the coding for this work was to use 

\texttt{mpirun -np 4 xterm -e gdb -ex run the-resulting-applcation}

but other approaches might work better than this for specific problems.
