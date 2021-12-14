#ifndef MY_TREE_H
#define MY_TREE_H


#include "..\..\StackLibrary\Stack.h"
#include "..\MathExpression\MathExpression.h"


/// Включает построение графа по базе данных.
#define GRAPHVIZ

/// Обычно используются не более трёх переменных (x, y, z).
const size_t MATH_AVERAGE_VAR_COUNT = 3;

/// Если в дереве будет обнаружено переменных больше, чем MATH_AVERAGE_VAR_COUNT,
/// то размер массивов будет увеличен в MATH_VAR_COUNT_SCALE раз
const size_t MATH_VAR_COUNT_SCALE   = 2;

struct MathNode
{
    /// Значение узла.
    MathExpression expression;
    /// Родитель узла.
    MathNode* parent;
    /// Левый потомок.
    MathNode* nodeLeft;
    /// Правый потомок.
    MathNode* nodeRight;
};

struct MathTree
{
    /// Корень дерева.
    MathNode* root;
    /// Наибольшее количество узлов дерева в глубину, чтобы изменить нужно вызвать TreeMeasure().
    size_t treeLength;

    /// Переменные, используемые в дереве
    char*   variables;
    /// Значения переменных, используемых в дереве
    double* values;
    /// Количество переменных, используемых в дереве
    size_t  varCapacity;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeConstructor(MathTree* tree);

MathNode* TreeNodeConstructor(const MathExpression* expression);

bool TreeDestructor(MathTree* tree);

bool TreeNodeDestructor(MathNode* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

void TreeAddRightNode(MathNode* parent, MathNode* child);

void TreeAddLeftNode(MathNode* parent, MathNode* child);

void TreeMeasure(MathTree* tree, MathNode* node, size_t length);

MathNode* TreeCopyRecursive(MathNode* nodeSrc);

bool IsLeaf(MathNode* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

MathNode* TreeFindObject(MathNode* node, MathExpression object);

MathNode* TreeFindObjectStack(MathNode* node, MathExpression object, Stack* stk);

MathNode* GetNodeFromStack(Stack* stk, size_t index);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeFindVariables(MathTree* tree);

bool TreeFindVariable(MathTree* tree, char variable);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

double TreeCalculate(MathTree* tree);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#ifdef GRAPHVIZ

bool CreateTreeGraph(const char* fileName, MathTree* tree);

#else

#define CreateTreeGraph(fileName, tree) false;

#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\


#endif
