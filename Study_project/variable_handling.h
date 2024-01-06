#include "stack.h"

extern stack_t vars, globals;

#define VAR_BORDER_FUNC 2
#define VAR_BORDER_BLOCK 1

int var_declare_global (type_t type, char *id, value_t gval);
int var_declare (type_t type, char *id, value_t gval);
int var_set (char *id, value_t gval);
int var_get (char *id);
void var_enter_block (void);
void var_leave_block (void);
void var_enter_function (void);
void var_leave_function (void);
void var_dump (void);
