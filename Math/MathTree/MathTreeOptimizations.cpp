#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <Windows.h>
#include <cmath>
#include <string.h>


#include "MathTree.h"
#include "..\..\Logs\Logs.h"

#include "..\MathExpression\MathDSL.h"

#define CHANGE_NODE(child)                                      \
    {                                                           \
        MathNode* left = child->nodeLeft;                       \
        MathNode* right = child->nodeRight;                     \
        node->expression = child->expression;                   \
        node->childCount = child->childCount;                   \
        node->nodeLeft = left;                                  \
        node->nodeRight = right;                                \
        result = true;                                          \
    }

const size_t minTreeOptimizerSize = 30;
const size_t treeOptimizerScale = 2;

struct MathTreeOptimizer
{
    MathNode** leftTrees;
    size_t     leftIndex;
    size_t     leftSize;
    MathNode** rightTrees;
    size_t     rightIndex;
    size_t     rightSize;
};

static MathNode* FindEqualTrees(MathNode* node, MathNode* firstTree, MathOperator oper);

static bool OptimizerConstructor(MathTreeOptimizer* optimizer);

static void OptimizerDestructor(MathTreeOptimizer* optimizer);

static bool AddRightTreeToOptimizer(MathNode* node, MathTreeOptimizer* optimizer);

static bool AddLeftTreeToOptimizer(MathNode* node, MathTreeOptimizer* optimizer);


///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///              Оптимизация структуры дерева, сокращение лишних узлов
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\


static void OptimizeNodeToNumber(MathNode* node, double number)
{
    assert(node);

    if (LEFT)
        TreeNodeDestructor(LEFT);
    if (RIGHT)
        TreeNodeDestructor(RIGHT);

    LEFT = RIGHT = nullptr;
    node->expression.type = ME_NUMBER;
    node->expression.me_number = number;
}

/**
 * @brief      Оптимизирует дерево, выполняя все оптимизации
 *             (см в MathTree.h функции MathTreeOptimize<X>, где <X> - вид оптимизации)
 * @param tree Указатель на дерево
*/
void MathTreeOptimize(MathNode* problem)
{
    assert(problem);

    bool result = false;
    do
    { 
        result = false;

        while (MathTreeOptimizeCalculatedNodes(problem))
            result = true;

        while (OptimizeWorthlessNodes(problem))
            result = true;

        while (OptimizeWorthlessTrees(problem))
            result = true;
    }
    while (result);
}

/**
 * @brief      Оптимизирует узлы, которые можно вычислить. Например: (4+8)*9 -> 108; sin(Pi/6) -> 0.5;
 *             Тригонометрические функции вычисляются в радианах!
 * @param node Узел, который нужно оптимизировать.
 * @return     true, если была оптимизация. false, если дерево оптимизировать нельзя.
*/
bool MathTreeOptimizeCalculatedNodes(MathNode* node)
{
    assert(node);

    bool result = false;
    if (LEFT)
        result |= MathTreeOptimizeCalculatedNodes(LEFT);
    if (RIGHT)
        result |= MathTreeOptimizeCalculatedNodes(RIGHT);

    if (TYPE_EQUAL(node, ME_OPERATOR) || TYPE_EQUAL(node, ME_FUNCTION))
    {
        bool canCalculate = true;
        double calcRes = CalculateNode(node, nullptr, &canCalculate);

        if (!canCalculate)
            return result;
        
        OptimizeNodeToNumber(node, calcRes);
        return true;
    }
    return result;
}

/**
 * @brief      Оптимизирует узлы, которые можно сократить. Например: x*1 -> x; x^0 -> 1;
 *             Для корректной работы данной оптимизации первоначально выполнить 
 *             while (OptimizeCalculatedNodes(node) == true);
 * @param node Узел, который нужно оптимизировать.
 * @return     true, если была оптимизация. false, если дерево оптимизировать нельзя.
*/
bool OptimizeWorthlessNodes(MathNode* node)
{
    assert(node);

    bool result = false;
    if (TYPE_EQUAL(node, ME_OPERATOR))
    {
        switch (node->expression.me_operator)
        {
            case ME_ADDITION:
            {
                // Оптимизируем:
                // 0 + x = x
                // x + 0 = x
                if (TYPE_EQUAL(LEFT, ME_NUMBER) && GET_NUM(LEFT) == 0)
                {
                    TreeNodeDestructor(LEFT);
                    CHANGE_NODE(RIGHT);
                }
                else if (TYPE_EQUAL(RIGHT, ME_NUMBER) && GET_NUM(RIGHT) == 0)
                {
                    TreeNodeDestructor(RIGHT);
                    CHANGE_NODE(LEFT);
                }
                break;
            }
            case ME_SUBTRACTION:
            {
                // 0 - x = -x (не оптимизируем).
                // x - 0 = x (оптимизируем)
                if (TYPE_EQUAL(RIGHT, ME_NUMBER) && GET_NUM(RIGHT) == 0)
                {
                    TreeNodeDestructor(RIGHT);
                    CHANGE_NODE(LEFT);
                }
                break;
            }
            case ME_MULTIPLICATION:
            {
                // Оптимизируем:
                // 0 * x = 0
                // x * 0 = 0
                // 1 * x = x
                // x * 1 = x
                if (TYPE_EQUAL(LEFT, ME_NUMBER)  && GET_NUM(LEFT) == 0 || 
                    TYPE_EQUAL(RIGHT, ME_NUMBER) && GET_NUM(RIGHT) == 0)
                {
                    OptimizeNodeToNumber(node, 0);
                    result = true;
                }
                else if (TYPE_EQUAL(LEFT, ME_NUMBER) && GET_NUM(LEFT) == 1)
                {
                    TreeNodeDestructor(LEFT);
                    CHANGE_NODE(RIGHT);
                }
                else if (TYPE_EQUAL(RIGHT, ME_NUMBER) && GET_NUM(RIGHT) == 1)
                {
                    TreeNodeDestructor(RIGHT);
                    CHANGE_NODE(LEFT);
                }
                break;
            }
            case ME_DIVISION:
            {
                // Оптимизируем:
                // 0 / x = 0
                // x / 0 = ERROR
                // x / 1 = x
                if (TYPE_EQUAL(LEFT, ME_NUMBER) && GET_NUM(LEFT) == 0)
                {
                    OptimizeNodeToNumber(node, 0);
                    result = true;
                }
                else if (TYPE_EQUAL(RIGHT, ME_NUMBER) && GET_NUM(RIGHT) == 0)
                {
                    LogLine("Внимание! В выражении выполнено деление на 0.", LOG_ERROR, true);
                    return false;
                }
                else if (TYPE_EQUAL(RIGHT, ME_NUMBER) && GET_NUM(RIGHT) == 1)
                {
                    TreeNodeDestructor(RIGHT);
                    CHANGE_NODE(LEFT);
                }
                break;
            }
            case ME_POWER:
            {
                // Оптимизируем:
                // x^1 = x
                // x^0 = 1
                // 1^x = 1
                // 0^x = 0
                // 0^0 = ERROR
                if (TYPE_EQUAL(RIGHT, ME_NUMBER) && GET_NUM(RIGHT) == 1)
                {
                    TreeNodeDestructor(RIGHT);
                    CHANGE_NODE(LEFT);
                }
                else if (TYPE_EQUAL(LEFT, ME_NUMBER) && GET_NUM(LEFT) == 0 && TYPE_EQUAL(RIGHT, ME_NUMBER) && GET_NUM(RIGHT) == 0)
                {
                    LogLine("Внимание! Выражение 0^0 лишено смысла. Вычисление выражения будет прервано", LOG_ERROR, true);
                    return false;
                }
                else if (TYPE_EQUAL(RIGHT, ME_NUMBER) && GET_NUM(RIGHT) == 0 || 
                         TYPE_EQUAL(LEFT, ME_NUMBER)  && GET_NUM(LEFT) == 1)
                {
                    OptimizeNodeToNumber(node, 1);
                    result = true;
                }
                else if (TYPE_EQUAL(LEFT, ME_NUMBER) && GET_NUM(LEFT) == 0)
                {
                    OptimizeNodeToNumber(node, 0);
                    result = true;
                }
                break;
            }
        }
    }

    if (!result)
    {
        if (LEFT)
            result |= OptimizeWorthlessNodes(LEFT);
        if (RIGHT)
            result |= OptimizeWorthlessNodes(RIGHT);
    }
    return result;
}

/**
 * @brief      Оптимизирует поддеревья, которые можно сократить. Например (обозначим за A - поддерево): A-A -> 0; A/A -> 1;
 *             Для корректной работы данной оптимизации первоначально выполнить
 *             while (OptimizeCalculatedNodes(node) == true); и 
 *             while (OptimizeWorthlessNodes(node) == true);
 * @param node Узел, который нужно оптимизировать.
 * @return     true, если была оптимизация. false, если дерево оптимизировать нельзя.
*/
bool OptimizeWorthlessTrees(MathNode* node)
{
    assert(node);

    bool result = false;
    // Оптимизируем поддеревья
    // A-A = 0
    // A/A = 1
    // A*A = A^2
    // A+A = 2*A
    if (LEFT)
        result = OptimizeWorthlessTrees(LEFT);
    if (RIGHT)
        result = OptimizeWorthlessTrees(RIGHT);

    if (TYPE_EQUAL(node, ME_OPERATOR))
    {
        bool LeftEqualRight = CompareTrees(LEFT, RIGHT);
        if (LeftEqualRight)
        {
            switch (node->expression.me_operator)
            {
                case ME_ADDITION:
                    node->expression.me_operator = ME_MULTIPLICATION;
                    OptimizeNodeToNumber(LEFT, 2);
                    break;
                case ME_SUBTRACTION:
                    OptimizeNodeToNumber(node, 0);
                    break;
                case ME_MULTIPLICATION:
                    node->expression.me_operator = ME_POWER;
                    OptimizeNodeToNumber(RIGHT, 2);
                    break;
                case ME_DIVISION:
                    OptimizeNodeToNumber(node, 1);
                    break;
            }
            result = true;
        }
    }
    return result;
}

static MathNode* FindEqualTrees(MathNode* node, MathNode* firstTree, MathOperator oper)
{
    MathNode* secondTree = nullptr;

    switch (ME_OPERATOR)
    {
        case ME_ADDITION:
        case ME_MULTIPLICATION:
            if ((oper == ME_SUBTRACTION || oper == ME_DIVISION) && CompareTrees(node, firstTree))
                return node;
            if (LEFT && TYPE_EQUAL(LEFT, ME_OPERATOR))
                secondTree = FindEqualTrees(LEFT, firstTree, oper);
            if (!secondTree && RIGHT && TYPE_EQUAL(RIGHT, ME_OPERATOR))
                secondTree = FindEqualTrees(RIGHT, firstTree, oper);

            break;
        case ME_SUBTRACTION:

            break;
        case ME_DIVISION:

            break;
    }
}

static bool OptimizerConstructor(MathTreeOptimizer* optimizer)
{
    assert(optimizer);

    optimizer->leftIndex = optimizer->rightIndex = 0;
    optimizer->leftSize  = optimizer->rightSize  = minTreeOptimizerSize;

    optimizer->leftTrees  = (MathNode**)calloc(minTreeOptimizerSize, sizeof(MathNode*));
    optimizer->rightTrees = (MathNode**)calloc(minTreeOptimizerSize, sizeof(MathNode*));

    if (!optimizer->leftTrees || !optimizer->rightTrees)
    {
        LogLine("Ошибка выделения памяти", LOG_ERROR, true);
        free(optimizer->leftTrees);
        free(optimizer->rightTrees);
        return false;
    }

    return true;
}

#define ADD_NODE_TO_OPTIMIZER(side)                                                                     \
    {                                                                                                   \
        if (optimizer->side##Index >= optimizer->side##Size)                                            \
        {                                                                                               \
            optimizer->side##Size *= 2;                                                                 \
            MathNode** arr = (MathNode**)realloc(optimizer->side##Trees,                                \
                                                       (optimizer->side##Size) * sizeof(MathNode*));    \
            if (!arr)                                                                                   \
            {                                                                                           \
                LogLine("Ошибка выделения памяти", LOG_ERROR, true);                                    \
                OptimizerDestructor(optimizer);                                                         \
                return false;                                                                           \
            }                                                                                           \
            optimizer->side##Trees = arr;                                                               \
        }                                                                                               \
        optimizer->side##Trees[optimizer->side## Index++] = node;                                       \
        return true;                                                                                    \
    }

static bool AddLeftTreeToOptimizer(MathNode* node, MathTreeOptimizer* optimizer)
{
    assert(node);
    assert(optimizer);

    ADD_NODE_TO_OPTIMIZER(left);
}

static bool AddRightTreeToOptimizer(MathNode* node, MathTreeOptimizer* optimizer)
{
    assert(node);
    assert(optimizer);

    ADD_NODE_TO_OPTIMIZER(right);
}

static void OptimizerDestructor(MathTreeOptimizer* optimizer)
{
    assert(optimizer);

    free(optimizer->leftTrees);
    free(optimizer->rightTrees);
}


#undef CHANGE_NODE
#undef CLEAR_NODE