#ifndef MATH_TREE_OPTIMIZATIONS_H_
#define MATH_TREE_OPTIMIZATIONS_H_


#include "MathTreeGeneral.h"

void MathTreeOptimize(MathNode* problem);

bool OptimizeWorthlessTrees(MathNode* node);

bool MathTreeOptimizeCalculatedNodes(MathNode* node);

bool OptimizeWorthlessNodes(MathNode* node);


#endif