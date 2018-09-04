% Execution of Task Parallel OpenMP Programs in Distributed Memory Environments - Preprocessor
% Markus Baur
% 2018-09-04

# Inhalt
- Motivation
- Overview OpenMP
- Tasking constructs
- Requirements
- Implementation
- Limitations and Outlook

# Motivation
- MPI is hard
- OpenMP is widely used and easy
- Large shared memory systems are expensive
- Task parallel programming has good capsulation

# Overview OpenMP
- API for easy parallelisation in shared memory environments
- Sequential code can be annotated and parallelised
- Comes from parallelisation of loops

# Overview OpenMP
## Example
```c++
int main() {
	int a[] = new int[4096];

	#pragma omp parallel for
	for(int i = 0; i < 4096; i++){
		a[i] = ...;
	}
}
```

# Overview OpenMP
## Task parallel OpenMP

- OpenMP also supports task based parallelism
- Also with annotations
- Has to identify independent tasks

# Overview OpenMP
## Example
```c++
void mergesort(int * X, int n, int * tmp)
{
   #pragma omp task firstprivate (X, n, tmp)
   mergesort(X, n/2, tmp);

   #pragma omp task firstprivate (X, n, tmp)
   mergesort(X+(n/2), n-(n/2), tmp);
 
   #pragma omp taskwait
   merge(X, n, tmp);
}
```

# Overview OpenMP
## Limitations
- Requires shared memory
- Completely different programming model
- Requires rewrite of applications

# Tasking constructs
- Annotations to source blocks
- Can be configured with clauses
- Executed by OpenMP thread group

# Tasking constructs
## Clauses
- if <!-- blocking -->
- final <!-- no-task -->
- untied
- default
- mergeable
- private
- firstprivate
- shared
- depend
- priority

# Tasking constructs
## Example
```c++
#pragma omp task firstprivate (X, n, tmp)
mergesort(X, n/2, tmp);
```

```c++
#pragma omp task untied mergeable if(i == 3) \
final(i == 5) depend(in: a)
a[i] = i + *p;
```

# Requirements
## General
- Scalability within a supercomputer
- No changes to OpenMP
- Runtime is done by Johannes

## Preprocessor
- Extract all necessary task data for runtime
- Rebuild program structure
- Determine size of variables

# Implementation
- Runtime header
- Use clang tooling
- Extraction of code and size
- Rewrite of main

# Implementation
## Runtime header
- Defines the task struct and methods
- Contains helper functions
- Only part shared between runtime and preprocessor

# Implementation
## Clang tooling
- First approach to parsing with python
- Problems when extracting source
- Extendable lexer and parser
- Insights into types, e.g. size

# Implementation
## Extraction
- Task annotation becomes Task struct
- All clauses are extracted
- Code is rewritten and extracted via clang
- Attempt to determine the size of Variables

# Implementation
## Size calculation at runtime
- Size information is wiped in C++
- Stack is small enough to send fully
- Can be determined via allocation size on heap
- Works only with default glibc allocator

# Implementation
## Size calculation at runtime
```c++
size_t t = (size_t)&var;
do {
    size = malloc_usable_size((void*)t);
    t--;
    loops++;
} while (size == 0); 

return size - loops;
```

# Limitations
- Integration is missing
- Nested pointers can not be transmitted
- Problems with iteration through clang parsed tree
- Scheduler not done yet
- Memory alignment has to be respected
- OpenMP annotations within tasks can overload a node
- Files and other process resources are not handled

# End

Thank you for listening!