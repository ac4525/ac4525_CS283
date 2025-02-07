#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dshlib.h"

#define DRAGON "dragon"

/*
 * Implement your main function by building a loop that prompts the
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.  Since we want fgets to also handle
 * end of file so we can run this headless for testing we need to check
 * the return code of fgets.  I have provided an example below of how
 * to do this assuming you are storing user input inside of the cmd_buff
 * variable.
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
 *
 *   Expected output:
 *
 *      CMD_OK_HEADER      if the command parses properly. You will
 *                         follow this by the command details
 *
 *      CMD_WARN_NO_CMD    if the user entered a blank command
 *      CMD_ERR_PIPE_LIMIT if the user entered too many commands using
 *                         the pipe feature, e.g., cmd1 | cmd2 | ... |
 *
 *  See the provided test cases for output expectations.
 */
int main()
{
    char *cmd_buff = malloc(SH_CMD_MAX * sizeof(char));
    int rc = 0;
    command_list_t clist;

    if (cmd_buff == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';  

        //exits program if user types the exit command
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            free(cmd_buff);
            exit(OK);
        }
        
        //if user types in dragon command, it will print the drexel dragon stored in the dragon file
        if (strcmp(cmd_buff, DRAGON) == 0) {
            printf("\n");
            FILE *file = fopen(DRAGON, "r");
            //in case dragon file was not downloaded or in right location
            if (file == NULL){
                printf("Error: Dragon File not found");
                continue;
            }
            char line[1024];  
            while (fgets(line, sizeof(line), file)) {
                printf("%s", line); 
            }
            fclose(file); 
            printf("\n");
            continue;
        }

        //checks for non-empty command
        if (strlen(cmd_buff) == 0) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        //call build_cmd_list to parse the commands entered by the user
        rc = build_cmd_list(cmd_buff, &clist);
        if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        }
        if (rc == ERR_CMD_OR_ARGS_TOO_BIG){
            printf("Error: Either executable name or the arg string too long\n");
            continue;
        }

        //prints header to show the amount of commands parsed, with arguments if applicable 
        printf(CMD_OK_HEADER, clist.num);
        for (int i = 0; i < clist.num; i++) {
            printf("<%d> %s", i + 1, clist.commands[i].exe);
            if (strlen(clist.commands[i].args) > 0) {
                printf(" [%s]", clist.commands[i].args);
            }
            printf("\n");
        }
    }

    free(cmd_buff);
    return OK;
}