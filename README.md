# Concurrent_Prefix_Sum
Problem by Dr.Neeraj Mittal.
Solution by Avishikta Bagchi.

Solving the Prefix-Sum problem using  processes, arrive-and-wait barrier for synchronization, and a concurrent implementation of the Hillis-Steele algorithm.

Arguments = (n, m, input file, output file name)
An array of size n is created to hold the values for which the prefix-sum will be calculated.
m cores are simulated by created m processes.
An input file is passed to initialize the array.
An output file name is passed that is created at the end of the program, which contains the final prefix-sum.

The main process should create m worker (child) processes to perform the work as required by modified Hillis and Steele’s algorithm. The main process is responsible for 
reading the input array from the input file and writing the output array to the output file.

Each iteration generates a new array with the final iteration generating the output array. All worker processes must finish the current iteration
before any worker process can start the next iteration. To achieve this synchronization, a barrier is implemented. A barrier provides a single method arriveAndWait( ). When a process
invokes the arriveAndWait( ) method, it is blocked until all other processes have also invoked the arriveAndWait( ) method. 

All processes should use shared memory created using shmget( ) system call to share all data structures, which includes the input array, the output array, all intermediate arrays as needed and
any data structure used to implement a reusable barrier.
