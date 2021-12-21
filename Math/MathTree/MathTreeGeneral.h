#ifndef MATH_TREE_GENERAL_H_
#define MATH_TREE_GENERAL_H_


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
    /// Количество потомков у поддерева
    size_t    childCount;
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


#endif