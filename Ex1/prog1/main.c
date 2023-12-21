#include <stdlib.h>
#include <stdio.h>
#include "check.h"

int main(void){

        FILE* fptr;
        char numeric_string[50];
        fptr = fopen("numbers.txt", "r");

        if (fptr == NULL) {
                printf("No txt file exists");
                exit(0);
        }       
        while(fgets(numeric_string, 50, fptr) != NULL){
		
                if (check(numeric_string))
			printf("The number %d can be divided by 3\n", atoi(numeric_string));
		

        }                

        fclose(fptr);

        return 0;
}       
