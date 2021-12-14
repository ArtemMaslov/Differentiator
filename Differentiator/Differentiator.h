#ifndef DIFFERENTIATOR_H_
#define DIFFERENTIATOR_H_


#include "..\Math\MathTree\MathTree.h"
#include "..\StringLibrary\StringLibrary.h"


struct Differentiator
{
    Text text;
    MathTree tree;
};


bool DifferentiatorConstructor(Differentiator* diff, FILE* inputFile);

void DifferentiatorDestructor(Differentiator* diff);

MathTree* Differentiate(Differentiator* diff, char diffVar);


#endif