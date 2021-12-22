#ifndef DIFFERENTIATOR_H_
#define DIFFERENTIATOR_H_


#include "..\Math\MathTree\MathTree.h"
#include "..\StringLibrary\StringLibrary.h"
#include "..\Latex\Latex.h"


struct Differentiator
{
    Text text;
    MathTree problem;
    MathTree answer;
};


bool DifferentiatorConstructor(Differentiator* diff, FILE* inputFile);

void DifferentiatorDestructor(Differentiator* diff);

bool Differentiate(Differentiator* diff, const char diffVar, Latex* latex);


#endif