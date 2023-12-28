#include "stack.h"

extern static stack_t vars, globals;

#define VAR_BORDER_FUNC 2
#define VAR_BORDER_BLOCK 1

int var_declare_global (char *id, int val);
int var_declare (char *id, int val);
int var_set (char *id, int val);
int var_get (char *id);
void var_enter_block (void);
void var_leave_block (void);
void var_enter_function (void);
void var_leave_function (void);
void var_dump (void);
