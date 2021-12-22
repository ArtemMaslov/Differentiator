#ifndef MATH_PARSER_H_
#define MATH_PARSER_H_


#include "..\MathExpression\MathExpression.h"
#include "..\MathTree\MathTree.h"
#include "..\..\StringLibrary\StringLibrary.h"
#include "..\..\StdParser\StdParser.h"


bool ParseMathTree(const Text* text, MathTree* problem);

bool ReadTreeFromFile(MathTree* problem, Text* text, FILE* file);

void WriteTreeToFile(const MathTree* problem, FILE* file);


#endif