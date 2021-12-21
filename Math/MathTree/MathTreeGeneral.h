#ifndef MATH_TREE_GENERAL_H_
#define MATH_TREE_GENERAL_H_


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


#endif