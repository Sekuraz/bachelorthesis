There is a permanently increasing need for more computation power, for example in order to run more detailed simulations
or use even more data for machine learning.
If one only wants to use a shared memory system this poses a big problem because shared memory systems grow much faster 
in price than in computation power.
This leads to the usage of clusters where a lot of shared memory systems are coupled together with a high bandwidth and 
low latency communication interface.
Going down this road allows for several thousand times the computation power of one single shared memory system, or how
it is called in a cluster, a single node.
On the other hand programming fast and efficient code for a cluster is hard and difficult, one has to get the 
communication right and try not to loose to much performance with it.
This thesis, together with another one by Johannes Erwerle\footnote{It can be found at \cite{jo}.}, tries to 
automatically transform a program from the simpler code which can be run on a single node to one which utilizes a cluster
and achieves the best scaling possible.
In order to do this we extract \omp tasks from the program and offload them to other nodes in the cluster.
The focus of this thesis is how to transform the code of the program so the runtime built in the other thesis can take 
over.