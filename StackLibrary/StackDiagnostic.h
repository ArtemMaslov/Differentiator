#ifndef STACK_DIAGNOSTIC_H_
#define STACK_DIAGNOSTIC_H_


#include "Stack.h"

struct DumpInfo
{
    const char *stack_variable_name;
    const char *stack_function_name;
    const char *stack_file;
    const int   stack_line;
    const char *programm_function_name;
    const char *programm_file;
    const int   programm_line;
};

int IsStackEmpty(const Stack *stack);

bool CheckForError(const int errors, const StackError errorName);

bool IsStackBroken(const int errors, const Stack* stack);


#endif