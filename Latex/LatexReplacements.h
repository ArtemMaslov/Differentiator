#ifndef LATEX_REPLACEMENTS_H_
#define LATEX_REPLACEMENTS_H_


#include "Latex.h"

const size_t ReplacementsMinSize = 15;

bool ReplacementsConstructor(Latex* latex);

void ReplacementsDestructor(Latex* latex);

void DoReplacement(MathNode* node, Latex* latex, const bool clearIndex = false);

void PrintReplacedNodes(Latex* latex);


#endif