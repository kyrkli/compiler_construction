struct Grammar{
	char* E;
	char* N;
	char*** P;
	char* X;
	int SIZE_ROWS;
	int SIZE_COLUMNS;
};

void init_grammar();

void delete_grammar();

int check_conventions();

int print_all_words();

int recognize_word(char* check_word);

char* replace (char *word, char *search, char *replace_word, int n);
