#include <stdio.h>

// EXTRA CREDIT - print the drexel dragon from the readme.md
extern void print_dragon(){
  printf("\n");
  FILE *file = fopen("dragon", "r");
  //in case dragon file was not downloaded or in right location
  if (file == NULL){
      printf("Error: Dragon File not found");
  }
  char line[1024];  
  while (fgets(line, sizeof(line), file)) {
      printf("%s", line); 
  }
  fclose(file); 
  printf("\n");
}
