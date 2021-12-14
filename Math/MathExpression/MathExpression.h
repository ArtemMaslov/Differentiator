#ifndef MATH_EXPRESSION_H_
#define MATH_EXPRESSION_H_

const double Pi  = 3.14159265358979323846;
const double Exp = 2.71828182845904523536;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
//                                     ���������
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

// ��������� �������� ��� �������� � ����.
enum MathConstant
{
    ME_PI      = 0,
    ME_EXP     = 1,
};

const char MathConstantNames[][4] = 
{
    "pi",
    "exp"
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
//                                     ���������
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

enum MathOperator
{
    ME_ADDITION       = 0,
    ME_SUBTRACTION    = 1,
    ME_MULTIPLICATION = 2,
    ME_DIVISION       = 3,
    ME_POWER = 4,
};

const char MathOperatorNames[][2] =
{
    "+",
    "-",
    "*",
    "/",
    "^",
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
//                                ����������� �������
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

enum MathFunction
{
    ME_SIN     = 0,
    ME_COS     = 1,
    ME_TG      = 2,
    ME_CTG     = 3,
    ME_SH      = 4,
    ME_CH      = 5,

    ME_LN      = 6,
    ME_SQRT    = 7,
    ME_CBRT    = 8,

    ME_ARCSIN  = 9,
    ME_ARCCOS  = 10,
    ME_ARCTG   = 11,
    ME_ARCCTG  = 12,
};

const char MathFunctionNames[][7] = 
{
    "sin",
    "cos",
    "tg",
    "ctg",
    "sh",
    "ch",
    
    "ln",
    "sqrt",
    "cbrt",
    
    "arcsin",
    "arccos",
    "arctg",
    "arcctg"
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
//                             ��������������  ���������
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

enum MathExpressionType
{
    ME_UNKNOWN  = -1,
    ME_NUMBER   = 0,
    ME_CONSTANT    = 1,
    ME_VARIABLE = 2,
    ME_OPERATOR = 3,
    ME_FUNCTION = 4,
};

struct MathExpression
{
    union
    {    
        double       me_number;
        MathOperator me_operator;
        char         me_variable;
        MathConstant me_constant;
        MathFunction me_function;
    };
    MathExpressionType type;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
//                                ����������� �������
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool MathExpressionParseString(MathExpression* expr, const char* ptr, const size_t count);

bool MathExpressionEqual(MathExpression* expr1, MathExpression* expr2);

void PrintMathExpression(MathExpression* expr, FILE* file);


#endif