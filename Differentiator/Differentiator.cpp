#include <stdio.h>
#include <assert.h>


#include "Differentiator.h"
#include "Parser.h"
#include "..\MathTree\MathTree.h"
#include "..\Logs\Logs.h"


bool DifferentiatorConstructor(Differentiator* diff, FILE* inputFile)
{
    TreeConstructor(&diff->tree);

    if (!ReadFile(&diff->text, inputFile))
        return false;

    ParseDiffTree(diff, &diff->text);

    return true;
}

void DifferentiatorDestructor(Differentiator* diff)
{
    TreeDestructor(&diff->tree);

    TextDestructor(&diff->text);
}

void Differentiate(Differentiator* diff)
{

}

void DifferentiateNode(MathNode* node)
{

}

