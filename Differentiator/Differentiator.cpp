#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "Differentiator.h"
#include "..\Math\MathTree\MathTree.h"
#include "..\Math\MathParser\MathParser.h"
#include "..\Logs\Logs.h"


static MathNode* DifferentiateNode(MathNode* nodeSrc, char diffVar);


bool DifferentiatorConstructor(Differentiator* diff, FILE* inputFile)
{
    if (!ReadTreeFromFile(&diff->tree, &diff->text, inputFile))
        return false;

    return true;
}

void DifferentiatorDestructor(Differentiator* diff)
{
    TreeDestructor(&diff->tree);

    TextDestructor(&diff->text);
}

MathTree* Differentiate(Differentiator* diff, char diffVar)
{
    MathTree* tree = (MathTree*)calloc(1, sizeof(MathTree));

    if (!tree)
        return nullptr;

    tree->root = DifferentiateNode(diff->tree.root, diffVar);
    return tree;
}

#include "DifferentiatorDSL.h"

static MathNode* DifferentiateNode(MathNode* nodeSrc, char diffVar)
{
    switch (nodeSrc->expression.type)
    {
        case ME_CONSTANT:
        case ME_NUMBER:
            return NUM(0);
        case ME_VARIABLE:
            if (IS_DIFF_VAR)
                return NUM(1);
            else
                return NUM(0);
        case ME_FUNCTION:
            switch (nodeSrc->expression.me_function)
            {
                case ME_SIN:
                    return COMPLEX_FUNC(FUNC(ME_COS, COPY(X)), X);
                case ME_COS:
                    return COMPLEX_FUNC(FUNC(ME_SIN, COPY(X)), X);
                case ME_TG:
                    return COMPLEX_FUNC(DIV( NUM(1), POW2(FUNC(ME_COS, COPY(X))) ), X);
                case ME_CTG:
                    return COMPLEX_FUNC(DIV( NUM(-1), POW2(FUNC(ME_SIN, COPY(X))) ), X);
                case ME_SH:
                    return COMPLEX_FUNC(FUNC(ME_CH, COPY(X)), X);
                case ME_CH:
                    return COMPLEX_FUNC(FUNC(ME_SH, COPY(X)), X);
                case ME_LN:
                    return COMPLEX_FUNC(DIV( NUM(1), COPY(X) ), X);
                case ME_SQRT:
                    return COMPLEX_FUNC(DIV( NUM(1), MUL(NUM(2), COPY(F)) ), X);
                case ME_CBRT:
                    return COMPLEX_FUNC(DIV( NUM(1), MUL( NUM(3), POW2(COPY(F)) ) ), X);
                case ME_ARCSIN:
                    return COMPLEX_FUNC(DIV( NUM(1), FUNC( ME_SQRT, SUB( NUM(1), POW2(COPY(X)) ) ) ), X);
                case ME_ARCCOS:
                    return COMPLEX_FUNC(DIV( NUM(-1), FUNC( ME_SQRT, SUB( NUM(1), POW2(COPY(X)) ) ) ), X);
                case ME_ARCTG:
                    return COMPLEX_FUNC(DIV( NUM(1), ADD( NUM(1), POW2(COPY(X)) ) ), X);
                case ME_ARCCTG:
                    return COMPLEX_FUNC(DIV( NUM(-1), ADD( NUM(1), POW2(COPY(X)) ) ), X);
                default:
                    return nullptr;
            }
            break;
        case ME_OPERATOR:
            switch (nodeSrc->expression.me_operator)
            {
                case ME_SUBTRACTION:
                    return SUB(DIF(LEFT), DIF(RIGHT));
                case ME_ADDITION:
                    return ADD(DIF(LEFT), DIF(RIGHT));
                case ME_MULTIPLICATION:
                    return ADD(MUL(DIF(LEFT), COPY(RIGHT)), MUL(DIF(RIGHT), COPY(LEFT)));
                case ME_DIVISION:
                    return DIV( SUB(MUL(DIF(LEFT), COPY(RIGHT)), MUL(DIF(RIGHT), COPY(LEFT))),
                                POW2(COPY(RIGHT)));
                case ME_POWER:
                    if (TYPE_EQUAL(RIGHT, ME_CONSTANT) ||
                        TYPE_EQUAL(RIGHT, ME_NUMBER))
                    {
                        return MUL( NUM(GET_NUM(RIGHT)), POW( COPY(LEFT), SUB(NUM(GET_NUM(RIGHT)), NUM(1)) ) );
                    }
                    else
                    {
                        return MUL(COPY(F), 
                            ADD( 
                            MUL( DIV(RIGHT, LEFT), DIF(LEFT) ), 
                            MUL( FUNC(ME_LN, COPY(LEFT)), DIF(RIGHT) )
                            ) );
                    }
                    break;
                default:
                    return nullptr;
            }
        default:
            return nullptr;
    }
}

#include "UndefDSL.h"

