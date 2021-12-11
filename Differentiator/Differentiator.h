#ifndef DIFFERENTIATOR_H_
#define DIFFERENTIATOR_H_


#include "..\Tree\Tree.h"
#include "..\StringLibrary\StringLibrary.h"


struct Differentiator
{
    Text text;
    Tree tree;
};


void DifferentiatorConstructor(Differentiator* diff, FILE* inputFile);

void DifferentiatorDestructor(Differentiator* diff);


#endif