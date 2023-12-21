#include "dfa.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){
	struct dfa *dfa = create_dfa(1, dfa1_transition);
	struct dfa_ctx *ctx = dfa_new_ctx(dfa, argv[1]);
	int accepted = run_dfa(ctx);
	if(accepted)
		printf("Word %s accepted!\n", argv[1]);
	else
		printf("Word %s rejected.\n", argv[1]);
	return 0;
}
