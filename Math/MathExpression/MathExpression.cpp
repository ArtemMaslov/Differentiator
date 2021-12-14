#include <stdio.h>
#include <assert.h>
#include <string.h>


#include "MathExpression.h"
#include "..\..\StdParser\StdParser.h"


static int CheckStrForMathExpressionType(const char* ptr, const char* meStrings,
                                         size_t stringsCount, size_t stringsLength, size_t count);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool MathExpressionParseString(MathExpression* expr, const char* ptr, const size_t count)
{
    assert(expr);
    assert(ptr);

    int exprType = ME_UNKNOWN;
    expr->type = ME_UNKNOWN;
    
    // Операторы
    if ((exprType = CheckStrForMathExpressionType(ptr, MathOperatorNames[0],
                                      sizeof(MathOperatorNames)    / sizeof(MathOperatorNames[0]),
                                      sizeof(MathOperatorNames[0]) / sizeof(char),
                                      count)) >= 0)
    {
        expr->type = ME_OPERATOR;
        expr->me_operator = (MathOperator)exprType;
        return true;
    }

    // Число
    int parsedCount = 0;
    if (sscanf(ptr, "%lf%n", &expr->me_number, &parsedCount))
    {
        const char* unparsed = ptr + parsedCount;
        size_t unparsedCount = count - parsedCount;
        if (SkipInputString(unparsed, unparsedCount))
        {
            expr->type = ME_NUMBER;
            return true;
        }
    }

    // Константы
    if ((exprType = CheckStrForMathExpressionType(ptr, MathConstantNames[0],
                                      sizeof(MathConstantNames)    / sizeof(MathConstantNames[0]),
                                      sizeof(MathConstantNames[0]) / sizeof(char),
                                      count)) >= 0)
    {
        expr->type = ME_CONSTANT;
        expr->me_constant = (MathConstant)exprType;
        return true;
    }

    // Функции
    if ((exprType = CheckStrForMathExpressionType(ptr, MathFunctionNames[0],
                                      sizeof(MathFunctionNames)    / sizeof(MathFunctionNames[0]),
                                      sizeof(MathFunctionNames[0]) / sizeof(char),
                                      count)) >= 0)
    {
        expr->type = ME_FUNCTION;
        expr->me_function = (MathFunction)exprType;
        return true;
    }

    // Переменные
    if (sscanf(ptr, "%c", &expr->me_variable))
    {
        const char* unparsed = ptr + 1;
        size_t unparsedCount = count - 1;
        if (expr->me_variable >= 'a' && expr->me_variable <= 'z' &&
            SkipInputString(unparsed, unparsedCount))
        {
            expr->type = ME_VARIABLE;
            return true;
        }
    }
    
    return false;
}

bool MathExpressionEqual(MathExpression* expr1, MathExpression* expr2)
{
    assert(expr1);
    assert(expr2);

    if (expr1->type != expr2->type)
    {
        return false;
    }

    switch (expr1->type)
    {
        case ME_NUMBER:
            if (expr1->me_number == expr2->me_number)
                return true;
            break;
        case ME_CONSTANT:
            if (expr1->me_constant == expr2->me_constant)
                return true;
            break;
        case ME_OPERATOR:
            if (expr1->me_operator == expr2->me_operator)
                return true;
            break;
        case ME_VARIABLE:
            if (expr1->me_variable == expr2->me_variable)
                return true;
            break;
        case ME_FUNCTION:
            if (expr1->me_function == expr2->me_function)
                return true;
            break;
        default:
            break;
    }
    return false;
}

void PrintMathExpression(MathExpression* expr, FILE* file)
{
    assert(expr);
    assert(file);

    switch (expr->type)
    {
        case ME_NUMBER:
            fprintf(file, "%.6lg", expr->me_number);
            break;
        case ME_CONSTANT:
            fputs(MathConstantNames[expr->me_constant], file);
            break;
        case ME_OPERATOR:
            fputs(MathOperatorNames[expr->me_operator], file);
            break;
        case ME_VARIABLE:
            fputc(expr->me_variable, file);
            break;
        case ME_FUNCTION:
            fputs(MathFunctionNames[expr->me_function], file);
            break;
        default:
            fputs("UNKNOWN", file);
            break;
    }
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

static int CheckStrForMathExpressionType(const char* ptr, const char* meStrings,
                                          size_t stringsCount, size_t stringsLength, size_t count)
{
    assert(ptr);
    assert(meStrings);

    for (int st = 0; st < stringsCount; st++)
    {
        const char* _meStr = meStrings + st * stringsLength;
        if (strncmp(_meStr, ptr, count) == 0)
        {
            size_t strLen = strlen(_meStr);
            const char* unparsed = ptr + strLen;
            size_t unparsedCount = count - strLen;
            if (SkipInputString(unparsed, unparsedCount))
            {
                return st;
            }
        }
    }
    return -1;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\