#include <stdio.h>
#include "struc.h"

int main(int argc, char** argv){
	init_grammar();
	
	if (argc == 2){
		char* word = argv[1];
		if(recognize_word(word))
			printf("\"%s\" is recognized!\n");
		else printf("\"%s\" is not recognized.\n");
	}
	else if (argc == 1) {
		print_all_words();
	}
	
	delete_grammar();
	return 0;
}
