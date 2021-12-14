#ifndef MY_TREE_H
#define MY_TREE_H


#include "..\..\StackLibrary\Stack.h"
#include "..\MathExpression\MathExpression.h"


/// �������� ���������� ����� �� ���� ������.
#define GRAPHVIZ

/// ������ ������������ �� ����� ��� ���������� (x, y, z).
const size_t MATH_AVERAGE_VAR_COUNT = 3;

/// ���� � ������ ����� ���������� ���������� ������, ��� MATH_AVERAGE_VAR_COUNT,
/// �� ������ �������� ����� �������� � MATH_VAR_COUNT_SCALE ���
const size_t MATH_VAR_COUNT_SCALE   = 2;

struct MathNode
{
    /// �������� ����.
    MathExpression expression;
    /// �������� ����.
    MathNode* parent;
    /// ����� �������.
    MathNode* nodeLeft;
    /// ������ �������.
    MathNode* nodeRight;
};

struct MathTree
{
    /// ������ ������.
    MathNode* root;
    /// ���������� ���������� ����� ������ � �������, ����� �������� ����� ������� TreeMeasure().
    size_t treeLength;

    /// ����������, ������������ � ������
    char*   variables;
    /// �������� ����������, ������������ � ������
    double* values;
    /// ���������� ����������, ������������ � ������
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
