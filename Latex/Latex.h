#ifndef LATEX_H_
#define LATEX_H_


#include "..\Math\MathTree\MathTree.h"


enum LatexSentenceType
{
    LATEX_COMMON1  = 1,
    LATEX_COMMON2  = 2,
    LATEX_OPERATOR = 4
};

enum LatexSentenceSize
{
    LATEX_COMMON1_SIZE = 8,
    LATEX_COMMON2_SIZE = 5,
    LATEX_OPERATOR_SIZE = 3
};

extern const char* MathSentencesCommon1[];
extern const char* MathSentencesCommon2[];
extern const char* MathSentencesOperators[];

struct Latex
{
    FILE*  file;
    const char*  fileName;
    size_t problemCounter = 1;
    size_t formulaCounter = 1;
    size_t formulaSkipCounter = 0;
    MathNode** variableReplacement;
    size_t variableReplacementSize;
};

#define LATEX_ARRAY(arr) arr, (sizeof((arr)) / sizeof((arr)[0]))

const int ProbabilityMiss = 5; // 250/1000

const int FormulasSkipCount = 10;

const size_t MaxNodesCount = 15;

const size_t MaxNodesCountPerLine = 30;

bool OpenLatexArticle(Latex* latex, const char* FileName);

void CloseLatexArticle(Latex* latex);

void LatexMathProblem(Latex* latex, MathTree* problem);

void LatexFormatedString(Latex* latex, const char* format, ...);

void LatexString(Latex* latex, const char* str);

void LatexMathDiffFormula(Latex* latex, MathNode* lval, MathNode* rval);

void LatexRandSentence(Latex* latex, int sentenceType);

void LatexMathProblemAnswer(Latex* latex, MathTree* problem, MathTree* answer);

void LatexMathFormula(Latex* latex, MathNode* lval, MathNode* rval);


#endif