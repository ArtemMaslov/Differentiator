#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "Differentiator.h"
#include "..\Math\MathTree\MathTree.h"
#include "..\Math\MathParser\MathParser.h"
#include "..\Logs\Logs.h"
#include "..\Latex\LatexReplacements.h"


static MathNode* DifferentiateNode(MathNode* nodeSrc, char diffVar, Latex* latex);

static void DiffTreeOptimize(MathNode* problem, Latex* latex);


bool DifferentiatorConstructor(Differentiator* diff, FILE* inputFile)
{
    assert(diff);
    assert(inputFile);

    if (!ReadTreeFromFile(&diff->problem, &diff->text, inputFile))
        return false;

    return true;
}

void DifferentiatorDestructor(Differentiator* diff)
{
    assert(diff);

    TreeDestructor(&diff->problem);

    TextDestructor(&diff->text);
}

bool Differentiate(Differentiator* diff, char diffVar, Latex* latex)
{
    assert(diff);
    assert(latex);

    diff->answer.root = DifferentiateNode(diff->problem.root, diffVar, latex);
    if (diff->answer.root == nullptr)
        return false;
    return true;
}

#include "..\Math\MathExpression\MathDSL.h"

#define IS_DIFF_VAR diffVar == node->expression.me_variable

#define DIF(node)                                               \
    DifferentiateNode(node, diffVar, latex)

#define COMPLEX_FUNC(node, cmp_src)                             \
    NewOperatorNode(ME_MULTIPLICATION,                          \
                    (node),                                     \
                    DIF(cmp_src, diffVar))


static MathNode* DifferentiateNode(MathNode* node, char diffVar, Latex* latex)
{
    assert(node);
    //assert(latex);
    
    //CreateTreeGraph(GraphLogPath, node, false);
    MathNode* result = nullptr;

    switch (node->expression.type)
    {
        case ME_CONSTANT:
        case ME_NUMBER:
            result = NUM(0);
            LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
            LatexString(latex, "производная константы равна 0.");
            break;
        case ME_VARIABLE:
            if (IS_DIFF_VAR)
            {
                result = NUM(1);
                LatexRandSentence(latex, LATEX_COMMON1);
                LatexString(latex, "производная x равна 1.");
            }
            else
            {
                result = NUM(0);
                LatexRandSentence(latex, LATEX_COMMON2);
                LatexString(latex, "производная не дифференцируемой переменной принимается равной 0.");
            }
            break;
        case ME_FUNCTION:
            switch (node->expression.me_function)
            {
                case ME_SIN:
                    result = COMPLEX_FUNC(FUNC(ME_COS, COPY(X)), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная синуса равна $(\\sin{x})^\\prime = \\cos{x}$.");
                    break;
                case ME_COS:
                    result = COMPLEX_FUNC(FUNC(ME_SIN, COPY(X)), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная косинуса равна $(\\cos{x})^\\prime = -\\sin{x}$.");
                    break;
                case ME_TG:
                    result = COMPLEX_FUNC(DIV( NUM(1), POW2(FUNC(ME_COS, COPY(X))) ), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная тангенса равна $(\\tan{x})^\\prime = \\frac{1}{(\\cos{x})^2}$.");
                    break;
                case ME_CTG:
                    result = COMPLEX_FUNC(DIV( NUM(-1), POW2(FUNC(ME_SIN, COPY(X))) ), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная котангенса равна $(\\cot{x})^\\prime = \\frac{1}{(\\sin{x})^2}$.");
                    break;
                case ME_SH:
                    result = COMPLEX_FUNC(FUNC(ME_CH, COPY(X)), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная гиперболического синуса равна $(\\sinh{x})^\\prime = \\cosh{x}$.");
                    break;
                case ME_CH:
                    result = COMPLEX_FUNC(FUNC(ME_SH, COPY(X)), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная гиперболического косинуса равна $(\\cosh{x})^\\prime = \\sinh{x}$.");
                    break;
                case ME_LN:
                    result = COMPLEX_FUNC(DIV( NUM(1), COPY(X) ), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная натурального логарифма равна $(\\ln{x})^\\prime = \\frac{1}{x}$.");
                    break;
                case ME_SQRT:
                    result = COMPLEX_FUNC(DIV( NUM(1), MUL(NUM(2), COPY(F)) ), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная корня вычисляется по формуле $(\\sqrt{x})^\\prime = \\frac{1}{2\\sqrt{x}}$.");
                    break;
                case ME_CBRT:
                    result = COMPLEX_FUNC(DIV( NUM(1), MUL( NUM(3), POW2(COPY(F)) ) ), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная кубического корня легко берется. Достаточно лишь представить кубический корень в виде степени с рациональным показателем.");
                    break;
                case ME_ARCSIN:
                    result = COMPLEX_FUNC(DIV( NUM(1), FUNC( ME_SQRT, SUB( NUM(1), POW2(COPY(X)) ) ) ), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная $(\\arcsin{x})^\\prime = \\frac{1}{\\sqrt{1 - x^2}}$.");
                    break;
                case ME_ARCCOS:
                    result = COMPLEX_FUNC(DIV( NUM(-1), FUNC( ME_SQRT, SUB( NUM(1), POW2(COPY(X)) ) ) ), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная $(\\arccos{x})^\\prime = \\frac{-1}{\\sqrt{1 - x^2}}$.");
                    break;
                case ME_ARCTG:
                    result = COMPLEX_FUNC(DIV( NUM(1), ADD( NUM(1), POW2(COPY(X)) ) ), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, "производная $(\\arctan{x})^\\prime = \\frac{1}{1 + x^2}$.");
                    break;
                case ME_ARCCTG:
                    result = COMPLEX_FUNC(DIV( NUM(-1), ADD( NUM(1), POW2(COPY(X)) ) ), X);
                    LatexRandSentence(latex, LATEX_COMMON1 | LATEX_COMMON2);
                    LatexString(latex, " у производной акркотангенса очень красивая формула: $\\frac{-1}{1 + x^2}$.");
                    break;
            }
            break;
        case ME_OPERATOR:
            switch (node->expression.me_operator)
            {
                case ME_SUBTRACTION:
                    result = SUB(DIF(LEFT), DIF(RIGHT));
                    LatexRandSentence(latex, LATEX_OPERATOR);
                    LatexString(latex, "производная разности равна разности производных, \\sout{что не скажешь о производной частного}.");
                    break;
                case ME_ADDITION:
                    result = ADD(DIF(LEFT), DIF(RIGHT));
                    LatexRandSentence(latex, LATEX_OPERATOR);
                    LatexString(latex, "производная суммы просто равна сумме производных.");
                    break;
                case ME_MULTIPLICATION:
                    result = ADD(MUL(DIF(LEFT), COPY(RIGHT)), MUL(DIF(RIGHT), COPY(LEFT)));
                    LatexRandSentence(latex, LATEX_OPERATOR);
                    LatexString(latex, "производная произведения равна $(f(x) \\cdot g(x))^\\prime = (f(x))^\\prime \\cdot g(x) + (g(x))^\\prime \\cdot f(x)$.");
                    break;
                case ME_DIVISION:
                    result = DIV( SUB(MUL(DIF(LEFT), COPY(RIGHT)), MUL(DIF(RIGHT), COPY(LEFT))),
                                POW2(COPY(RIGHT)));
                    LatexRandSentence(latex, LATEX_OPERATOR);
                    LatexString(latex, "производная частного \\sout{имеет очень громоздкую формулу} равна $(f(x) \\cdot g(x))^\\prime = \\frac{(f(x))^\\prime \\cdot g(x) - (g(x))^\\prime \\cdot f(x)}{(g(x))^2}$.");
                    break;
                case ME_POWER:
                    if (TYPE_EQUAL(RIGHT, ME_CONSTANT) ||
                        TYPE_EQUAL(RIGHT, ME_NUMBER))
                    {
                        result = MUL(MUL( NUM(GET_NUM(RIGHT)), POW( COPY(LEFT), SUB(NUM(GET_NUM(RIGHT)), NUM(1)) ) ), DIF(LEFT) );
                        LatexRandSentence(latex, LATEX_COMMON2);
                        LatexString(latex, "производная степенной функции вычисляется легко по формуле: $(x^n)^\\prime = n \\cdot x^{n-1}$");
                    }
                    else
                    {
                        result = MUL(COPY(F), 
                            ADD( 
                            MUL( DIV(COPY(RIGHT), COPY(LEFT)), DIF(LEFT) ), 
                            MUL( FUNC(ME_LN, COPY(LEFT)), DIF(RIGHT) )
                            ) );
                        LatexRandSentence(latex, LATEX_COMMON2);
                        LatexString(latex, "производную функции, содержащей возведение в степень всегда неприятно считать! Ведь производная равна такому крокодилу: $(f(x)^{g(x)})^\\prime = f(x)^{g(x)} \\cdot (\\ln{f(x)} \\cdot g(x))^\\prime$");
                    }
                    break;
            }
            break;
    }
    if (result)
        LatexMathDiffFormula(latex, node, result);

    DiffTreeOptimize(result, latex);

    return result;
}

static void DiffTreeOptimize(MathNode* problem, Latex* latex)
{
    assert(problem);
    assert(latex);

    MathNode* copy = nullptr;
    bool result = false;
    do
    { 
        result = false;
        copy = TreeCopyRecursive(problem);
        while (MathTreeOptimizeCalculatedNodes(problem))
            result = true;
        while (OptimizeWorthlessNodes(problem))
            result = true;
        while (OptimizeWorthlessTrees(problem))
            result = true;
        if (result)
        {
            LatexString(latex, "\nПриведем подобные слагаемые.");
            LatexMathFormula(latex, copy, problem);
        }
        TreeNodeDestructor(copy);
    }
    while (result);

    fflush(latex->file);
}

#undef DIF
#undef COMPLEX_FUNC
#undef IS_DIFF_VAR

#include "..\Math\MathExpression\UndefMathDSL.h"

