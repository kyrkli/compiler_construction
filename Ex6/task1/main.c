#include <stdio.h>
#include "dpda.h"
#include <string.h>

int main(){
	char word[128];
	printf("Please enter word: ");
	fgets(word, sizeof(word), stdin);
	word[strlen(word)-1] = '\0';
	struct dpda* dpda = create_dpda(1, dpda1_transition);
	struct dpda_ctx* ctx = dpda_new_ctx(dpda, word);
	int accepted = run_dpda(ctx);
	if(accepted)
		printf("Word %s accepted!\n", word);
	else
		printf("Word %s rejected.\n", word);
	return 0;
}
