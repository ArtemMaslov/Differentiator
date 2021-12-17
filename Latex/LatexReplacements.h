#ifndef LATEX_REPLACEMENTS_H_
#define LATEX_REPLACEMENTS_H_


#include "Latex.h"

void DoReplacement(MathNode* node, Latex* latex, bool clearIndex = false);

void PrintReplacedNodes(Latex* latex);


#endif