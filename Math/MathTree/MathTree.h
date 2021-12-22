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

MathNode* TreeCopyRecursive(const MathNode* nodeSrc);

bool IsLeaf(const MathNode* node);

bool CompareTrees(const MathNode* node1, const MathNode* node2);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

MathNode* TreeFindObject(MathNode* node, const MathExpression object);

MathNode* TreeFindObjectStack(MathNode* node, const MathExpression object, Stack* stk);

MathNode* GetNodeFromStack(const Stack* stk, const size_t index);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeFindVariables(MathTree* problem);

bool TreeFindVariable(const MathTree* problem, const char variable);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

double TreeCalculate(const MathTree* problem);

double CalculateNode(const MathNode* node, const MathTree* problem, bool* canCalculate);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#ifdef GRAPHVIZ

bool CreateTreeGraph(const char* outImagefileName, const MathNode* node, const bool openFile = false);

#else

#define CreateTreeGraph(fileName, tree) false;

#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\


#endif
