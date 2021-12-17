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
 * @brief      Оптимизирует дерево, выполняя все оптимизации
 *             (см в MathTree.h функции MathTreeOptimize<X>, где <X> - вид оптимизации)
 * @param tree Указатель на дерево
*/
void MathTreeOptimize(MathTree* problem);

/**
 * @brief      Оптимизирует узлы, которые можно вычислить. Например: (4+8)*9 -> 108; sin(Pi/6) -> 0.5;
 *             Тригонометрические функции вычисляются в радианах!
 * @param node Узел, который нужно оптимизировать.
 * @return     true, если была оптимизация. false, если дерево оптимизировать нельзя.
*/
bool MathTreeOptimizeCalculatedNodes(MathNode* node);

/**
 * @brief      Оптимизирует узлы, которые можно сократить. Например: x*1 -> x; x^0 -> 1;
 *             Для корректной работы данной оптимизации первоначально выполнить 
 *             while (OptimizeCalculatedNodes(node) == true);
 * @param node Узел, который нужно оптимизировать.
 * @return     true, если была оптимизация. false, если дерево оптимизировать нельзя.
*/
bool OptimizeWorthlessNodes(MathNode* node);

/**
 * @brief      Оптимизирует поддеревья, которые можно сократить. Например (обозначим за A - поддерево): A-A -> 0; A/A -> 1;
 *             Для корректной работы данной оптимизации первоначально выполнить
 *             while (OptimizeCalculatedNodes(node) == true); и 
 *             while (OptimizeWorthlessNodes(node) == true);
 * @param node Узел, который нужно оптимизировать.
 * @return     true, если была оптимизация. false, если дерево оптимизировать нельзя.
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
