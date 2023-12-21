#include "struc.h"
#include "ringQueue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct Grammar* G;

int check_conventions(){

	return 0;
}

int check_all_terminals(char* word){
	if(word != NULL){
		for(int i = 0; i < strlen(word); i++){
			if(!(word[i] >= 97 && word[i] <= 122))
				return 0;
		}
		return 1;
	} 
	return 0;
}

int recognize_word(char* check_word){
	char* word;
	char* new_word;
	ringQueue_init();
	enqueue(G->X);
	while(!is_empty()){
		//1
		word = dequeue();

		//2
		if(check_all_terminals(word)){
			if(!strcmp(word, check_word))
				return 1;
		}
		//3
		for(int i = 0; i < G->SIZE_ROWS; i++){
			for(int j = 1; j< G->SIZE_COLUMNS; j++){
				new_word = replace(word, G->P[i][0], G->P[i][j], 0);
				if(new_word != NULL){
					if(strlen(new_word) <= strlen(check_word)){
						enqueue(new_word);
						strcpy(new_word, "");
					}
				}
			}
		}
	}
	free(new_word);
	ringQueue_deinit();
	return 0;


}

int print_all_words(){
	char* word;
	char* new_word;
	ringQueue_init();
	enqueue(G->X);
	while(!is_empty()){
		//1
		word = dequeue();

		//2
		if(check_all_terminals(word))
			printf("Word: %s\n", word);
		//3
		for(int i = 0; i < G->SIZE_ROWS; i++){
			for(int j = 1; j< G->SIZE_COLUMNS; j++){
				new_word = replace(word, G->P[i][0], G->P[i][j], 0);
				if(new_word != NULL){
					enqueue(new_word);
					strcpy(new_word, "");
				}
			}
		}
	}
	free(new_word);
	ringQueue_deinit();
	return 0;
}
void init_grammar(){
	G = (struct Grammar *) malloc(1 * sizeof(struct Grammar));
	G->SIZE_ROWS = 9;
	G->SIZE_COLUMNS = 3;
	
	G->E = (char*) malloc (11 * sizeof(char));
	strcpy(G->E, "abcpq");
	
	G->N = (char*) malloc (11 * sizeof(char));
	strcpy(G->N, "XY");
	
	G->P = (char***) malloc (G->SIZE_ROWS * sizeof(char**));
	for(int i = 0; i < G->SIZE_ROWS; i++)
		G->P[i] = (char**) malloc (G->SIZE_COLUMNS * sizeof(char*));
	
	for(int i = 0; i < G->SIZE_ROWS; i++)
		for(int j = 0; j < G->SIZE_COLUMNS; j++)
			G->P[i][j] = (char*) malloc (30 * sizeof(char));

	strcpy(G->P[0][0], "X");
	strcpy(G->P[0][1], "aBC");
	strcpy(G->P[0][2], "aSBC");
	
	strcpy(G->P[1][0], "CB");
	strcpy(G->P[1][1], "CZ");
	
	strcpy(G->P[2][0], "CZ");
	strcpy(G->P[2][1], "WZ");
	
	strcpy(G->P[3][0], "WZ");
	strcpy(G->P[3][1], "WC");
	
	strcpy(G->P[4][0], "WC");
	strcpy(G->P[4][1], "BC");
	
	strcpy(G->P[5][0], "aB");
	strcpy(G->P[5][1], "ab");
	
	strcpy(G->P[6][0], "bB");
	strcpy(G->P[6][1], "bb");
	
	strcpy(G->P[7][0], "bC");
	strcpy(G->P[7][1], "bc");
	
	strcpy(G->P[8][0], "cC");
	strcpy(G->P[8][1], "cc");
	


	G->X = (char*) malloc (11 * sizeof(char));
	strcpy(G->X, "X");
}

void delete_grammar(){
	free(G->E);
	free(G->N);
	
	
	for(int i = 0; i < G->SIZE_ROWS; i++)
		for(int j = 0; j < G->SIZE_COLUMNS; j++)
			free(G->P[i][j]);

	for(int i = 0; i < G->SIZE_ROWS; i++)
		free(G->P[i]);
	
	free(G->P);
	
	free(G->X);
	free(G);
}

char* replace (char *word, char *search, char *replace_word, int n){
	char* new_word = (char*) malloc(30 * sizeof(char));
	int tmp_n = -1;
	int start_replace_id = -1;
	int len_word = strlen(word);	
	int len_search = strlen(search);

	for(int i=0; i<len_word;i++) {
		for(int j=0; j<len_search && i < len_word; j++){
				
			if(word[i] == search[j]){
				if(start_replace_id == -1)
					start_replace_id = i;
				if(j+1 == len_search){
					tmp_n++;
				} else i++;
				
			} else {
				if(j!=0)
					i--;
				j = len_search;
				start_replace_id = -1;
				break;
			}
			
		} 
		if(tmp_n == n)
			break;
		else start_replace_id = -1;
		
	}
	if(start_replace_id == -1){
		return NULL;
	}

	strncpy(new_word, word, start_replace_id);//copy the previous part
	
	//concatenate the new(middle) part
	for(int i = start_replace_id, j = 0; j < strlen(replace_word); i++, j++){
		new_word[i] = replace_word[j];
	}
	
	for(int i = start_replace_id + strlen(search), j = strlen(new_word); i < strlen(word) ;i++, j++){
		new_word[j] = word[i];
	}
	return new_word;
}
