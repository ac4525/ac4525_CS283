#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"


#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    command_list_t cmd_list;
    int exit_num = 0;

    //allocate memory for the command buffer
    cmd_list.commands[0]._cmd_buffer = malloc(SH_CMD_MAX * sizeof(char));
    if (cmd_list.commands[0]._cmd_buffer == NULL) {
        printf("Error: Memory allocation failed\n");
        return ERR_MEMORY;
    }

    while (1) {
        
        printf("%s", SH_PROMPT);

        if (fgets(cmd_list.commands[0]._cmd_buffer, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        //remove the trailing \n from cmd_buff
        cmd_list.commands[0]._cmd_buffer[strcspn(cmd_list.commands[0]._cmd_buffer, "\n")] = '\0';


        char *pipeCheck = cmd_list.commands[0]._cmd_buffer;
        while (*pipeCheck == ' ' || *pipeCheck == '\t') {
            pipeCheck++;
        }
        if ((strncmp(pipeCheck, PIPE_STRING, 1) == 0) || (strncmp(pipeCheck + strlen(pipeCheck) - 1, PIPE_STRING, 1) == 0)){
            printf("Command cannot start or end with pipes\n");
            exit_num = ENOENT;
            continue;
        }

        cmd_list.num = 0;
        
        //split input using pipe symbol 
        char *cmd = strtok(cmd_list.commands[0]._cmd_buffer, PIPE_STRING);
        int invalid_command = 0;  

        //This while loop stores commands split by pipes
        while (cmd != NULL && cmd_list.num < CMD_MAX) {
            while (*cmd == ' ' || *cmd == '\t') cmd++;  
            if (*cmd == '\0' || strlen(cmd) == 0) {
                invalid_command = 1;  
            }
            
            cmd_list.commands[cmd_list.num]._cmd_buffer = cmd;
            cmd_list.num++;
            cmd = strtok(NULL, PIPE_STRING);
        }
        
        //checks for empty strings
        if (invalid_command == 1 || cmd_list.num == 0) {
            printf(CMD_WARN_NO_CMD);
            exit_num = WARN_NO_CMDS;
            continue;
        }

        //check if too many pipes are used
        if (cmd_list.num >= CMD_MAX) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            exit_num = ERR_TOO_MANY_COMMANDS;
            continue;
        }

        int pipefds[cmd_list.num - 1][2]; //file descriptors for pipes
        pid_t pids[cmd_list.num];
        
        //create pipes
        for (int i = 0; i < cmd_list.num - 1; i++) {
            if (pipe(pipefds[i]) == -1) {
                perror("pipe");
                exit(ERR_EXEC_CMD);
            }
        }

        //Iterate over each command in the pipeline
        for (int i = 0; i < cmd_list.num; i++) {
            
            cmd_list.commands[i].argc = 0;
            char *start = cmd_list.commands[i]._cmd_buffer;
            char *current = start;
            
            //This while loop parses the command by spaces, while maintaining all the content in quotations as one argument
            while (*current != '\0') {
                //skip leading spaces of current pointer
                while (*current == SPACE_CHAR || *current == '\t') {
                    current++;
                }
                if (*current == '\0' || cmd_list.commands[i].argc >= CMD_ARGV_MAX) {
                    break;
                }

                //if there are quotes, capture everything inside the quote as the argument
                if (*current == QUOTE_CHAR) {
                    current++;
                    start = current;
                    while (*current != QUOTE_CHAR && *current != '\0') {
                        current++;
                    }
                    //move past the quote for the next argument
                    if (*current == QUOTE_CHAR) {
                        *current = '\0';
                        current++;
                    } else {
                        printf("Error: Mismatched quotes\n");
                        exit(ERR_EXEC_CMD);
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
                cmd_list.commands[i].argv[cmd_list.commands[i].argc++] = start;
                if (cmd_list.commands[i].argc >= CMD_ARGV_MAX) {
                    break;
                }
            }
            cmd_list.commands[i].argv[cmd_list.commands[i].argc] = NULL;
            
            //checks for argument being too long
            for (int j = 0; j < cmd_list.commands[i].argc; j++) {
                if (strlen(cmd_list.commands[i].argv[j]) > ARG_MAX) {
                    exit_num = ERR_CMD_OR_ARGS_TOO_BIG;
                    printf("argument too long\n");
                    break;
                }
            }

            //checks for executable name being too long
            if (strlen(cmd_list.commands[i].argv[0]) > EXE_MAX) {
                exit_num = ERR_CMD_OR_ARGS_TOO_BIG;
                printf("executable too long\n");
                break;
            }
            

            //checks for commands with too many arguments
            if (cmd_list.commands[i].argc == CMD_ARGV_MAX) {
                printf("Too many arguments\n");
                exit_num = ERR_TOO_MANY_COMMANDS;
                break;
            }

            //exits program if user types the exit command
            if (strcmp(cmd_list.commands[i]._cmd_buffer, EXIT_CMD) == 0 && cmd_list.commands[i].argc == 1) {
                free(cmd_list.commands[0]._cmd_buffer);
                exit_num = 0;
                exit(OK);
            }

            //prints dragon if user types the dragon command
            if (strcmp(cmd_list.commands[i]._cmd_buffer, "dragon") == 0 && cmd_list.commands[i].argc == 1) {
                print_dragon();
                exit_num = 0;
                continue;
            }

            //executes cd command if user types the cd command
            if (strcmp(cmd_list.commands[i].argv[0], "cd") == 0) {
                if (cmd_list.commands[i].argc == 2) {
                    if (chdir(cmd_list.commands[i].argv[1]) != 0) {
                        printf("%s Directory does not exist\n", cmd_list.commands[i].argv[1]);
                        exit_num = ENOTDIR;
                        continue;
                    }
                    exit_num = 0;
                } else if (cmd_list.commands[i].argc > 2) {
                    printf("Too many arguments\n");
                    exit_num = ERR_TOO_MANY_COMMANDS;
                }
                break;
            }

            //prints return code of most recent command, excluding rc itself
            if (strcmp(cmd_list.commands[i].argv[0], "rc") == 0) {
                printf("%d\n", exit_num);
                continue;
            }

            //Fork process for command execution
            pids[i] = fork();
            if (pids[i] == -1) {
                perror("fork error");
                exit(ERR_EXEC_CMD);
            }

            if (pids[i] == 0) { //Child process
                //Redirect input if not the first command
                if (i > 0) {
                    dup2(pipefds[i - 1][0], STDIN_FILENO);
                }

                //Redirect output if not the last command
                if (i < cmd_list.num - 1) {
                    dup2(pipefds[i][1], STDOUT_FILENO);
                }

                //Close all pipes in child process
                for (int j = 0; j < cmd_list.num - 1; j++) {
                    close(pipefds[j][0]);
                    close(pipefds[j][1]);
                }

                //Execute the command
                int rc = execvp(cmd_list.commands[i].argv[0], cmd_list.commands[i].argv);

                if (rc < 0) {
                    if (errno == ENOENT) {
                        printf("Command not found\n");
                    } else if (errno == EACCES) {
                        printf("Permission denied\n");
                    } else if (errno == ENOTDIR) {
                        printf("Not a directory\n");
                    } else if (errno == EISDIR) {
                        printf("Cannot execute directory\n");
                    } else if (errno == ENOEXEC) {
                        printf("Cannot execute invalid format\n");
                    } else {
                        printf("Command cannot be executed\n");
                    }
                    exit(errno);
                }
            }
        }

        //Supervisor process
        for (int i = 0; i < cmd_list.num - 1; i++) {
            close(pipefds[i][0]);
            close(pipefds[i][1]);
        }
        //Wait for all children
        for (int i = 0; i < cmd_list.num; i++) {
            int status;
            waitpid(pids[i], &status, 0);
            if (WIFEXITED(status)) {
                exit_num = WEXITSTATUS(status);
            }
        }
    }

    free(cmd_list.commands[0]._cmd_buffer);
    return exit_num;
}
