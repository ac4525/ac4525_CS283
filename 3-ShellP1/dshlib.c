#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist){
    
    char *cmd = strtok(cmd_line, PIPE_STRING); //separates the user's command by pipe if possible
    int commands = 0; //used to keep track of the amount of commands parsed

    memset(clist, 0, sizeof(command_list_t));  //clears list

    //This while loop iterates through all the commands separated by a pipe if there is one
    while (cmd != NULL) {
        if (commands >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        //removes leading spaces by changing the pointer start position
        while (*cmd == ' ') {
            cmd++;
        }

        //removes trailing spaces by starting at the end of the command and changing the end pointer position
        char *end = cmd + strlen(cmd) - 1;
        while (end > cmd && *end == ' ') {
            end--;
        }
        *(end + 1) = '\0';

        //finds first space to indicate everything after it to be the arguments to the executable name
        char *space = strchr(cmd, SPACE_CHAR);

        if (space != NULL) {
            //ensures that argument and executable name are within the maximum length
            if (space - cmd > EXE_MAX || strlen(space + 1) > ARG_MAX){
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            //
            strncpy(clist->commands[commands].exe, cmd, space - cmd);
            strncpy(clist->commands[commands].args, space + 1, strlen(space + 1));
        } else {
            if (strlen(cmd) > EXE_MAX){
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strcpy(clist->commands[commands].exe, cmd);
        }

        commands++;
        cmd = strtok(NULL, PIPE_STRING); //iterates to next command separated by pipes
    }
    clist->num = commands;
    return OK;
}