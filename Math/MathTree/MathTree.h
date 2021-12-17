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
    /// ���������� �������� � ���������
    size_t    childCount;
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

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

/**
 * @brief      ������������ ������, �������� ��� �����������
 *             (�� � MathTree.h ������� MathTreeOptimize<X>, ��� <X> - ��� �����������)
 * @param tree ��������� �� ������
*/
void MathTreeOptimize(MathTree* problem);

/**
 * @brief      ������������ ����, ������� ����� ���������. ��������: (4+8)*9 -> 108; sin(Pi/6) -> 0.5;
 *             ������������������ ������� ����������� � ��������!
 * @param node ����, ������� ����� ��������������.
 * @return     true, ���� ���� �����������. false, ���� ������ �������������� ������.
*/
bool MathTreeOptimizeCalculatedNodes(MathNode* node);

/**
 * @brief      ������������ ����, ������� ����� ���������. ��������: x*1 -> x; x^0 -> 1;
 *             ��� ���������� ������ ������ ����������� ������������� ��������� 
 *             while (OptimizeCalculatedNodes(node) == true);
 * @param node ����, ������� ����� ��������������.
 * @return     true, ���� ���� �����������. false, ���� ������ �������������� ������.
*/
bool OptimizeWorthlessNodes(MathNode* node);

/**
 * @brief      ������������ ����������, ������� ����� ���������. �������� (��������� �� A - ���������): A-A -> 0; A/A -> 1;
 *             ��� ���������� ������ ������ ����������� ������������� ���������
 *             while (OptimizeCalculatedNodes(node) == true); � 
 *             while (OptimizeWorthlessNodes(node) == true);
 * @param node ����, ������� ����� ��������������.
 * @return     true, ���� ���� �����������. false, ���� ������ �������������� ������.
*/
bool OptimizeWorthlessTrees(MathNode* node);

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
