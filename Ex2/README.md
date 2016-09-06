1. Run the programs from assignments A and B with fork, vfork and threads.

- What is the difference between fork and vfork?
vfork() has undefined behaviour when trying to access any variables except from `pid`.

- How do POSIX threads handle local and global variables?
Global variables are cached (each thread has its own copy). 


2. Run the program from assignment C.

- What happens with the threads that does not get access to a resource right away?
It waits (does nothing and releases all its locks).

- Are the resources shared fairly among the threads?
The resources are shared randomly.

3. Run the program from assignment D.

- What happens when running the threads before adding a mutex, and why?
Nothing goes wrong! WHY??
Expected: Second thread shows non-incremented variables because the first thread increments their cached copies. 

4. Run the program from assignment E.

- How did the deadlock happen?
All philosofers take forks in the same order (e.g. left, then right, independent of indexes which results that the last philosofer takes forks in reverse order with respect to indexes).

- How did you solve it?
Each philosofer first takes lower-index fork, then higher-index fork, no matter which of the forks is right or left.

- Can you think of other methods to solve it?
Take both forks simultaneously or learn to eat with 1 fork.