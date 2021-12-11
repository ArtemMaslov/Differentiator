#ifndef STACK_PROTECTION_H_
#define STACK_PROTECTION_H_


#include "Stack.h"


#ifdef STACK_CRC
int64_t CalculateCRC(char *buf, size_t len);

inline bool CheckDataCRC(Stack *stack)
{
    if (stack)
    {
        if (stack->data && stack->dataCRC != CalculateCRC((char*)stack->data, stack->stackCapacity))
            return false;
        else
            return true;
    }
    return false;
}

inline bool CheckStackCRC(Stack *stack)
{
    if (stack)
    {
        int64_t stackCRC = stack->stackCRC;
        stack->stackCRC = 0;
        if (stackCRC != CalculateCRC((char*)(stack), sizeof(Stack)))
        {
            stack->stackCRC = stackCRC;
            return false;
        }
        else
        {
            stack->stackCRC = stackCRC;
            return true;
        }
    }
    return false;
}

inline void CalculateDataCRC(Stack *stack)
{
    if (stack->data)
        stack->dataCRC = CalculateCRC((char*)stack->data, stack->stackCapacity);
}

inline void CalculateStackCRC(Stack *stack)
{
    if (stack)
    {
        CalculateDataCRC(stack);
        stack->stackCRC = 0;
        stack->stackCRC = CalculateCRC((char*)(stack), sizeof(Stack));
    }
}
#endif


#endif