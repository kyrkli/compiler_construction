#ifndef VARIABLE_HANDLING_H
#define VARIABLE_HANDLING_H

#include "stack.h"

extern stack_t vars, globals;
extern int ast_id;
#define VAR_BORDER_FUNC 2
#define VAR_BORDER_BLOCK 1

void* var_declare_global (type_t type, char *id, void* gval, int size);
void* var_declare (type_t type, char *id, void* gval, int size);
void* var_set (char *id, void* gval, type_t expected_type, int index);
stackval_t var_get (char *id);
void var_enter_block (void);
void var_leave_block (void);
void var_enter_function (void);
void var_leave_function (void);
void var_dump (void);

void set_general_svar_zero(stackval_t* svar);
void var_declare_general_zero(stackval_t svar, char mod);
void var_declare_general_val(stackval_t svar, char mod);
void set_general_svar(stackval_t svar, int index);

#endif
