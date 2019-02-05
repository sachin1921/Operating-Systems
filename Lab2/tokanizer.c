#include <string.h>
#include <stdio.h>

int main () {
   char str[80] = "Hello this is a string";
   char * tokens;

   tokens = strtok(str, " ");

   while(tokens != NULL){
   	printf("%s\n", tokens);
   	tokens = strtok(NULL, " ");
   }

}