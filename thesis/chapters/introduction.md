OpenMP \cite{openmp45} is a widely used API for parallel scientific computing applications.

Was ist OpenMP?
Wozu wird es verwendet?

- parallel computing
  - loops
  - tasks
  - GPGPUs/Accelerators
- easy to use
- shared memory

Limitations of shared memory systems

- Number of CPU cores/FLOPs is limited or very expensive

Motivation: Easy way to speed up existing applications

- Fast to test if speedup is possible
- no changes needed in the code

- complete parallel rewrites in MPI/HPX/etc needs time
- deep understanding of what the code is doing and why it is doing it
- probably many performance optimizations have to be done again

