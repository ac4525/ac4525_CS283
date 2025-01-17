#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
//add additional prototypes here
int  count_words(char *, int, int);
void  word_print(char *, int, int);
void  reverse(char *, int, int);
void replace(char *, int, int, char *, char *);
int isEmpty(char *);




int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    
    //checks for non-null pointers
    if (buff == NULL || user_str == NULL){
        return -2;
    }
    int str_len = 0; //to count the amount of characters
    int space = 1; //to check if the space if duplicate, starts at 1 to avoid leading spaces to be copied over to the buffer
    //This while loop iterates through the entire string that the user inputs and transfers over characters
    //and non-consecutive spaces
    while (*user_str != '\0'){
        if (*user_str != ' ' && *user_str != '\t' && *user_str != '\n'){
            *buff = *user_str;
            str_len++;
            buff++;
            space = 0;
        } else if (space == 0){
            *buff = ' ';
            str_len++;
            buff++;
            space = 1;
        }
        user_str++;
    }
    //This while loop removes trailing spaces in the buffer
    while (str_len > 0 && (*(buff - 1) == '\t' || *(buff - 1) == '\n' || *(buff - 1) == ' ')) {
        buff--;  
        str_len--;  
    }
    //user's string is too long even after trimming spaces will cause an error
    if (str_len > len){
        return -1;
    }
    int original_len = str_len; //store the original length of the words
    //This while loop fills the remaining space of the buffer with dots
    while (str_len < len){
        *buff = '.';
        buff++;
        str_len++;
    }
    *(buff + str_len) = '\0'; //set null terminator on buffer
    return original_len; 
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    printf("]");
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
    } else {
        characters = str_len;
    }
    //This for loop counts the amount of spaces in the buffer
    for (int i = 0; i < characters; i++) {
        if (*(buff + i) == ' ') {
            words++;
        }
    }
    //adds an extra word number if the string is not empty
    if (isEmpty(buff) == 0){
        words++;
    }
    return words;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

void word_print(char *buff, int len, int str_len) {
    int characters;
    int word_num = 1;
    int word_letters = 0;
    char *word = NULL;  //used to store each individual word
    //ensure characters in the string are not over the allowed amount
    if (str_len > len) {
        characters = len;
    } else {
        characters = str_len;
    }
    //This for loop iterates through the entire buffer and captures each word in a pointer
    //The beginning of the pointer is reset each time a space is found, indicating the start of a new word
    //It also counts the amount of letters in each word by checking whether or not the character is a space
    for (int i = 0; i < characters; i++) {
        if (word_letters == 0) {
            word = buff + i;  
        }
        if (*(buff + i) == ' ') {  
            printf("%d. %.*s(%d)\n", word_num, word_letters, word, word_letters);
            word_num++;
            word_letters = 0;  
        } else {
            word_letters++;
        }
    }
    //prints the last word that is missed in the loop if the buffer is not empty
    if (isEmpty(buff) == 0){
        printf("%d. %.*s(%d)\n", word_num, word_letters, word, word_letters);
    } else {
        word_num-=1;
    }
    printf("\nNumber of words returned: %d\n", word_num);
}

void reverse(char *buff, int len, int str_len){
    int characters;
    //ensure characters in the string are not over the allowed amount
    if (str_len > len) {
        characters = len;
    } else {
        characters = str_len;
    }
    //This for loop iterates through half of the characters in the buffer to swap the letters
    //from front and back of the entire buffer. The front character is copied into a temporary
    //variable, so that it can be used to be copied into the back character.
    for (int i = 0; i < characters/2; i++){
        char front = *(buff + i);
        *(buff + i) = *(buff + characters - i - 1);
        *(buff + characters - i - 1) = front;
    }
}

void replace(char *buff, int len, int str_len, char *target, char *replace){
    int characters;
    int word_letters = 0;
    char *word = NULL; //used to store each individual word
    char *temp_tcounter = target; //used to count number of letters in the target word
    char *temp_rcounter = replace; //same purpose as above, but for the replacement word
    int target_letters = 0;
    int replace_letters = 0;
    int place_found = 0; //used to locate the index that the target was found at
    int found = 0; //used to determine whether or not the target word was found inside the buffer

    //This while loop is used to count the amount of letters in the target word
    while (*temp_tcounter != '\0'){
        temp_tcounter++;
        target_letters++;
    }
    //This while loop is used to count amount of letters in the replacement word
    while (*temp_rcounter != '\0'){
        temp_rcounter++;
        replace_letters++;
    }
    //ensure characters in the string are not over the allowed amount
    if (str_len > len) {
        characters = len;
    } else {
        characters = str_len;
    }
    
    //This for loop has the same logic as the one used in word_print, except instead of printing each word,
    //each letter of every word is compared to the target word's letters. To prevent out-of-bound errors, 
    //the words will only be compared when they have the same length of characters. The loop ends early
    //when the target word is found.
    for (int i = 0; i < characters; i++) {
        if (word_letters == 0) {
            word = buff + i;  
        }
        if (*(buff + i) == ' ' && (target_letters == word_letters)) {  
            for (int j = 0; j < word_letters; j++){
                if (*(word+j) != *(target+j)){
                    found = 0;
                } else {
                    found = 1;
                }
            }
            word_letters = 0;  
        } else if (*(buff + i) == ' '){
            word_letters = 0;
        } else {
            word_letters++;
        }
        if (found == 1){
            break;
        }
        place_found++;
    }
    //checks for the comparison of the last word, which was missed in the previous for loop
    if (target_letters == word_letters){
        for (int j = 0; j < word_letters; j++){
            if (*(word+j) != *(target+j)){
                found = 0;
            } else {
                found = 1;
            }
        }
    }
    //sets a new place found value if the target word is the last word in the buffer
    //one is added rather than subtracted to stay consistent with the previous for loop value of place_found
    //otherwises, an error message will showcase that the word is not found in the buffer
    if (found == 1 && place_found > characters){
        place_found = characters + 1;
    } else if (found == 0){
        printf("Not Implemented!\n");
        exit(3);
    }
    //The following conditional statements uses different approaches to replace the target letters with the replacement
    //letters depending on whether the length of their string are equal, less, or greater than one another
    if (target_letters == replace_letters) {
        //if both words have the same amount of letter, the following for loop directly
        //converts the letters of the target word to the replacement word's letters.
        for (int i = 0; i < target_letters; i++){
            *(buff + place_found - target_letters + i) = *(replace + i);  
        }
    }
    else if (target_letters > replace_letters){
        //if the target word has more letters than the replacement word, the first loop will replace as much of the target
        //word letters as possible with the replacement word letters. Then, the second for loop, will pad the remaining 
        //letters with spaces.
        for (int i = 0; i < replace_letters; i++){
            *(buff + place_found - target_letters + i) = *(replace + i);  
        }
        for (int i = 0; i < target_letters - replace_letters; i++){
            *(buff + place_found - target_letters + replace_letters + i) = ' ';
        }
        //used to finalize the buffer by removing the padding of spaces
        setup_buff(buff, buff, len);     
    }
    else {
        //if the target word has less letters than the replacement word, every letter after the target word
        //will be shifted to the right by the difference of the two words. The first for loop shifts every letter 
        //in the buffer, starting from the last index to avoid the need of a temporary variable to hold the letter.
        //After every letter is shifted, the second for loop will replace the target letters with the replacement letters.
        int shift_amount = replace_letters - target_letters;
        for (int i = characters - 1; i >= place_found; i--) {
            *(buff + i + shift_amount) = *(buff + i);
            }
        for (int i = 0; i < replace_letters; i++) {
            *(buff + place_found - target_letters + i) = *(replace + i);
        }
        //truncate letters that exceed the buffer limit
        if (characters + shift_amount > len){
            *(buff + characters) = '\0';
        }
    }
}

//function made to reduce repition in figuring out if an empty string was inputted by the user
int isEmpty(char *buff){
    while (*buff != '\0'){
        if (*buff != '.'){
            return 0;
        }
        buff++;
    }
    return 1;
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
            if (argc < 5){
                usage(argv[0]);
                exit(1);
            }
            char *target_word = argv[3];
            char *replacement_word = argv[4];
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
//          The length ensures that the functions do not exceed the amount of memory 
//           that the buffer can hold. This safety practice ensures that no accidental 
//           out-of-bound or segmentation core dump errors occur.