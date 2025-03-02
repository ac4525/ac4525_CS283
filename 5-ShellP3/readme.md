1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation ensure that all child processes complete before the shell continues accepting user input by having a for loop that waits for each of the child processes. If waitpid() is not called on all child processes, they may never show up as an output on the shell or it can cause pipe commands to be out of order.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

It is necessary to close unused pipes ends after calling dup2() because to prevent processes from resource leaks, and unintended data flow. Something that could go wrong if you leave pipes open is that it may hang indefinitely, which can lead to data corruption.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is implemented as a built-in rather than external command because the directories and files are on the local shell. Making cd an external command can be difficult because we would have to share our local path contents with the external command so that it can access it.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To modify the implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently, realloc() can be used to increase the storage as more pipe commands are needed to be stored. The trade-offs are that it is much more complex, making debugging processes harder, and uses more resources.
