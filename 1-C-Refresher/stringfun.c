#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
void  word_print(char *, int, int);
void  reverse(char *, int, int);
void replace(char *, int, int, char , char);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    
    //checks for non-null pointers and empty strings
    if (buff == NULL || user_str == NULL || *user_str == '\0'){
        return -2;
    }

    //to count the amount of characters
    int str_len = 0;
    //to check if the space if duplicate
    int space = 1;
    while (*user_str != '\0'){
        if (*user_str != ' ' && *user_str != '\t'){
            *buff = *user_str;
            buff++;
            str_len++;
            space = 0;
        }
        else if (space == 0){
            *buff = ' ';
            buff++;
            str_len++;
            space = 1;
        }
        user_str++;
    }

    //remove trailing spaces
    while (str_len > 0 && (*(buff - 1) == ' ' || *(buff - 1) == '\t')) {
        buff--;  
        str_len--;  
    }

    //user's string is too long
    if (str_len > len){
        return -1;
    }
    //user's string is just spaces or tabs
    else if (str_len == 0){
        return -2;
    }    

    //store the original length of the words
    int original_len = str_len;
    //adds trailing dots to reach 50 characters
    while (str_len < len){
        *buff = '.';
        buff++;
        str_len++;
    }
    //set null terminator
    *(buff + str_len) = '\0';
    return original_len; //for now just so the code compiles. 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    
    int words = 0;
    int characters;
    //ensure characters is not over the buffer amount
    if (str_len > len){
        characters = len;
    } 
    else {
        characters = str_len;
    }
    
    //counts amount of spaces to correlate to number of words
    for (int i = 0; i < characters; i++) {
        if (*(buff + i) == ' ') {
            words++;
        }
    }   

    //extra word from counting spaces
    return words + 1;
}


//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

void word_print(char *buff, int len, int str_len) {
    int characters;
    int word_num = 1;
    int word_letters = 0;
    //to store each individual word
    char *word = NULL;  
    //ensure characters is not over the buffer amount
    if (str_len > len) {
        characters = len;
    } else {
        characters = str_len;
    }
    
    //iterates through entire buffer 
    for (int i = 0; i < characters; i++) {
        //every new word will reset the word pointer
        if (word_letters == 0) {
            word = buff + i;  
        }
        //space indicates new word
        if (*(buff + i) == ' ') {  
            printf("%d. %.*s (%d)\n", word_num, word_letters, word, word_letters);
            word_num++;
            word_letters = 0;  
        }
        //counts letter of each word
        else {
            word_letters++;
        }
    }
    //prints last word that is missed in the loop
    printf("%d. %.*s (%d)\n", word_num, word_letters, word, word_letters);
}

void reverse(char *buff, int len, int str_len){
    int characters;
    //ensure characters is not over the buffer amount
    if (str_len > len) {
        characters = len;
    } else {
        characters = str_len;
    }

    //iterates half of the characters pointer to swap letters from front and back
    for (int i = 0; i < characters/2; i++){
        //temporary variable to store the front character
        char front = *(buff + i);
        //swaps front and back characters
        *(buff + i) = *(buff + characters - i - 1);
        *(buff + characters - i - 1) = front;
      
    }
    
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // This is safe because it checks that there are at least two arguments before
    // checking that the second argument starts with a hyphen. Since the program's name
    // is considered an argument, it will ensure that the user entered something else
    // before it checks that the user started their argument with a hyphen.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // This checks that the user has at least 3 arguments, including the program's name.
    // This ensures that the user entered a flag, along with a string.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = malloc(BUFFER_SZ*sizeof(char));
    if (buff == NULL){
        printf("Error allocating memeroy for buffer");
        exit(2);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'w':
            printf("Word Print\n");
            printf("----------\n");
            word_print(buff, BUFFER_SZ, user_str_len);
            break;
        
        case 'r':
            reverse(buff, BUFFER_SZ, user_str_len);
            break;
        
        case 'x':
            char target_word = argv[3];
            char replacement_word = argv[4];
            replace(buff, BUFFER_SZ, user_str_len, target_word, replacement_word);
            break;

        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE