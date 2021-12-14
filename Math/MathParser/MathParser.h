#ifndef MATH_PARSER_H_
#define MATH_PARSER_H_


#include "..\MathExpression\MathExpression.h"
#include "..\MathTree\MathTree.h"
#include "..\..\StringLibrary\StringLibrary.h"
#include "..\..\StdParser\StdParser.h"


bool ParseMathTree(Text* text, MathTree* tree);

bool ReadTreeFromFile(MathTree* tree, Text* text, FILE* file);

void WriteTreeToFile(MathTree* tree, FILE* file);


#endif