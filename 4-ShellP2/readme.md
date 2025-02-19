1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  We use fork/execvp instead of calling execvp directly because fork creates a child process, which allows the shell to continue running. If fork was not provided, it would cause the shell to terminate itself after each command.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork() system call fails, it exits with an error code and the child processor is not created. My implementation sets the error code equal to the return code and prints an error message. Then, it continues running and asking for user input.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  The execvp() finds the command to execute by looking through directories in a given path. The system environment variable that plays a role in this process is PATH.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The purpose of calling wait() in the parent process after forking is to wait for the child processor to finish processing, so that the parent processor can collect the exit status of the child process. If we did not call it, the result of the child processor may never be shown and the parent processor would continue to run with processes in the background.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() provides the exit status code from the child processor when terminated. It is important because it allows the parent processor to determine if the child processor successfully executed a command or failed to specific reason.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of build_cmd_buff() handles quoted arguments by putting all the content inside a closed quotation as one argument. I had to revert back to manually parsing the arguments by spaces and having a conditional statement that checks for quotations. This is necessary because using strtok() or strchr() from the previous assignment caused the spaces inside quotations to not be saved. Additionally, all content inside the quotation had to be put together for the echo command to work properly.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: My previous assignment used strchr() to search for the first space and splitting the commands by pipes. However, for this assignment pipes were not an issue, so that splitting part had to be removed. Additionally, using strchr() or strtok() to split the command by spaces made it really challenging to manage quotations within the command. I spent a lot time trying to use those functions to split the arguments and consider the quotations, but it kept bringing up tons of unexpected behaviors. As a result, I reverted back to manually parsing the arguments by spaces and having a conditional statement that checks for quotations.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  The purpose of signals in a Linux system is that is notifies a process about events and controls its behaviors. They differ from other forms of interprocess communication by having asynchronous notifications and do not transfer data. IPC transfer data between processes rather than indicating an event or request. 

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL is signal 9. This signal immediately terminates a process and cannot be caught, blocked, or ignored by the process. It is typically used when a process is unresponsive and needs to be forcefully killed. SIGTERM is signal 15. This signal sends a request to terminate a process. It is typically used to cleanly exit or terminate processes. SIGINT is signal 2. This signal is created when a user interrupts a process by pressing control + c on their keyboards. It is typically used to stop long running processes in the terminal.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it is immediately stopped. It cannot be caught or ignored like SIGINT because it was designed in a way to stop a program that cannot be prevented. The kernel gives SIGSTOP system-level control over processes.
