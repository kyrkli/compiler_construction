#include <stdlib.h>
#include "check.h"

int check(char *nr){
        if (atoi(nr) % 3 == 0) 
                return 1;
        else return 0;
}
