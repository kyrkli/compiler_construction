#include "ast.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "queue.h"

#define MAX_DECLARED_FUNC 10
//for keywords @$ = @1 etc with yylinenumber

Queue qparam;
Queue arr_el;

astnode_t* declared_functions[MAX_DECLARED_FUNC];

int cur_counter_func = 0;
int run_func = 0;
int ast_id = 0;

astnode_t *astnode_new (asttype_t type) { 
	astnode_t *a = calloc (sizeof *a, 1);
	a->type = type; 
	a->id = ++ast_id;
	return a; 
}

stackval_t execute_ast (astnode_t *root) {
	if(root == NULL)
		return (stackval_t) {};

	printf("\nstart of execute ast, node = %s,\t\tast_id = %d\n", node2str(root), root->id);
	//print_gdata(root->val.svar);
	switch (root->type) { 
		case PROG:
			execute_ast(root->child[0]);
			root->val.svar = execute_ast(root->child[1]);
			return root->val.svar;
		case GLOBAL:
			root->val.svar = execute_ast(root->child[0]);
			return root->val.svar;
		case GLVARDEF: 
			root->val.svar = execute_ast(root->child[0]);
			switch(root->val.svar.type){
				case _char:
				case _int:
				case _bool:
				case _double:
				case _charptr:
					break;
				case _intptr:
				case _boolptr:
					//create array
					root->val.svar.gval.intptr_val = createArray(root->val.svar.type, root->val.svar.size);
					
					//initialize array (if needed)
					if(!isEmpty(&arr_el))
						initArray(root->val.svar);
					
					break;
				case _doubleptr:
					root->val.svar.gval.doubleptr_val = createArray(root->val.svar.type, root->val.svar.size);
					
					if(!isEmpty(&arr_el))
						initArray(root->val.svar);
					
					break;

			}

			var_declare_general_val(root->val.svar, 'g');
			return root->val.svar;
		case BLOCK:
			var_enter_block();
			root->val.svar = execute_ast(root->child[0]);
			//var_leave_block();
			return root->val.svar;
		case SLOCAL:	
			root->val.svar = execute_ast(root->child[0]);
			execute_ast(root->child[1]);
			return root->val.svar;
		case FUNCDEF:
			//save the root of the ast as a declared function in programm
			if(!find_func(root->val.svar.id)){
				runtime_error(cur_counter_func < MAX_DECLARED_FUNC,
							  "The programm reached the maximum of declared functions, please decrease the number of functions or increase the maximum\n");	
				declared_functions[cur_counter_func++] = root;
			}

			//execute func (default 0)
			if(run_func){
				run_func = 0;
				var_enter_function();	
				var_declare_general_zero((stackval_t) {.id = "return", .type = root->val.svar.type}, 'l');
				
				//assign the given parameters
				execute_ast(root->child[0]);
				
				runtime_error(isEmpty(&qparam), 
							  "Too many arguments at the calling the function\n");

				//run the function block
				execute_ast(root->child[1]);
				//get the return value
				root->val.svar = var_get("return");
				var_leave_function();
			}
			return root->val.svar;
		case FPARAM:
			execute_ast(root->child[0]);
			
			//declared argument
			stackval_t arg = execute_ast(root->child[1]); //has type and id
			
			//get the type and value from the passed parameters queue as a declared argument
			stackval_t passed_arg = dequeue(&qparam); //has type and value, probably id
			runtime_error(passed_arg.type != _error,
						  "Too less arguments at the calling the function\n");
			runtime_error(arg.type == passed_arg.type,
						  "The used type of parameter for call function isn't matched to the declared parameter\n");
			
			//assign the value from passed argument in the queue to the declared argument
			arg.gval = passed_arg.gval;
			
			//declare new local variable
			var_declare_general_val(arg, 'l');

			return root->val.svar;
		case FUNCCALL:
			//get the arguments
			execute_ast(root->child[0]);
			
			//find func ast
			astnode_t *root_of_func = find_func(root->val.svar.id);
 			runtime_error(root_of_func != NULL,
						  "Usage of undeclared function\n");
			
			//run func and save the result of func
			run_func = 1;
			root->val.svar = execute_ast(root_of_func);
			return root->val.svar;
		case APARAM:
			execute_ast(root->child[0]);
		
			//get next parameter
			root->val.svar  = execute_ast(root->child[1]); //type and value, in some cases has also id
			enqueue(&qparam, root->val.svar);
			return root->val.svar;
		case SETARR:
			//get type and value for variable
			stackval_t index1 = execute_ast(root->child[0]);
			stackval_t newval1 = execute_ast(root->child[1]);
			
			runtime_error(index1.type == _int,
						  "Index of array must be an integer\n");

			newval1.id = root->val.svar.id;
			set_general_svar(newval1, index1.gval.int_val);
			root->val.svar = newval1;
			return root->val.svar;
		case GETARRVAL:
			root->val.svar = execute_ast(root->child[0]);
			return root->val.svar;
		case GETARR:
			//arr pointer
			root->val.svar = var_get(root->val.svar.id);
			
			stackval_t index = execute_ast(root->child[0]);

			runtime_error(index.type == _int,
						  "Index of array must be an integer\n");
			runtime_error(index.gval.int_val >= 0 && index.gval.int_val < root->val.svar.size,
						  "Index of array must be between 0 and size of the array\n");
			
			//get value
			switch(root->val.svar.type){
				case _boolptr:
				case _intptr:
					int* iarr = root->val.svar.gval.intptr_val;
					root->val.svar = (stackval_t) { .type = _int, .gval.int_val = iarr[index.gval.int_val]};
					break;
				case _doubleptr:
					double* darr = root->val.svar.gval.doubleptr_val;
					root->val.svar = (stackval_t) { .type = _double, .gval.double_val = darr[index.gval.int_val]};
					break;
				default:
					assert(0);
			}
			return root->val.svar;
		case SLOCAL2:
			execute_ast(root->child[0]);
			root->val.svar = execute_ast(root->child[1]);
			return root->val.svar;	
		case LOCAL:
			root->val.svar = execute_ast(root->child[0]);
			return root->val.svar;
		case LVARDEF:
			root->val.svar = execute_ast(root->child[0]);
			
			switch(root->val.svar.type){
				case _char:
				case _int:
				case _bool:
				case _double:
				case _charptr:
					break;
				case _intptr:
				case _boolptr:
					//create array
					root->val.svar.gval.intptr_val = createArray(root->val.svar.type, root->val.svar.size);
					
					//initialize array (if needed)
					if(!isEmpty(&arr_el))
						initArray(root->val.svar);
					
					break;
				case _doubleptr:
					root->val.svar.gval.doubleptr_val = createArray(root->val.svar.type, root->val.svar.size);
					
					if(!isEmpty(&arr_el))
						initArray(root->val.svar);
					
					break;

			}

			var_declare_general_val(root->val.svar, 'l');
			return root->val.svar;
		case VARDEF:
			root->val.svar = execute_ast(root->child[0]);
			set_general_svar_zero(&root->val.svar);
			return root->val.svar;
		case ASSVARDEF:
			execute_ast(root->child[0]);
			execute_ast(root->child[1]);
			if(root->child[0]->val.svar.type == _bool && root->child[1]->val.svar.type == _int){
				root->child[1]->val.svar.type = _bool;
				if(root->child[1]->val.svar.gval.int_val > 0)
					root->child[1]->val.svar.gval.int_val = 1;
				else
					root->child[1]->val.svar.gval.int_val = 0;
			}
			runtime_error(	root->child[0]->val.svar.type == root->child[1]->val.svar.type,
							"The type of the variable need to match the type of the assigning value\n");
			//assigning type, id to the root
			root->val.svar = root->child[0]->val.svar;
			//assigning value to the root
			if(root->val.svar.type == _charptr)
				root->val.svar.gval.charptr_val = strdup(root->child[1]->val.svar.gval.charptr_val);
			else
				root->val.svar.gval = root->child[1]->val.svar.gval;
			return root->val.svar;
		case SCANF:
			root->val.svar = my_scanf(execute_ast(root->child[0]), 0);
			return root->val.svar;
		case VARIABLE:
			return root->val.svar; 
		case GETIDVAL:
			root->val.svar = var_get(root->val.svar.id);	
			return root->val.svar;
		case BOOLTERM:
			root->val.svar = execute_ast(root->child[0]);
			return root->val.svar;
		case SCOMPARE:
			root->val.svar = execute_ast(root->child[0]);
			return root->val.svar;
		case IF:
			root->val.svar = execute_ast(root->child[0]);
			if(root->val.svar.gval.int_val)
				execute_ast(root->child[1]);
			return root->val.svar;
		case IFELSE:
			root->val.svar = execute_ast(root->child[0]);
			if(root->val.svar.gval.int_val)
				execute_ast(root->child[1]);
			else execute_ast(root->child[2]);
			return root->val.svar;	
		case RETURNCALL:
			root->val.svar = execute_ast(root->child[0]);	
			return root->val.svar;
		case COND:
			root->val.svar = execute_ast(root->child[0]);
			switch(root->val.svar.type){
				case _char:
					runtime_error(root->val.svar.gval.char_val == 'y' || root->val.svar.gval.char_val == 'n',
								 "char type in condition can be only 'y' or 'n'\n");
					if(root->val.svar.gval.char_val == 'y')
						root->val.svar.gval.int_val = 1;
					else root->val.svar.gval.int_val = 0;
					break;
				case _int:
					if(root->val.svar.gval.int_val)
						root->val.svar.gval.int_val = 1;
					else root->val.svar.gval.int_val = 0;
					break;
				case _bool:
					break;
				case _double:
					if(root->val.svar.gval.double_val)
						root->val.svar.gval.int_val = 1;
					else root->val.svar.gval.int_val = 0;
					break;
				case _charptr:
					if(root->val.svar.gval.charptr_val)
						root->val.svar.gval.int_val = 1;
					else root->val.svar.gval.int_val = 0;
					break;
			}
			root->val.svar.type = _bool;
			return root->val.svar;
		case FOR:
			//start
			stackval_t saved_start = execute_ast(root->child[0]); 
			
			stackval_t inc = execute_ast(root->child[2]);
		
			for(stackval_t run = saved_start; execute_ast(root->child[1]).gval.int_val > 0; ){
				//BLOCK
				execute_ast(root->child[3]);
				
				//increment	
				run = increment(saved_start.type, saved_start.id, inc.id[0], run, inc);	
			}

			root->val.svar = saved_start;
			return root->val.svar;
//TODO at the definition of new variables to check if the id is a keyword like PRINT or or smth else
//TODO void type for function return
		case STARTFOR:
			root->val.svar = execute_ast(root->child[0]);
			runtime_error(root->val.svar.type == _int || root->val.svar.type == _double || root->val.svar.type == _charptr,
						  "Start of FOR can be ONLY int, real or string");
			return root->val.svar;	
		case INC:
			return root->val.svar;	
		case WHILE:
			while(execute_ast(root->child[0]).gval.int_val > 0){
				execute_ast(root->child[1]);
			}
			return root->val.svar;
		case SETVAR:
			//get type and value for variable
			stackval_t newval = execute_ast(root->child[0]);
			newval.id = root->val.svar.id;
			set_general_svar(newval, 0);
			root->val.svar = newval;
			return root->val.svar;
		case PRINT:
			root->val.svar = execute_ast(root->child[0]);
			switch(root->val.svar.type){
				case _char:
					printf("%c\n", root->val.svar.gval.char_val);
					break;
				case _int:
				case _bool:
					printf("%d\n", root->val.svar.gval.int_val);
					break;
				case _double:
					printf("%f\n", root->val.svar.gval.double_val);
					break;
				case _charptr:
					printf("%s\n", root->val.svar.gval.charptr_val);
					break;
			}
			return root->val.svar;
		case ARRDEF:
			root->val.svar = execute_ast(root->child[0]); //get type id and size
			root->val.svar.type = set_type_arr(root->val.svar); //from int to int* for example
			return root->val.svar;
		case ARRASS:
			stackval_t type_id_len = execute_ast(root->child[0]);	
			
			//creating queue for assigned elements
			execute_ast(root->child[1]);
		
			runtime_error(type_id_len.type == peek(&arr_el).type,
						  "The type of the array must be matched to the assigned elements\n");
			
			root->val.svar = type_id_len;
			root->val.svar.type = set_type_arr(root->val.svar); //from int to int* for example
			return root->val.svar;
		case ARRAY:
			stackval_t type_id = execute_ast(root->child[0]);
			stackval_t size = execute_ast(root->child[1]);
			root->val.svar = type_id;
			root->val.svar.size = size.size;
			return root->val.svar;
		case LENGTH:
			stackval_t length = execute_ast(root->child[0]);
			
			runtime_error(length.type == _int,
						 "The length of array must have int data type\n");
			runtime_error(length.gval.int_val > 0,
						 "The minimum length of array must be 1\n");
			
			root->val.svar.size = length.gval.int_val;
			return root->val.svar;
		case ELEMENTS:
			stackval_t left_el = execute_ast(root->child[0]);
			stackval_t right_el = execute_ast(root->child[1]);
			
			runtime_error(left_el.type == right_el.type,
						 "The elements of the array must have the same data type\n");

			root->val.svar = right_el;	
			enqueue(&arr_el, root->val.svar);
			return root->val.svar;
		case ELEMENT:
			root->val.svar = execute_ast(root->child[0]);
			enqueue(&arr_el, root->val.svar);
			return root->val.svar;
		case STERM: 
			stackval_t saved = execute_ast(root->child[0]);
			
			runtime_error(root->val.type == saved.type,
						 "The type of a term at the moment of declaration or using need to match the calculated type of the term\n");
			root->val.svar = saved;
			return root->val.svar;
		case NUM:
			return root->val.svar;
		case CHAR:
			return root->val.svar;
		case STR:
			return root->val.svar;
		case MOD:
			root->val.svar = calc(execute_ast(root->child[0]), '%', execute_ast(root->child[1])); 
			return root->val.svar;
		case PLUS:
			root->val.svar = calc(execute_ast(root->child[0]), '+', execute_ast(root->child[1])); 
			return root->val.svar; 
		case MINUS:
			root->val.svar = calc(execute_ast(root->child[0]), '-', execute_ast(root->child[1])); 
			return root->val.svar;
		case MULT:
			root->val.svar = calc(execute_ast(root->child[0]), '*', execute_ast(root->child[1])); 
			return root->val.svar;
		case DIV:
			root->val.svar = calc(execute_ast(root->child[0]), '/', execute_ast(root->child[1])); 
			return root->val.svar;
		case EXP:
			root->val.svar = calc(execute_ast(root->child[0]), '^', execute_ast(root->child[1])); 
			return root->val.svar;
		case BOOLEAN:
			return root->val.svar;
		case OR:
			root->val.svar = calc(execute_ast(root->child[0]), 'o', execute_ast(root->child[1])); 
			return root->val.svar;
		case AND:
			root->val.svar = calc(execute_ast(root->child[0]), 'a', execute_ast(root->child[1])); 
			return root->val.svar;
		case XOR:
			root->val.svar = calc(execute_ast(root->child[0]), 'x', execute_ast(root->child[1])); 
			return root->val.svar;
		case NOT:
			root->val.svar = calc((stackval_t) { }, 'n', execute_ast(root->child[0])); 
			return root->val.svar;
		case GREATER:
			root->val.svar = compare(execute_ast(root->child[0]), '>', execute_ast(root->child[1]));
			return root->val.svar;
		case LESS:
			root->val.svar = compare(execute_ast(root->child[0]), '<', execute_ast(root->child[1]));
			return root->val.svar;
		case EQUAL:
			root->val.svar = compare(execute_ast(root->child[0]), '=', execute_ast(root->child[1]));
			return root->val.svar;
		case NOTEQ:
			root->val.svar = compare(execute_ast(root->child[0]), '!', execute_ast(root->child[1]));
			return root->val.svar;
		default: 
			assert(1);
	}
	return (stackval_t) {};
}

void* createArray(type_t type, int size) {
	void *arr;
	switch(type){
		case _intptr:
		case _boolptr:
			arr = (int *) malloc(size * sizeof(int));
			break;
		case _doubleptr:
			arr = (double *) malloc(size * sizeof(double));
			break;
		default:
			assert(0);
	}
	
	if (!arr) {
        perror("Memory allocation failed\n");
        exit(1);
    }
	return arr;
}

void initArray(stackval_t sarr){
	
	for(int i = 0; i < sarr.size; i++){
		runtime_error(!isEmpty(&arr_el),
					  "Too less elements for definition of array\n");
		switch(sarr.type){
			case _intptr:
				int* intArr = sarr.gval.intptr_val;
				intArr[i] = dequeue(&arr_el).gval.int_val;
				break;
			case _boolptr:
				int* boolArr = sarr.gval.intptr_val;
				boolArr[i] = dequeue(&arr_el).gval.int_val;
				break;
			case _doubleptr:
				double* doubleArr = sarr.gval.doubleptr_val;
				doubleArr[i] = dequeue(&arr_el).gval.double_val;
				break;
			default:
				assert(0);
		}
	}
	runtime_error(isEmpty(&arr_el),
				  "Too many elements for definition of array\n");
}

type_t set_type_arr(stackval_t arr){
	switch(arr.type){
		case _char:
			return _charptr;
		case _int:
			return _intptr;
		case _double:
			return _doubleptr;
		case _bool:
			return _boolptr;
		default:
			assert(0);
			return _error;
	}
}

char* node2str(astnode_t* node){
	switch(node->type){
		case PROG:
			return "PROG";
		case GETARRVAL:
			return "GETARRVAL";
		case GETARR:
			return "GETARR";
		case SETARR:
			return "SETARR";
		case ELEMENT:
			return "ELEMENT";
		case ELEMENTS:
			return "ELEMENTS";
		case ARRAY:
			return "ARRAY";
		case ARRDEF:
			return "ARRDEF";
		case ARRASS:
			return "ARRASS";
		case LENGTH:
			return "LENGTH";
		case MOD:
			return "MOD";
		case WHILE:
			return "WHILE";
		case SCANF:
			return "SCANF";
		case SETVAR:
			return "SETVAR";
		case RETURNCALL:
			return "RETURNCALL";
		case ASSVARDEF:
			return "ASSVARDEF";
		case VARIABLE:
			return "VARIABLE";
		case FOR:
			return "FOR";
		case INC:
			return "INC";
		case STARTFOR:
			return "STARTFOR";
		case LOCAL:
			return "LOCAL";
		case CHAR:
			return "CHAR";
		case SLOCAL:
			return "SLOCAL";
		case FUNCCALL:
			return "FUNCCALL";
		case FUNCDEF:
			return "FUNCDEF";
		case FPARAM:
			return "FPARAM";
		case APARAM:
			return "APARAM";
		case ARGS:
			return "ARGS";
		case SLOCAL2:
			return "SLOCAL2";
		case LVARDEF:
			return "LVARDEF";
		case PRINT:
			return "PRINT";
		case COND:
			return "COND";
		case IF:
			return "IF";
		case IFELSE:
			return "IFELSE";
		case GREATER:
			return "GREATER";
		case LESS:
			return "LESS";
		case EQUAL:
			return "EQUAL";
		case NOTEQ:
			return "NOTEQ";
		case BOOLEAN:
			return "BOOLEAN";
		case BOOLTERM:
			return "BOOLTERM";
		case SCOMPARE:
			return "SCOMPARE";
		case OR:
			return "OR";
		case AND:
			return "AND";
		case XOR:
			return "XOR";
		case NOT:
			return "NOT";
		case BLOCK:
			return "BLOCK";
		case VARDEF:
			return "VARDEF";
		case GLOBAL:
			return "GLOBAL";
		case GLVARDEF:
			return "GLVARDEF";
		case STERM:
			return "STERM";
		case STR:
			return "STR";
		case NUM:
			return "NUM";
		case GETIDVAL:
			return "GETIDVAL";
		case PLUS:
			return "PLUS";
		case MINUS:
			return "MINUS";
		case MULT:
			return "MULT";
		case DIV:
			return "DIV";
		case EXP:
			return "EXP";
	}
}

void print_ast(astnode_t *root, int depth) {
	static FILE *dot; 
	if (depth == 0) { 
		dot = fopen("ast.gv", "w"); 
		fprintf(dot, "digraph ast {\n"); 
	} // Create graph node 
	fprintf(dot, " n%d [label=\"%s\"]\n", root->id, node2str(root)); 
	for (int i = 0; i < MAXCHILDREN; i++) {
			if (root->child[i]) { 
				// Create graph edge
				fprintf(dot, " n%d -> n%d\n", root->id, root->child[i]->id);
				print_ast(root->child[i], depth+1);
			} 
	} 

	if (depth == 0) { 
		fprintf(dot, "}\n"); 
		fclose(dot); 
	} 
}

stackval_t compare(stackval_t num1, char op, stackval_t num2){
	runtime_error(num1.type == num2.type, "Only the same types of the term can be compared\n");
	stackval_t result = {.type = _bool};
	switch(num1.type){
		case _char:
			switch(op){
				case '>':
					result.gval.int_val = (num1.gval.char_val > num2.gval.char_val) ? 1 : 0;
					return result;
				case '<':
					result.gval.int_val = (num1.gval.char_val < num2.gval.char_val) ? 1 : 0;
					return result;
				case '=':
					result.gval.int_val = (num1.gval.char_val == num2.gval.char_val) ? 1 : 0;
					return result;
				case '!':
					result.gval.int_val = (num1.gval.char_val != num2.gval.char_val) ? 1 : 0;
					return result;
			}
			break;
		case _int:
		case _bool:
			switch(op){
				case '>':
					result.gval.int_val = (num1.gval.int_val > num2.gval.int_val) ? 1 : 0;
					return result;
				case '<':
					result.gval.int_val = (num1.gval.int_val < num2.gval.int_val) ? 1 : 0;
					return result;
				case '=':
					result.gval.int_val = (num1.gval.int_val == num2.gval.int_val) ? 1 : 0;
					return result;
				case '!':
					result.gval.int_val = (num1.gval.int_val != num2.gval.int_val) ? 1 : 0;
					return result;
			}
			break;
		case _double:
			switch(op){
				case '>':
					result.gval.int_val = (num1.gval.double_val > num2.gval.double_val) ? 1 : 0;
					return result;
				case '<':
					result.gval.int_val = (num1.gval.double_val < num2.gval.double_val) ? 1 : 0;
					return result;
				case '=':
					result.gval.int_val = (num1.gval.double_val == num2.gval.double_val) ? 1 : 0;
					return result;
				case '!':
					result.gval.int_val = (num1.gval.double_val != num2.gval.double_val) ? 1 : 0;
					return result;
			}
			break;
		case _charptr:
			switch(op){
				case '>':
					result.gval.int_val = (strcmp(num1.gval.charptr_val, num2.gval.charptr_val) > 0) ? 1 : 0;
					return result;
				case '<':
					result.gval.int_val = (strcmp(num1.gval.charptr_val, num2.gval.charptr_val) < 0) ? 1 : 0;
					return result;
				case '=':
					result.gval.int_val = (strcmp(num1.gval.charptr_val, num2.gval.charptr_val) == 0) ? 1 : 0;
					return result;
				case '!':
					result.gval.int_val = (strcmp(num1.gval.charptr_val, num2.gval.charptr_val) != 0) ? 1 : 0;
					return result;
			}
			break;
	}

}

stackval_t calc(stackval_t num1, char op, stackval_t num2){
	if(num1.type == _bool && num2.type != _int)
		num2.type = _bool;
	if(num2.type == _bool && num1.type != _int)
		num1.type = _bool;
	runtime_error(num1.type == num2.type, "Only the same types of the term can be calculated\n");
	
	if(num1.type == _bool){
		switch(op){
		case 'o':
			return (stackval_t) { .type = _bool, .gval.int_val = num1.gval.int_val || num2.gval.int_val};
		case 'a':
			return (stackval_t) { .type = _bool, .gval.int_val = num1.gval.int_val && num2.gval.int_val};
		case 'x':
			return (stackval_t) { .type = _bool, .gval.int_val = num1.gval.int_val != num2.gval.int_val};
		case 'n':
			return (stackval_t) { .type = _bool, .gval.int_val = !num2.gval.int_val};
		default :
			assert(0);
		}
	}
	if(num1.type == _int){
		switch(op){
		case '+':
			return (stackval_t) { .type = _int, .gval.int_val = num1.gval.int_val + num2.gval.int_val};
		case '-':
			return (stackval_t) { .type = _int, .gval.int_val = num1.gval.int_val - num2.gval.int_val};
		case '*':
			return (stackval_t) { .type = _int, .gval.int_val = num1.gval.int_val * num2.gval.int_val};
		case '/':
			return (stackval_t) { .type = _int, .gval.int_val = num1.gval.int_val / num2.gval.int_val};
		case '^':
			return (stackval_t) { .type = _int, .gval.int_val = (int) pow(num1.gval.int_val, num2.gval.int_val)};
		case '%':
			return (stackval_t) { .type = _int, .gval.int_val = num1.gval.int_val % num2.gval.int_val};
		default :
			assert(0);
		}
	} 
	if(num1.type == _double){
		switch(op){
		case '+':
			return (stackval_t) { .type = _double, .gval.double_val = num1.gval.double_val + num2.gval.double_val};
		case '-':
			return (stackval_t) { .type = _double, .gval.double_val = num1.gval.double_val - num2.gval.double_val};
		case '*':
			return (stackval_t) { .type = _double, .gval.double_val = num1.gval.double_val * num2.gval.double_val};
		case '/':
			return (stackval_t) { .type = _double, .gval.double_val = num1.gval.double_val / num2.gval.double_val};
		case '^':
			return (stackval_t) { .type = _double, .gval.double_val = pow(num1.gval.double_val, num2.gval.double_val)};
		case '%':
			runtime_error(0,
						  "Operator modulo can be used only with int type\n");
		default :
			assert(0);
		}
	}
	return (stackval_t) {};
}

stackval_t increment(type_t type, char* id, char op, stackval_t base, stackval_t inc){
	switch(op){
		case '+':
			if(type == _int){
				var_set(id, & (int) {base.gval.int_val + inc.gval.int_val}, type, 0);
				base.gval.int_val = base.gval.int_val + inc.gval.int_val;
				return base;
			} else {
				var_set(id, & (double) {base.gval.double_val + inc.gval.double_val}, type, 0);
				base.gval.double_val = base.gval.double_val + inc.gval.double_val;
				return base;
			}
		case '-':
			if(type == _int){
				var_set(id, & (int) {base.gval.int_val - inc.gval.int_val}, type, 0);
				base.gval.int_val = base.gval.int_val - inc.gval.int_val;
				return base;
			} else {
				var_set(id, & (double) {base.gval.double_val - inc.gval.double_val}, type, 0);
				base.gval.double_val = base.gval.double_val - inc.gval.double_val;
				return base;
			}
		case '*':
			if(type == _int){
				var_set(id, & (int) {base.gval.int_val * inc.gval.int_val}, type, 0);
				base.gval.int_val = base.gval.int_val * inc.gval.int_val;
				return base;
			} else {
				var_set(id, & (double) {base.gval.double_val * inc.gval.double_val}, type, 0);
				base.gval.double_val = base.gval.double_val * inc.gval.double_val;
				return base;
			}
		case '/':
			if(type == _int){
				var_set(id, & (int) {base.gval.int_val / inc.gval.int_val}, type, 0);
				base.gval.int_val = base.gval.int_val / inc.gval.int_val;
				return base;
			} else {
				var_set(id, & (double) {base.gval.double_val / inc.gval.double_val}, type, 0);
				base.gval.double_val = base.gval.double_val / inc.gval.double_val;
				return base;
			}
	}
}

stackval_t my_scanf(stackval_t data, int index){
	int isSuccess = 0;
	switch(data.type){
		case _char:
			printf("Enter a char: ");
			isSuccess = scanf("%c", &data.gval.char_val);
			
			runtime_error(isSuccess == 1,
						  "Invalid input\n");
			
			printf("You entered: %c\n", data.gval.char_val);
			set_general_svar(data, index);
			break;
		case _int:
			printf("Enter an integer: ");
			isSuccess = scanf("%d", &data.gval.int_val);
			
			runtime_error(isSuccess == 1,
						  "Invalid input\n");
		
			printf("You entered: %d\n", data.gval.int_val);
			set_general_svar(data, index);
			break;		
		case _bool:
			printf("Enter a bool (0 or 1): ");
			isSuccess = scanf("%d", &data.gval.int_val);
			
			runtime_error(isSuccess == 1,
						  "Invalid input\n");
			runtime_error(data.gval.int_val == 0 || data.gval.int_val == 1,
						  "Bool value can be filled only with 0 or 1\n");	
			
			printf("You entered: %d\n", data.gval.int_val);
			set_general_svar(data, index);
			break;
		case _double:
			printf("Enter a real: ");
			isSuccess = scanf("%lf", &data.gval.double_val);
			
			runtime_error(isSuccess == 1,
						  "Invalid input\n");
	
			printf("You entered: %lf\n", data.gval.double_val);
			set_general_svar(data, index);
			break;
		case _charptr:
			printf("Enter a string: ");
			isSuccess = scanf("%s", data.gval.charptr_val);
			
			runtime_error(isSuccess == 1,
						  "Invalid input\n");
			
			printf("You entered: %s\n", data.gval.charptr_val);
			set_general_svar(data, index);
			break;
	}
	return data;
}

astnode_t* find_func(char* id){
	for(int i = 0; i < cur_counter_func; i++){
		if(!strcmp(declared_functions[i]->val.svar.id, id))
			return declared_functions[i];
	}
	//not found
	return NULL;
}
