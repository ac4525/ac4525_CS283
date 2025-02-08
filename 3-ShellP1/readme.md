1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  'fget()' is good choice for this application because it allows the program properly process one line before moving onto the next one. It also limits the amount of characters that can be read to prevent the buffer from overflowing.
    
2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: 'malloc()' was needed to allocate memory for 'cmd_buff' because the space needed will vary based off of the user's input. It is better than allocating a fixed size array to reduce having too much memory space.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  It was nesecarry to trim leading and trailing spaces from each command before storing it to ensure that the spaces are not counted towards the maximum argument or executable name characters. The extra spaces may cause an error or mess up the format of the commands, where the executable name will be counted as an argument because of the leading space. Overall, this process was required to properly process or store each command.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  One redirection example is redirecting the standard output to a file. A challenge that might occur is file handling, where a new file is created each time or one file gets repeatly overwritten. Another redirection example is redirecting standard input from a file. A challenge that might occur is handling file permissions and being able to correctly process the commands that are written in the files. A third example is appending output to a file. A challenge that might occur is ensuring that the file does not get overwritten by new output data. A fourth example is redirecting standard error to a file. The same challenge occurs as redirecting a standard output, where the file needs to correctly handled.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  The key difference between redirection and piping is that redirection is used for transfering or processing commands from or to a file, while piping is passing on the output of a command to another command directly.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It is important to keep these separate in a shell because it helps with debugging and error handling. It separates the valid and invalid outputs to allow the user to find potential problems with the shell.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our custom shell should handle errors from commands that fail by printing an error statement explaining what command failed and why it failed. Our custom shell should not merge STDOUT anad STDERR for better error handling. However, if it must be merged, the redirection "2>&1" can be used to merge them.
