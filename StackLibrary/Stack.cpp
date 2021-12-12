#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <cmath>


#include "..\Logs\Logs.h"
#include "Stack.h"
#include "StackProtection.h"
#include "StackDiagnostic.h"


#ifdef  STACK_LOGS

FILE* stackLogFile = nullptr;

#endif //  StackLogs

static Stack* StackResize(Stack *stack, int *error);

static size_t CalculateDecreasedCapacity(size_t oldCapacity, size_t stackSize, bool* shouldResize);

void StackLogConstructor(FILE* file)
{
    LogConstructor("Stack.log", "StackLibrary");
}


int StackConstructor(Stack *stack, size_t elementSize, size_t Capacity)
{
#ifdef STACK_LOGS
    assert(stackLogFile);

    LogLine(stackLogFile, "StackConstructor", DEBUG);
#endif

    if (stack == nullptr)
    {
#ifdef STACK_LOG_ERRORS
        LogLine(stackLogFile, "ERROR: Stack ptr is null", ERROR);
#endif
        StackDump(stack, stackLogFile);
        return STACKERR_PTR_IS_NULL;
    }

    int error = 0;
    error |= IsStackEmpty(stack);
    error |= (elementSize > 0) ? STACKERR_NO_ERRORS : STACKERR_ELEM_SIZE_INVALIDE;

    if (error == STACKERR_NO_ERRORS)
    {
        stack->elementSize   = elementSize;
        stack->stackCapacity = Capacity;
        stack->stackSize     = 0;
        
#ifdef STACK_CANARIES
        stack->canaryLeft = STACK_LEFT_CANARY_VALUE;
        stack->canaryRight = STACK_RIGHT_CANARY_VALUE;
#endif

        if (stack->stackCapacity > 0)
        {
            stack->stackCapacity = (stack->stackCapacity < STACK_MIN_CAPACITY) ? STACK_MIN_CAPACITY : stack->stackCapacity;
            stack->data = calloc(Capacity, stack->elementSize * Capacity);
        }

#ifdef STACK_CRC
        CalculateStackCRC(stack);
#endif
    }
    else
        StackDump(stack, stackLogFile);

    return error;
}

int StackDestructor(Stack *stack)
{
#ifdef STACK_LOGS
    LogLine(stackLogFile, "StackDestructor", DEBUG);
#endif

    if (stack == nullptr)
    {
#ifdef STACK_LOG_ERRORS
        LogLine(stackLogFile, "ERROR: Stack ptr is null", ERROR);
#endif
        StackDump(stack, stackLogFile);
        return STACKERR_PTR_IS_NULL;
    }

    if (stack->data)
        free((char*)stack->data - sizeof(int64_t));
    
    memset(stack, 0, sizeof(Stack));

    return STACKERR_NO_ERRORS;
}

int StackPush(Stack *stack, void *expression)
{
#ifdef STACK_LOGS
    LogLine(stackLogFile, "StackPush", DEBUG);
#endif

    int error = ValidateStack(stack);

    if (expression == nullptr)
    {
#ifdef STACK_LOG_ERRORS
    LogLine(stackLogFile, "ERROR: Trying to push null value", ERROR);
#endif
        StackDump(stack, stackLogFile);
        return STACKERR_NULL_VALUE;
    }

    if (IsStackBroken(error, stack))
        return error;

    stack = StackResize(stack, &error);

    if (stack)
    {
        memmove((char*)stack->data + stack->elementSize * stack->stackSize++, expression, stack->elementSize);
#ifdef STACK_CRC
        CalculateStackCRC(stack);
#endif
    }

    error |= ValidateStack(stack);

    if (error > 0)
        StackDump(stack, stackLogFile);

    return error;
}

void* StackPop(Stack *stack, int *error)
{
#ifdef STACK_LOGS
    LogLine(stackLogFile, "StackPop", DEBUG);
#endif

    int _error = ValidateStack(stack);

    if (IsStackBroken(_error, stack))
    {
        if (error)
            *error = _error;
        return nullptr;
    }

    if (stack->stackSize == 0)
    {
#ifdef STACK_LOG_ERRORS
        LogLine(stackLogFile, "ERROR: Stack is empty", ERROR);
#endif
        if (error)
            *error |= STACKERR_STACK_IS_EMPTY;
        //StackDump(stack, stackLogFile);
        return nullptr;
    }
    
    stack->stackSize--;

    stack = StackResize(stack, error);

#ifdef STACK_CRC
    CalculateStackCRC(stack);
#endif
    
    _error |= ValidateStack(stack);
    if (error)
        *error |= _error;

    if (_error > 0)
        StackDump(stack, stackLogFile);

    return (char*)stack->data + (stack->elementSize * stack->stackSize);
}

void* StackGetElemAt(Stack* stack, size_t index)
{
    if (index > stack->stackSize)
        return nullptr;
    
    return (char*)stack->data + index * stack->elementSize;
}

static Stack* StackResize(Stack *stack, int *error)
{
#ifdef STACK_LOGS
    LogLine(stackLogFile, "StackResize", DEBUG);
#endif

    assert(stack);

    size_t oldCapacity = stack->stackCapacity;
    bool shouldResize = false;

    if (stack->stackCapacity == 0)
    {
        stack->stackCapacity = STACK_MIN_CAPACITY;
        shouldResize = true;
    }
    //else if (stack->stackSize == 0)
    //{
    //    stack->stackCapacity = 0;
    //    shouldResize = true;
    //}
    else if (stack->stackSize >= stack->stackCapacity)
    {
        if (stack->stackSize < stack->stackCapacity * 2)
            stack->stackCapacity *= STACK_CAPACITY_SCALE_COEFFICIENT;
        else
            stack->stackCapacity = stack->stackSize;
        shouldResize = true;
    }
#ifdef STACK_CANDECREASE_CAPACITY
    else
    {
        /*int rawCapacity = (stack->stackCapacity / STACK_CAPACITY_SCALE) - STACK_CAPACITY_DECREASE_DELTA;
        size_t newCapacity = rawCapacity < 0 ? 0 : rawCapacity;
        if (stack->stackSize < newCapacity)
        {
            stack->stackCapacity = newCapacity < STACK_MIN_CAPACITY ? STACK_MIN_CAPACITY : newCapacity;
            shouldResize = true;
        }*/
        stack->stackCapacity = CalculateDecreasedCapacity(stack->stackCapacity, stack->stackSize, &shouldResize);
    }
#endif // STACK_CANDECREASE_CAPACITY
        
    if (shouldResize)
    {
        char *dataPtr = nullptr;
        
        if (stack->data)
            dataPtr = (char*)stack->data - sizeof(int64_t);

        void *reallocResult = 
            realloc(dataPtr, sizeof(int64_t) + stack->stackCapacity * stack->elementSize + sizeof(int64_t));

        if (reallocResult == nullptr)
        {
#ifdef STACK_LOG_ERRORS
            LogLine(stackLogFile, "ERROR: Stack no memory", ERROR);
#endif
            if (error)
                *error |= STACKERR_NO_MEMORY;
            StackDump(stack, stackLogFile);
            return nullptr;
        }

        stack->data = (char*)reallocResult + sizeof(int64_t);
        if (stack->stackCapacity > oldCapacity)
            memset((char*)stack->data + oldCapacity * stack->elementSize, 0, (stack->stackCapacity - oldCapacity) * stack->elementSize);

        ((int64_t*)( (char*)stack->data - sizeof(int64_t) ))[0] = STACK_LEFT_CANARY_VALUE;

        ((int64_t*)( (char*)stack->data + stack->stackCapacity * stack->elementSize ))[0] = STACK_RIGHT_CANARY_VALUE;
    }

    return stack;
}

static size_t CalculateDecreasedCapacity(size_t oldCapacity, size_t stackSize, bool* shouldResize)
{
    size_t proportionalCapacity = (oldCapacity * (0.5 - STACK_CAPACITY_DECREASE_COEFFICIENT)) > 0 ? 
                                   (size_t)(oldCapacity * (0.5 - STACK_CAPACITY_DECREASE_COEFFICIENT)) : 0;
    size_t deltaCapacity        = (oldCapacity / 2.0 - STACK_MIN_CAPACITY) > 0 ? (size_t)(oldCapacity / 2.0 - STACK_MIN_CAPACITY) : 0;
    size_t calculatedCapacity   = proportionalCapacity < deltaCapacity ? proportionalCapacity : deltaCapacity;

    calculatedCapacity = calculatedCapacity < STACK_MIN_CAPACITY ? STACK_MIN_CAPACITY : calculatedCapacity;
    if (calculatedCapacity > stackSize)
    {
        if (calculatedCapacity != oldCapacity)
            *shouldResize = true;
        return calculatedCapacity;
    }
    else
        return oldCapacity;
}
