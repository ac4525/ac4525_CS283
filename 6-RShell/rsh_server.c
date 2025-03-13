
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>

//INCLUDES for extra credit
//#include <signal.h>
//#include <pthread.h>
//-------------------------

#include "dshlib.h"
#include "rshlib.h"


/*
 * start_server(ifaces, port, is_threaded)
 *      ifaces:  a string in ip address format, indicating the interface
 *              where the server will bind.  In almost all cases it will
 *              be the default "0.0.0.0" which binds to all interfaces.
 *              note the constant RDSH_DEF_SVR_INTFACE in rshlib.h
 * 
 *      port:   The port the server will use.  Note the constant 
 *              RDSH_DEF_PORT which is 1234 in rshlib.h.  If you are using
 *              tux you may need to change this to your own default, or even
 *              better use the command line override -s implemented in dsh_cli.c
 *              For example ./dsh -s 0.0.0.0:5678 where 5678 is the new port  
 * 
 *      is_threded:  Used for extra credit to indicate the server should implement
 *                   per thread connections for clients  
 * 
 *      This function basically runs the server by: 
 *          1. Booting up the server
 *          2. Processing client requests until the client requests the
 *             server to stop by running the `stop-server` command
 *          3. Stopping the server. 
 * 
 *      This function is fully implemented for you and should not require
 *      any changes for basic functionality.  
 * 
 *      IF YOU IMPLEMENT THE MULTI-THREADED SERVER FOR EXTRA CREDIT YOU NEED
 *      TO DO SOMETHING WITH THE is_threaded ARGUMENT HOWEVER.  
 */
int start_server(char *ifaces, int port, int is_threaded){
    int svr_socket;
    int rc;
    
    //to remove warning for unused extra credit
    is_threaded = 0;
    if (is_threaded == 0){
        rc = 0;
    }

    //
    //TODO:  If you are implementing the extra credit, please add logic
    //       to keep track of is_threaded to handle this feature
    //

    svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0){
        int err_code = svr_socket;  //server socket will carry error code
        return err_code;
    }

    rc = process_cli_requests(svr_socket);

    stop_server(svr_socket);


    return rc;
}

/*
 * stop_server(svr_socket)
 *      svr_socket: The socket that was created in the boot_server()
 *                  function. 
 * 
 *      This function simply returns the value of close() when closing
 *      the socket.  
 */
int stop_server(int svr_socket){
    return close(svr_socket);
}

/*
 * boot_server(ifaces, port)
 *      ifaces & port:  see start_server for description.  They are passed
 *                      as is to this function.   
 * 
 *      This function "boots" the rsh server.  It is responsible for all
 *      socket operations prior to accepting client connections.  Specifically: 
 * 
 *      1. Create the server socket using the socket() function. 
 *      2. Calling bind to "bind" the server to the interface and port
 *      3. Calling listen to get the server ready to listen for connections.
 * 
 *      after creating the socket and prior to calling bind you might want to 
 *      include the following code:
 * 
 *      int enable=1;
 *      setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
 * 
 *      when doing development you often run into issues where you hold onto
 *      the port and then need to wait for linux to detect this issue and free
 *      the port up.  The code above tells linux to force allowing this process
 *      to use the specified port making your life a lot easier.
 * 
 *  Returns:
 * 
 *      server_socket:  Sockets are just file descriptors, if this function is
 *                      successful, it returns the server socket descriptor, 
 *                      which is just an integer.
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code is returned if the socket(),
 *                               bind(), or listen() call fails. 
 * 
 */
int boot_server(char *ifaces, int port){
    
    //makes the server socket
    int svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket == -1){
        return ERR_RDSH_COMMUNICATION;
    }

    int enable=1;
    setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    //initializes the sockaddr_in structure to specify the server address details
    struct sockaddr_in s_address;
    s_address.sin_family = AF_INET;
    s_address.sin_port = htons(port);
    if (inet_pton(AF_INET, ifaces, &s_address.sin_addr) <= 0) {
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }
   
    //bind the server to the port and interface
    if (bind(svr_socket, (struct sockaddr *)&s_address, sizeof(s_address)) == -1) {
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    //allows the server to receive connections
    if (listen(svr_socket, 5) == -1) {
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    printf("Server is listening on %s:%d\n", ifaces, port);

    return svr_socket;
}

/*
 * process_cli_requests(svr_socket)
 *      svr_socket:  The server socket that was obtained from boot_server()
 *   
 *  This function handles managing client connections.  It does this using
 *  the following logic
 * 
 *      1.  Starts a while(1) loop:
 *  
 *          a. Calls accept() to wait for a client connection. Recall that 
 *             the accept() function returns another socket specifically
 *             bound to a client connection. 
 *          b. Calls exec_client_requests() to handle executing commands
 *             sent by the client. It will use the socket returned from
 *             accept().
 *          c. Loops back to the top (step 2) to accept connecting another
 *             client.  
 * 
 *          note that the exec_client_requests() return code should be
 *          negative if the client requested the server to stop by sending
 *          the `stop-server` command.  If this is the case step 2b breaks
 *          out of the while(1) loop. 
 * 
 *      2.  After we exit the loop, we need to cleanup.  Dont forget to 
 *          free the buffer you allocated in step #1.  Then call stop_server()
 *          to close the server socket. 
 * 
 *  Returns:
 * 
 *      OK_EXIT:  When the client sends the `stop-server` command this function
 *                should return OK_EXIT. 
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code terminates the loop and is
 *                returned from this function in the case of the accept() 
 *                function failing. 
 * 
 *      OTHERS:   See exec_client_requests() for return codes.  Note that positive
 *                values will keep the loop running to accept additional client
 *                connections, and negative values terminate the server. 
 * 
 */
int process_cli_requests(int svr_socket){
    
    int client_socket; //used to accept a client socket connection
    struct sockaddr_in c_address;
    socklen_t client_len = sizeof(c_address);
    char *buffer = (char *)malloc(RDSH_COMM_BUFF_SZ);  //Allocate buffer for communication

    if (!buffer) {
        return ERR_RDSH_COMMUNICATION;
    }

    while (1) {
        //accepts a client socket connection
        client_socket = accept(svr_socket, (struct sockaddr *)&c_address, &client_len);
        if (client_socket < 0) {
            free(buffer);
            return ERR_RDSH_COMMUNICATION;
        }

        //executes a client's command requests until they choose to exit or stop the server
        int rc = exec_client_requests(client_socket);
        close(client_socket);
        if (rc < 0) {
            printf(RCMD_SERVER_EXITED);
            break;
        }
    }

    free(buffer);
    stop_server(svr_socket);
    
    return OK_EXIT;
}

/*
 * exec_client_requests(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client
 *   
 *  This function handles accepting remote client commands. The function will
 *  loop and continue to accept and execute client commands.  There are 2 ways
 *  that this ongoing loop accepting client commands ends:
 * 
 *      1.  When the client executes the `exit` command, this function returns
 *          to process_cli_requests() so that we can accept another client
 *          connection. 
 *      2.  When the client executes the `stop-server` command this function
 *          returns to process_cli_requests() with a return code of OK_EXIT
 *          indicating that the server should stop. 
 * 
 *  Note that this function largely follows the implementation of the
 *  exec_local_cmd_loop() function that you implemented in the last 
 *  shell program deliverable. The main difference is that the command will
 *  arrive over the recv() socket call rather than reading a string from the
 *  keyboard. 
 * 
 *  This function also must send the EOF character after a command is
 *  successfully executed to let the client know that the output from the
 *  command it sent is finished.  Use the send_message_eof() to accomplish 
 *  this. 
 * 
 *  Of final note, this function must allocate a buffer for storage to 
 *  store the data received by the client. For example:
 *     io_buff = malloc(RDSH_COMM_BUFF_SZ);
 *  And since it is allocating storage, it must also properly clean it up
 *  prior to exiting.
 * 
 *  Returns:
 * 
 *      OK:       The client sent the `exit` command.  Get ready to connect
 *                another client. 
 *      OK_EXIT:  The client sent `stop-server` command to terminate the server
 * 
 *      ERR_RDSH_COMMUNICATION:  A catch all for any socket() related send
 *                or receive errors. 
 */
int exec_client_requests(int cli_socket) {

    char *io_buff = malloc(RDSH_COMM_BUFF_SZ); //allocate enough space for io_buff
    if (io_buff == NULL) {
        return ERR_RDSH_COMMUNICATION;
    }

    int bytes_received;
    //creates a command list structure to store all the posible commands that the user entered if separated by pipes
    command_list_t *cmd_list;
    cmd_list = malloc(sizeof(command_list_t));
    cmd_list->commands[0]._cmd_buffer = malloc(SH_CMD_MAX * sizeof(char));
    cmd_list->commands[0]._cmd_buffer[strcspn(cmd_list->commands[0]._cmd_buffer, "\n")] = '\0';

    //This while loop continues to process client requests or commands until they end the connection
    while (1) {
        
        //receive information from the client socket
        bytes_received = recv(cli_socket, io_buff, RDSH_COMM_BUFF_SZ, 0);
        
        if (bytes_received == -1) {
            free(io_buff);  
            return ERR_RDSH_COMMUNICATION;
        } else if (bytes_received == 0) {
            break;
        }
        
        io_buff[bytes_received] = '\0'; //null terminate the client socket information

        //checks if the command received is exit, which will close the client connection
        if (rsh_match_command(io_buff) == BI_CMD_EXIT) {
            send_message_eof(cli_socket);  
            free(io_buff);  
            return OK;
        //checks if the command received is stop-server, which will close both the client and server connection
        } else if (rsh_match_command(io_buff) == BI_CMD_STOP_SVR)  {
            send_message_eof(cli_socket);  
            free(io_buff); 
            return OK_EXIT;
        } 
        else {
            
            //checks if the command starts or ends with a pipe, which is invalid
            if ((strncmp(io_buff, PIPE_STRING, 1) == 0) || (strncmp(io_buff + strlen(io_buff) - 1, PIPE_STRING, 1) == 0)){
                send_message_string(cli_socket, "Command cannot start or end with pipes\n");
                continue;
            }

            //initializes the amount of commands to be zero
            cmd_list->num = 0;
            
            //split input using pipe symbol 
            char *cmd = strtok(io_buff, PIPE_STRING);
            int invalid_command = 0; 
            int valid = 0; 

            //This while loop stores commands split by pipes
            while (cmd != NULL && cmd_list->num < CMD_MAX) {
                while (*cmd == SPACE_CHAR || *cmd == '\t') cmd++;  
                if (*cmd == '\0' || strlen(cmd) == 0) {
                    invalid_command = 1;  
                }
                //printf("Unparsed: %s\n", cmd);
                
                //parses each command separated by pipes
                valid = parse_command(cmd, cmd_list);  
                
                //printf("size: %d\n", cmd_list->commands[cmd_list->num].argc);

                //if too many arguments are entered, an error statement will print
                if (valid  == 1){
                    send_message_string(cli_socket, "Too many arguments\n");
                    break;
                } else if (valid == 2){
                    send_message_string(cli_socket, "Error: Mismatched quotes\n");
                    break;
                }
            
                //printf("Parsed arguments for command: %s\n", cmd);
                //printf("1st argo: %s\n", cmd_list->commands[cmd_list->num].argv[1]);

                //keeps track of number of commands 
                cmd_list->num++;
                cmd = strtok(NULL, PIPE_STRING);
            }
            
            //skip the rest of the while loop if an invalid command is used
            if (valid == 1){
                continue;
            } else if (valid == 2){
                continue;
            }
        
            //checks for empty strings
            if (invalid_command == 1 || cmd_list->num == 0) {
                send_message_string(cli_socket, CMD_WARN_NO_CMD);
                continue;
            }

            //check if too many pipes are used
            if (cmd_list->num >= CMD_MAX) {
                send_message_string(cli_socket, "error: piping limited to 8 commands\n");
                continue;
            }
            //executes the client's commands
            rsh_execute_pipeline(cli_socket, cmd_list);
        }

        send_message_eof(cli_socket);
    }
    free(cmd_list->commands[0]._cmd_buffer);
    free(cmd_list);
    free(io_buff); 
    
    return ERR_RDSH_COMMUNICATION;
}

int parse_command(char *cmd, command_list_t *cmd_list) {
    char *current = cmd;
    char *start = current;
    cmd_buff_t *cmd_buff = &cmd_list->commands[cmd_list->num];

    cmd_buff->argc = 0;
    cmd_buff->_cmd_buffer = cmd;
    
    //skip leading spaces
    while (*current == SPACE_CHAR || *current == '\t') {
        current++;
    }

    while (*current != '\0') {
        //skip leading spaces of current pointer
        while (*current == SPACE_CHAR || *current == '\t') {
            current++;
        }
        if (*current == '\0' || cmd_buff->argc >= CMD_ARGV_MAX) {
            break;
        }
        //if there are quotes, capture everything inside the quote as the argument
        if (*current == QUOTE_CHAR) {
            current++; 
            start = current;

            while (*current != QUOTE_CHAR  && *current != '\0') {
                current++;
            }

            //move past the quote for the next argument
            if (*current == QUOTE_CHAR) {
                *current = '\0';
                current++; 
            } else {
                return 2;
            }
        } else {
            //non quotes will be normally parse with spaces or tabs
            start = current;
            while (*current != SPACE_CHAR && *current != '\t' && *current != '\0') {
                current++;
            }
            if (*current != '\0') {
                *current = '\0'; 
                current++;
            }
        }

        //store the argument in the command's argv array
        cmd_buff->argv[cmd_buff->argc++] = start;

        //skip over any trailing spaces or tabs for the next loop
        while (*current == SPACE_CHAR || *current == '\t') {
            current++;
        }
    }
    if (cmd_buff->argc > 8){
        return 1;
    }
    // Null-terminate the argv array
    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

/*
 * send_message_eof(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client

 *  Sends the EOF character to the client to indicate that the server is
 *  finished executing the command that it sent. 
 * 
 *  Returns:
 * 
 *      OK:  The EOF character was sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the EOF character. 
 */
int send_message_eof(int cli_socket){
    int bytes_sent;

    //send one character, the EOF character.
    bytes_sent = send(cli_socket, &RDSH_EOF_CHAR, 1, 0);
    if (bytes_sent == 1){
        //this is what is expected, we sent a single character,
        //the EOF character, so we can return a good error code.
        //we use OK for this as defined in dshlib.h
        return OK;
    }
    

    //handle error and send back an appropriate error code
    //if bytes_sent < 0 that would indicate a network error
    //if it equals zero it indicates the character could not
    //be sent, which is also an error.  I could not imagine a
    //situation where bytes_sent > 1 since we told send to 
    //send exactly one byte, but if this happens it would also
    //be an error.

    //Ill just return a generic COMMUNICATION error we defined
    //for you in rshlib.h, but you can return different error
    //codes for different conditions if you want. 
    return ERR_RDSH_COMMUNICATION;
}

/*
 * send_message_string(cli_socket, char *buff)
 *      cli_socket:  The server-side socket that is connected to the client
 *      buff:        A C string (aka null terminated) of a message we want
 *                   to send to the client. 
 *   
 *  Sends a message to the client.  Note this command executes both a send()
 *  to send the message and a send_message_eof() to send the EOF character to
 *  the client to indicate command execution terminated. 
 * 
 *  Returns:
 * 
 *      OK:  The message in buff followed by the EOF character was 
 *           sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the message followed by the EOF character. 
 */
int send_message_string(int cli_socket, char *buff){
    int bytes_sent;
    //sends the message to the client interface
    bytes_sent = send(cli_socket, buff, strlen(buff), 0);
    if (bytes_sent == -1) {
        return ERR_RDSH_COMMUNICATION;
    }

    if (send_message_eof(cli_socket) != OK) {
        return ERR_RDSH_COMMUNICATION;
    }

    return OK;
}


/*
 * rsh_execute_pipeline(int cli_sock, command_list_t *clist)
 *      cli_sock:    The server-side socket that is connected to the client
 *      clist:       The command_list_t structure that we implemented in
 *                   the last shell. 
 *   
 *  This function executes the command pipeline.  It should basically be a
 *  replica of the execute_pipeline() function from the last deliverable. 
 *  The only thing different is that you will be using the cli_sock as the
 *  main file descriptor on the first executable in the pipeline for STDIN,
 *  and the cli_sock for the file descriptor for STDOUT, and STDERR for the
 *  last executable in the pipeline.  See picture below:  
 * 
 *      
 *┌───────────┐                                                    ┌───────────┐
 *│ cli_sock  │                                                    │ cli_sock  │
 *└─────┬─────┘                                                    └────▲──▲───┘
 *      │   ┌──────────────┐     ┌──────────────┐     ┌──────────────┐  │  │    
 *      │   │   Process 1  │     │   Process 2  │     │   Process N  │  │  │    
 *      │   │              │     │              │     │              │  │  │    
 *      └───▶stdin   stdout├─┬──▶│stdin   stdout├─┬──▶│stdin   stdout├──┘  │    
 *          │              │ │   │              │ │   │              │     │    
 *          │        stderr├─┘   │        stderr├─┘   │        stderr├─────┘    
 *          └──────────────┘     └──────────────┘     └──────────────┘   
 *                                                      WEXITSTATUS()
 *                                                      of this last
 *                                                      process to get
 *                                                      the return code
 *                                                      for this function       
 * 
 *  Returns:
 * 
 *      EXIT_CODE:  This function returns the exit code of the last command
 *                  executed in the pipeline.  If only one command is executed
 *                  that value is returned.  Remember, use the WEXITSTATUS()
 *                  macro that we discussed during our fork/exec lecture to
 *                  get this value. 
 */
int rsh_execute_pipeline(int cli_sock, command_list_t *clist) {
    int pipe_fds[2]; //File descriptors
    pid_t pid;
    int prev_fd = cli_sock;  
    int status;
    int rc = 0;

    if (!clist || clist->num == 0) {
        return EXIT_FAILURE;
    }

    //This for loops attempts to execute each command in the pipeline
    for (int i = 0; i < clist->num; i++) {

        if (i < clist->num - 1) {  
            if (pipe(pipe_fds) < 0) {
                return EXIT_FAILURE;
            }
        }
        //Check if the command is a built-in command for cd
        if (rsh_built_in_cmd(&clist->commands[i]) == BI_EXECUTED){
            continue;
        } 
        pid = fork(); //Create a child process
        if (pid == -1) {
            return EXIT_FAILURE;
        }

        if (pid == 0) { //Child process
            //Redirect input for the first command to the client socket or previous process
            if (i == 0) {
                dup2(cli_sock, STDIN_FILENO);
            } else {
                dup2(prev_fd, STDIN_FILENO);
            }
            //Redirect output to the next process or client socket 
            if (i < clist->num - 1) {
                dup2(pipe_fds[1], STDOUT_FILENO);
            } else {
                dup2(cli_sock, STDOUT_FILENO);
                dup2(cli_sock, STDERR_FILENO);
            }
            
            //Close all pipes in child process
            close(pipe_fds[0]);
            close(pipe_fds[1]);
            close(prev_fd);
            close(cli_sock);

            //checks if the command is built-in dragon command
            if (rsh_built_in_cmd(&clist->commands[i]) == BI_CMD_DRAGON){
                print_dragon();
                exit(EXIT_SUCCESS);
            } 
            //Execute the command
            rc = execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            //Checks if the command is invalid
            if (rc != 0){
                printf("Command is invalid or not implemented\n");
            }
            exit(EXIT_FAILURE);
        } else { 
            if (i < clist->num - 1) {
                close(pipe_fds[1]);  
            }

            if (prev_fd != cli_sock) {
                close(prev_fd);  
            }

            if (i < clist->num - 1) {  
                prev_fd = pipe_fds[0];
            } else { 
                prev_fd = cli_sock;
            }
        
        }
    }
    //Wait for all children
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
}

/**************   OPTIONAL STUFF  ***************/
/****
 **** NOTE THAT THE FUNCTIONS BELOW ALIGN TO HOW WE CRAFTED THE SOLUTION
 **** TO SEE IF A COMMAND WAS BUILT IN OR NOT.  YOU CAN USE A DIFFERENT
 **** STRATEGY IF YOU WANT.  IF YOU CHOOSE TO DO SO PLEASE REMOVE THESE
 **** FUNCTIONS AND THE PROTOTYPES FROM rshlib.h
 **** 
 */

/*
 * rsh_match_command(const char *input)
 *      cli_socket:  The string command for a built-in command, e.g., dragon,
 *                   cd, exit-server
 *   
 *  This optional function accepts a command string as input and returns
 *  one of the enumerated values from the BuiltInCmds enum as output. For
 *  example:
 * 
 *      Input             Output
 *      exit              BI_CMD_EXIT
 *      dragon            BI_CMD_DRAGON
 * 
 *  This function is entirely optional to implement if you want to handle
 *  processing built-in commands differently in your implementation. 
 * 
 *  Returns:
 * 
 *      BI_CMD_*:   If the command is built-in returns one of the enumeration
 *                  options, for example "cd" returns BI_CMD_CD
 * 
 *      BI_NOT_BI:  If the command is not "built-in" the BI_NOT_BI value is
 *                  returned. 
 */
Built_In_Cmds rsh_match_command(const char *input)
{
    //handles all the built-in commands by matching them with the input
    if (strcmp(input, "cd") == 0){
        return BI_CMD_CD;
    } else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    } else if (strcmp(input, EXIT_CMD) == 0){
        return BI_CMD_EXIT;
    } else if (strcmp(input, "stop-server") == 0){
        return BI_CMD_STOP_SVR;
    } 
    return BI_NOT_BI;
}

/*
 * rsh_built_in_cmd(cmd_buff_t *cmd)
 *      cmd:  The cmd_buff_t of the command, remember, this is the 
 *            parsed version fo the command
 *   
 *  This optional function accepts a parsed cmd and then checks to see if
 *  the cmd is built in or not.  It calls rsh_match_command to see if the 
 *  cmd is built in or not.  Note that rsh_match_command returns BI_NOT_BI
 *  if the command is not built in. If the command is built in this function
 *  uses a switch statement to handle execution if appropriate.   
 * 
 *  Again, using this function is entirely optional if you are using a different
 *  strategy to handle built-in commands.  
 * 
 *  Returns:
 * 
 *      BI_NOT_BI:   Indicates that the cmd provided as input is not built
 *                   in so it should be sent to your fork/exec logic
 *      BI_EXECUTED: Indicates that this function handled the direct execution
 *                   of the command and there is nothing else to do, consider
 *                   it executed.  For example the cmd of "cd" gets the value of
 *                   BI_CMD_CD from rsh_match_command().  It then makes the libc
 *                   call to chdir(cmd->argv[1]); and finally returns BI_EXECUTED
 *      BI_CMD_*     Indicates that a built-in command was matched and the caller
 *                   is responsible for executing it.  For example if this function
 *                   returns BI_CMD_STOP_SVR the caller of this function is
 *                   responsible for stopping the server.  If BI_CMD_EXIT is returned
 *                   the caller is responsible for closing the client connection.
 * 
 *   AGAIN - THIS IS TOTALLY OPTIONAL IF YOU HAVE OR WANT TO HANDLE BUILT-IN
 *   COMMANDS DIFFERENTLY. 
 */
Built_In_Cmds rsh_built_in_cmd(cmd_buff_t *cmd)
{
    //only the cd command is directly executed, while other commands must be done directly on the other functions
    if (rsh_match_command(cmd->argv[0]) == BI_CMD_CD) {
        if (cmd->argc == 2) {
            if (chdir(cmd->argv[1]) != 0) {
                return ENOTDIR;
            }
            return BI_EXECUTED;
        } else if (cmd->argc > 2) {
            return ERR_TOO_MANY_COMMANDS;
        }
    } 
    return rsh_match_command(cmd->argv[0]);
}
