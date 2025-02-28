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
int exec_local_cmd_loop() {
    cmd_buff_t cmd_buff;
    int exit_num = 0;

    //allocate memory for the command buffer
    cmd_buff._cmd_buffer = malloc(SH_CMD_MAX * sizeof(char));
    if (cmd_buff._cmd_buffer == NULL) {
        printf("Error: Memory allocation failed\n");
        return ERR_MEMORY;
    }
 
    while (1) {
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff._cmd_buffer, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        //remove the trailing \n from cmd_buff
        cmd_buff._cmd_buffer[strcspn(cmd_buff._cmd_buffer, "\n")] = '\0';    
        
        cmd_buff.argc = 0;
        char *start = cmd_buff._cmd_buffer;
        char *current = start;

        //This while loop parses the command by spaces, while maintaining all the content in quotations as one argument
        while (*current != '\0') {
            
            //skip leading spaces of current pointer
            while (*current == SPACE_CHAR || *current == '\t') {
                if (*current == '\0') {
                    break;  
                }
                current++;
            }
            if (*current == '\0' || cmd_buff.argc >= CMD_MAX) {
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
        
            cmd_buff.argv[cmd_buff.argc++] = start;
        
            if (cmd_buff.argc >= CMD_MAX) {
                break;
            }
        }

        cmd_buff.argv[cmd_buff.argc] = NULL;

        //checks for non-empty command
        if (cmd_buff.argc == 0) {
            printf(CMD_WARN_NO_CMD); 
            exit_num = WARN_NO_CMDS;
            continue;
        }
        
        //checks for commands with too many arguments
        if (cmd_buff.argc == CMD_MAX){
            printf("too many arguments\n");
            exit_num = ERR_TOO_MANY_COMMANDS;
            continue;
        }
        
        //exits program if user types the exit command
        if (strcmp(cmd_buff._cmd_buffer, EXIT_CMD) == 0 && cmd_buff.argc == 1) {
            free(cmd_buff._cmd_buffer);
            exit_num = 0;
            exit(OK);
        }
    
        //prints dragon if user types the dragon command
        if (strcmp(cmd_buff._cmd_buffer, "dragon") == 0 && cmd_buff.argc == 1) {
            print_dragon();
            exit_num = 0;
            continue;
        }
    

        //executes cd command if user types the cd command
        if (strcmp(cmd_buff.argv[0], "cd") == 0){
            if (cmd_buff.argc == 2){
                if (chdir(cmd_buff.argv[1]) != 0) {
                    printf("%s Directory does not exist\n", cmd_buff.argv[1]);
                    exit_num = ENOTDIR;
                    continue;
                }
                exit_num = 0;
            } else if (cmd_buff.argc > 2){
                printf("too many arguments");
                exit_num = ERR_TOO_MANY_COMMANDS;
            }
            continue;
        }

        //prints return code of most recent command, excluding rc itself
        if (strcmp(cmd_buff.argv[0], "rc") == 0){
            printf("%d\n", exit_num);
            continue;
        }
       
        // Fork a process to execute the command
        int f_result, c_result; 
        f_result = fork();
        if (f_result < 0) {
            exit_num = errno;
            exit(errno);
        }
        if (f_result == 0) {  // Child process
            int rc = execvp(cmd_buff.argv[0], cmd_buff.argv);
            
            if (rc < 0){
                if (errno == ENOENT) {
                    printf("Command not found\n");
                } else if (errno == EACCES) {
                    printf("Permission denied\n");
                } else if (errno == ENOTDIR) {
                    printf("Not a directory\n");
                }else if (errno == EISDIR) {
                    printf("Cannot execute directory\n");
                } else if (errno == ENOEXEC) {
                    printf("Cannot execute invalid format\n");
                }
                else {
                    printf("Command cannot be executed\n");
                }
                exit(errno); 
            }
        } else {  // Parent process
            wait(&c_result);
    
            //macro in the runtime library to extract
            //the status code from what wait_returns
            exit_num = WEXITSTATUS(c_result);
            if (WEXITSTATUS(c_result) == 0){
                continue;
            } else {
                continue;
            }
        
        }
    }

    free(cmd_buff._cmd_buffer);
    return exit_num;
}