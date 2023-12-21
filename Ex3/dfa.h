struct dfa {
	int initial;
	int (*transition)(int state, char ch);
};

struct dfa_ctx {
	struct dfa *dfa;
	int state;
	char* input;
	int offset;
};



int dfa1_transition (int st, char ch);

struct dfa *create_dfa (int i, int (*t)(int, char));

struct dfa_ctx *dfa_new_ctx (struct dfa *dfa, char *word);

int run_dfa (struct dfa_ctx *ctx);

int delete_dfa_ctx (struct dfa_ctx *ctx);

int delete_dfa(struct dfa* dfa);
