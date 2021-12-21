#ifndef MY_TREE_H
#define MY_TREE_H


#include "..\..\StackLibrary\Stack.h"
#include "..\MathExpression\MathExpression.h"
#include "MathTreeGeneral.h"
#include "MathTreeOptimizations.h"


///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeConstructor(MathTree* problem);

MathNode* TreeNodeConstructor(const MathExpression* expression);

bool TreeDestructor(MathTree* problem);

bool TreeNodeDestructor(MathNode* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

void TreeAddRightNode(MathNode* parent, MathNode* child);

void TreeAddLeftNode(MathNode* parent, MathNode* child);

size_t TreeMeasure(MathNode* node);

MathNode* TreeCopyRecursive(MathNode* nodeSrc);

bool IsLeaf(MathNode* node);

bool CompareTrees(MathNode* node1, MathNode* node2);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

MathNode* TreeFindObject(MathNode* node, MathExpression object);

MathNode* TreeFindObjectStack(MathNode* node, MathExpression object, Stack* stk);

MathNode* GetNodeFromStack(Stack* stk, size_t index);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeFindVariables(MathTree* problem);

bool TreeFindVariable(MathTree* problem, char variable);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

double TreeCalculate(MathTree* problem);

double CalculateNode(MathNode* node, MathTree* problem, bool* canCalculate);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#ifdef GRAPHVIZ

bool CreateTreeGraph(const char* outImagefileName, MathNode* node, bool openFile = false);

#else

#define CreateTreeGraph(fileName, tree) false;

#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\


#endif
