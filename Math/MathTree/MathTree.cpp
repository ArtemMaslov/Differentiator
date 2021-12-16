#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <Windows.h>
#include <cmath>


#include "MathTree.h"
#include "..\..\Logs\Logs.h"

#include "..\MathExpression\MathDSL.h"

#define CALC(node) CalculateNode(node, problem, canCalculate)


static bool TreeNodeFindVariables(MathNode* node, MathTree* problem);

static double CalculateNode(MathNode* node, MathTree* problem, bool* canCalculate);

static bool TreeAddVariable(MathTree* problem, char variable);

static void OptimizeNodeToNumber(MathNode* node, double number);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///                           ������������ � �����������
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeConstructor(MathTree* problem)
{
    LogLine("������ TreeConstructor()", LOG_DEBUG);
    assert(problem);

    problem->root = nullptr;

    problem->varCapacity  = MATH_AVERAGE_VAR_COUNT;
    problem->variables = (char*)calloc(problem->varCapacity, sizeof(char));
    problem->values    = (double*)calloc(problem->varCapacity, sizeof(double));

    return true;
}

MathNode* TreeNodeConstructor(const MathExpression* expression)
{
    LogLine("������ TreeNodeConstructor()", LOG_DEBUG);
    //assert(value);

    MathNode* node = (MathNode*)calloc(1, sizeof(MathNode));

    if (!node)
    {
        LogLine("NodeConstructor: �� ������� ������ ��� �������� ������ ����", LOG_ERROR, true);
        return nullptr;
    }

    LEFT = nullptr;
    RIGHT = nullptr;

    if (expression)
        memcpy(&node->expression, expression, sizeof(MathExpression));

    return node;
}

bool TreeDestructor(MathTree* problem) 
{
    LogLine("������ TreeDestructor()", LOG_DEBUG);
    // assert(tree);
    
    if (problem)
    {
        TreeNodeDestructor(problem->root);

        free(problem->values);
        free(problem->variables);
    }

    return true;
}

bool TreeNodeDestructor(MathNode* node)
{
    LogLine("������ TreeNodeDestructor()", LOG_DEBUG);
    // assert(node);

    if (node)
    {
        if (LEFT)
            TreeNodeDestructor(LEFT);
        if (RIGHT)
            TreeNodeDestructor(RIGHT);
        free(node);
    }

    return true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///                     ������ � ������� (����������� �������)
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

void TreeAddLeftNode(MathNode* parent, MathNode* child)
{
    LogLine("������ TreeAddLeftNode()", LOG_DEBUG);
    assert(parent);
    assert(child);

    parent->nodeLeft = child;
    child->parent = parent;
}

void TreeAddRightNode(MathNode* parent, MathNode* child)
{
    LogLine("������ TreeAddRightNode()", LOG_DEBUG);
    assert(parent);
    assert(child);

    parent->nodeRight = child;
    child->parent = parent;
}

size_t TreeMeasure(MathNode* node)
{
    size_t result = 1;
    if (node->nodeLeft)
        result += TreeMeasure(node->nodeLeft);
    if (node->nodeRight)
        result += TreeMeasure(node->nodeRight);

    return result;
}

#define CHECK_NODE(node)                                        \
    if (!(node))                                                \
    {                                                           \
        TreeNodeDestructor(node);                               \
        return nullptr;                                         \
    }

/**
 * @brief               ���������� �������� ������.
 * @param nodeSrc       �������� ������.
 * @param nodeParentDst ���������, ���� ���� ����������� ������.
 * @return              false � ������ ������, true � ������ ������.
*/
MathNode* TreeCopyRecursive(MathNode* nodeSrc)
{
    MathNode* node = TreeNodeConstructor(&nodeSrc->expression);

    CHECK_NODE(node)
    
    if (nodeSrc->nodeLeft)
    {
        LEFT = TreeCopyRecursive(nodeSrc->nodeLeft);
        
        CHECK_NODE(LEFT)
            
        TreeAddLeftNode(node, LEFT);
    }
    
    if (nodeSrc->nodeRight)
    {
        RIGHT = TreeCopyRecursive(nodeSrc->nodeRight);
        
        CHECK_NODE(RIGHT)
            
        TreeAddRightNode(node, RIGHT);
    }
    
    return node;
}

#undef CHECK_NODE

bool IsLeaf(MathNode* node)
{
    if (node && (LEFT == nullptr || RIGHT == nullptr))
        return true;
    else
        return false;
}

/**
 * @brief       ���������� ��� ���������.
 * @param node1 ������ ���������.
 * @param node2 ������ ���������.
 * @return      true, ���� ���������� �����.
*/
bool CompareTrees(MathNode* node1, MathNode* node2)
{
    if (MathExpressionEqual(&node1->expression, &node2->expression))
    {
        // ���� ��� ����� ������ ��������, ���� ��� �� ����� ������ ��������
        if ((node1->nodeLeft == nullptr) == (node2->nodeLeft == nullptr) && 
            !CompareTrees(node1->nodeLeft, node2->nodeLeft))
            return false;
        
        if ((node1->nodeLeft == nullptr) == (node2->nodeLeft == nullptr) && 
            !CompareTrees(node1->nodeRight, node2->nodeRight))
            return false;

        return true;
    }
    return false;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///                              ����� ��������� ������
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

MathNode* TreeFindObject(MathNode* node, MathExpression object)
{
    LogLine("������ TreeFindObject()", LOG_DEBUG);
    assert(node);
    
    if (MathExpressionEqual(&object, &node->expression) == 0)
        return node;
    
    MathNode* result = nullptr;

    if (LEFT)
        result = TreeFindObject(LEFT, object);
    
    if (RIGHT && !result)
        result = TreeFindObject(RIGHT, object);
    
    return result;
}

MathNode* TreeFindObjectStack(MathNode* node, MathExpression object, Stack* stk)
{
    LogLine("������ TreeFindObjectStack()", LOG_DEBUG);
    assert(stk);
    assert(node);

    StackPush(stk, &node);

    if (MathExpressionEqual(&object, &node->expression) == 0)
        return node;

    MathNode* result = nullptr;

    if (LEFT)
        result = TreeFindObjectStack(LEFT, object, stk);
        
    if (RIGHT && !result)
        result = TreeFindObjectStack(RIGHT, object, stk);

    if (!result)
        StackPop(stk, nullptr);

    return result;
}

MathNode* GetNodeFromStack(Stack* stk, size_t index)
{
    LogLine("������ GetNodeFromStack()", LOG_DEBUG);
    assert(stk);

    MathNode** nodeptr = (MathNode**)StackGetElemAt(stk, index);

    if (nodeptr)
        return *nodeptr;

    return nullptr;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///                      ������ � ��������������� �����������
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeFindVariables(MathTree* problem)
{
    return TreeNodeFindVariables(problem->root, problem);
}

static bool TreeNodeFindVariables(MathNode* node, MathTree* problem)
{
    if (node->expression.type == ME_VARIABLE)
    {
        if (!TreeFindVariable(problem, node->expression.me_variable) &&
            !TreeAddVariable(problem, node->expression.me_variable))
                return false;
    }

    bool res = true;

    if (LEFT)
        res = TreeNodeFindVariables(LEFT, problem);
    if (RIGHT)
        res = TreeNodeFindVariables(RIGHT, problem);

    return res;
}

bool TreeFindVariable(MathTree* problem, char variable)
{
    size_t capacity = problem->varCapacity;
    for (size_t index = 0; index < capacity; index++)
    {
        if (problem->variables[index] == variable)
            return true;
    }
    return false;
}

static bool TreeAddVariable(MathTree* problem, char variable)
{
    size_t index = 0;
    for (; index < problem->varCapacity; index++)
    {
        if (problem->variables[index] == '\0')
            break;
    }

    if (index == problem->varCapacity)
    {
        char* newVar = (char*)realloc(problem->variables, problem->varCapacity * MATH_VAR_COUNT_SCALE);
        char* newVal = (char*)realloc(problem->values, problem->varCapacity *MATH_VAR_COUNT_SCALE);

        if (!newVar || !newVal)
        {
            free(newVar);
            free(newVal);
            return false;
        }
    }

    problem->variables[index] = variable;
    return true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///            ���������� ����������, (����������� �������� ����������)
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

double TreeCalculate(MathTree* problem)
{
    return CalculateNode(problem->root, problem, nullptr);
}

static double CalculateNode(MathNode* node, MathTree* problem, bool* canCalculate)
{
    assert(node);
    //assert(tree);
    //assert(canCalculate);

    switch (node->expression.type)
    {
        case ME_NUMBER:
            return GET_NUM(node);
        case ME_CONSTANT:
            switch (node->expression.me_constant)
            {
                case ME_PI:
                    return Pi;
                case ME_EXP:
                    return Exp;
            }
            break;
        case ME_VARIABLE:
            if (problem)
            {
                for (int st = 0; st < problem->varCapacity; st++)
                {
                    if (problem->variables[st] == node->expression.me_variable)
                        return problem->values[st];
                }
            }
            break;
        case ME_FUNCTION:
            switch (node->expression.me_function)
            {
                case ME_SIN:
                    return sin(GET_NUM(node));
                case ME_COS:
                    return cos(GET_NUM(node));
                case ME_TG:
                    return tan(GET_NUM(node));
                case ME_CTG:
                    return 1.0 / tan(GET_NUM(node));
                case ME_SH:
                    return sinh(GET_NUM(node));
                case ME_CH:
                    return cosh(GET_NUM(node));
                case ME_LN:
                    return log(GET_NUM(node));
                case ME_SQRT:
                    return sqrt(GET_NUM(node));
                case ME_CBRT:
                    return cbrt(GET_NUM(node));
                case ME_ARCSIN:
                    return asin(GET_NUM(node));
                case ME_ARCCOS:
                    return acos(GET_NUM(node));
                case ME_ARCTG:
                    return atan(GET_NUM(node));
                case ME_ARCCTG:
                    return Pi/2 - atan(GET_NUM(node));
            }
            break;
        case ME_OPERATOR:
            double leftVal = CALC(LEFT);
            if (canCalculate && !(*canCalculate))
                return 0;

            double rightVal = CALC(RIGHT);
            if (canCalculate && !(*canCalculate))
                return 0;

            switch (node->expression.me_operator)
            {
                case ME_ADDITION:
                    return leftVal + rightVal;
                case ME_SUBTRACTION:
                    return leftVal - rightVal;
                case ME_MULTIPLICATION:
                    return leftVal * rightVal;
                case ME_DIVISION:
                    return leftVal / rightVal;
                case ME_POWER:
                    return pow(leftVal, rightVal);
            }
            break;
    }
    
    if (canCalculate)
        *canCalculate = false;
    return 0;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///              ����������� ��������� ������, ���������� ������ �����
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#define CHANGE_NODE(child)                                      \
    {                                                           \
        if (node->parent->nodeLeft == node)                     \
            TreeAddLeftNode(node->parent, child);               \
        else                                                    \
            TreeAddRightNode(node->parent, child);              \
    }

#define CLEAR_NODE                                              \
    {                                                           \
        TreeNodeDestructor(node);                               \
        result = true;                                          \
    }

static void OptimizeNodeToNumber(MathNode* node, double number)
{
    if (LEFT)
        TreeNodeDestructor(LEFT);
    if (RIGHT)
        TreeNodeDestructor(RIGHT);

    LEFT = RIGHT = nullptr;
    node->expression.type = ME_NUMBER;
    node->expression.me_number = number;
}

/**
 * @brief      ������������ ������, �������� ��� �����������
 *             (�� � MathTree.h ������� MathTreeOptimize<X>, ��� <X> - ��� �����������)
 * @param tree ��������� �� ������
*/
void MathTreeOptimize(MathTree* problem)
{
    bool result = false;
    do
    { 
        result = false;
        while (MathTreeOptimizeCalculatedNodes(problem->root))
            result = true;
        while (OptimizeWorthlessNodes(problem->root))
            result = true;
        while (OptimizeWorthlessTrees(problem->root))
            result = true;
    }
    while (result);
}

/**
 * @brief      ������������ ����, ������� ����� ���������. ��������: (4+8)*9 -> 108; sin(Pi/6) -> 0.5;
 *             ������������������ ������� ����������� � ��������!
 * @param node ����, ������� ����� ��������������.
 * @return     true, ���� ���� �����������. false, ���� ������ �������������� ������.
*/
bool MathTreeOptimizeCalculatedNodes(MathNode* node)
{
    bool result = false;
    if (LEFT)
        result |= MathTreeOptimizeCalculatedNodes(LEFT);
    if (RIGHT)
        result |= MathTreeOptimizeCalculatedNodes(RIGHT);

    if (TYPE_EQUAL(node, ME_OPERATOR) || TYPE_EQUAL(node, ME_FUNCTION))
    {
        bool canCalculate = true;
        double result = CalculateNode(node, nullptr, &canCalculate);

        if (!canCalculate)
            return false;
        
        OptimizeNodeToNumber(node, result);
        return true;
    }
    return false;
}

/**
 * @brief      ������������ ����, ������� ����� ���������. ��������: x*1 -> x; x^0 -> 1;
 *             ��� ���������� ������ ������ ����������� ������������� ��������� 
 *             while (OptimizeCalculatedNodes(node) == true);
 * @param node ����, ������� ����� ��������������.
 * @return     true, ���� ���� �����������. false, ���� ������ �������������� ������.
*/
bool OptimizeWorthlessNodes(MathNode* node)
{
    bool result = false;
    if (TYPE_EQUAL(node, ME_OPERATOR))
    {
        switch (node->expression.me_operator)
        {
            case ME_ADDITION:
            {
                // ������������:
                // 0 + x = x
                // x + 0 = x
                if (GET_NUM(LEFT) == 0)
                {
                    CHANGE_NODE(RIGHT);

                    RIGHT = nullptr;
                    CLEAR_NODE;
                }
                else if (GET_NUM(RIGHT) == 0)
                {
                    CHANGE_NODE(LEFT);

                    LEFT = nullptr;
                    CLEAR_NODE;
                }
                break;
            }
            case ME_SUBTRACTION:
            {
                // 0 - x = -x (�� ������������).
                // x - 0 = x (������������)
                if (GET_NUM(RIGHT) == 0)
                {
                    CHANGE_NODE(LEFT);

                    LEFT = nullptr;
                    CLEAR_NODE;
                }
                break;
            }
            case ME_MULTIPLICATION:
            {
                // ������������:
                // 0 * x = 0
                // x * 0 = 0
                // 1 * x = x
                // x * 1 = x
                if (GET_NUM(LEFT) == 0 || GET_NUM(RIGHT) == 0)
                {
                    MathNode* newNode = NUM(0);
                    CHANGE_NODE(newNode);

                    CLEAR_NODE;
                }
                else if (GET_NUM(LEFT) == 1)
                {
                    CHANGE_NODE(RIGHT);

                    RIGHT = nullptr;
                    CLEAR_NODE;
                }
                else if (GET_NUM(RIGHT) == 1)
                {
                    CHANGE_NODE(LEFT);

                    LEFT = nullptr;
                    CLEAR_NODE;
                }
                break;
            }
            case ME_DIVISION:
            {
                // ������������:
                // 0 / x = 0
                // x / 0 = ERROR
                // x / 1 = x
                if (GET_NUM(LEFT) == 0)
                {
                    MathNode* newNode = NUM(0);
                    CHANGE_NODE(newNode);

                    CLEAR_NODE;
                }
                else if (GET_NUM(RIGHT) == 0)
                {
                    // ERROR;
                    return false;
                }
                else if (GET_NUM(RIGHT) == 1)
                {
                    CHANGE_NODE(LEFT);

                    LEFT = nullptr;
                    CLEAR_NODE;
                }
                break;
            }
            case ME_POWER:
            {
                // ������������:
                // x^1 = x
                // x^0 = 1
                // 1^x = 1
                // 0^x = 0
                // 0^0 = ERROR
                if (GET_NUM(RIGHT) == 1)
                {
                    CHANGE_NODE(LEFT);

                    LEFT = nullptr;
                    CLEAR_NODE;
                }
                else if (GET_NUM(LEFT) == 0 && GET_NUM(RIGHT) == 0)
                {
                    // ERROR;
                    return false;
                }
                else if (GET_NUM(RIGHT) == 0 || GET_NUM(LEFT) == 1)
                {
                    MathNode* newNode = NUM(1);
                    CHANGE_NODE(newNode);

                    CLEAR_NODE;
                }
                else if (GET_NUM(LEFT) == 0)
                {
                    MathNode* newNode = NUM(0);
                    CHANGE_NODE(newNode);

                    CLEAR_NODE;
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
 * @brief      ������������ ����������, ������� ����� ���������. �������� (��������� �� A - ���������): A-A -> 0; A/A -> 1;
 *             ��� ���������� ������ ������ ����������� ������������� ���������
 *             while (OptimizeCalculatedNodes(node) == true); � 
 *             while (OptimizeWorthlessNodes(node) == true);
 * @param node ����, ������� ����� ��������������.
 * @return     true, ���� ���� �����������. false, ���� ������ �������������� ������.
*/
bool OptimizeWorthlessTrees(MathNode* node)
{
    bool result = false;
    // ������������ ����������
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

#undef CHANGE_NODE
#undef CLEAR_NODE

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///                   ���������� ����� ������ � ������� GraphViz
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#ifdef GRAPHVIZ

static void CreateNodeGraph(FILE* file, MathNode* node, size_t parentId, bool IsRight);

bool CreateTreeGraph(const char* outImagefileName, MathNode* node, bool openFile)
{
    LogLine("������ CreateTreeGraph()", LOG_DEBUG);
    assert(outImagefileName);
    assert(node);

    FILE* file = fopen("treeGraph.gv", "w");

    if (!file)
    {
        LogLine("CreateTreeGraph: ������ �������� �����.", LOG_ERROR, true);
        return false;
    }
    CreateGraphCallCount++;

    fputs("digraph G\n"
          "{\n"
          "    bgcolor=\"lightblue\"\n"
          "    node [fontcolor=gray11, style=filled, fontsize = 18];\n", file);

    CreateNodeGraph(file, node, 0, true);

    fputs("}", file);

    fclose(file);

    char cmd[500];

    sprintf(cmd, "dot \"treeGraph.gv\" -Tpng > \"%s%d.png\"", outImagefileName, CreateGraphCallCount);
    system(cmd);

    remove("treeGraph.gv");

    if (openFile)
        system(outImagefileName);

    return true;
}

static void CreateNodeGraph(FILE* file, MathNode* node, size_t parentId, bool IsRight)
{
    LogLine("������ CreateNodeGraph()", LOG_DEBUG);
    assert(file);
    assert(node);

    static size_t nodeId = 1;
    size_t _nodeId = nodeId;
                  
    switch (node->expression.type)
    {
        case ME_NUMBER:
            fprintf(file, "    node%zd[shape = pentagon,   fillcolor=chartreuse4, label = <", _nodeId);
            break;
        case ME_VARIABLE:
            fprintf(file, "    node%zd[shape = trapezium,  fillcolor=azure4,      label = <", _nodeId);
            break;
        case ME_CONSTANT:
            fprintf(file, "    node%zd[shape = house, fillcolor=darkslateblue, label = <", _nodeId);
            break;
        case ME_OPERATOR:
            fprintf(file, "    node%zd[shape = oval,       fillcolor=cadetblue1,  fontsize = 28, label = <", _nodeId);
            break;
        case ME_FUNCTION:
            fprintf(file, "    node%zd[shape = hexagon,    fillcolor=cadetblue3,  label = <", _nodeId);
            break;
        default:
            fprintf(file, "    node%zd[shape = rect,       fillcolor=darkred,     label = <", _nodeId);
            break;
    }
    
    PrintMathExpression(&node->expression, file);
    fputs(">];\n", file);

    nodeId++;

    if (parentId != 0)
        fprintf(file, "        node%zd -> node%zd [color=%s];\n", 
                parentId, _nodeId, IsRight?"darkorchid3":"coral1");

    if (LEFT)
        CreateNodeGraph(file, LEFT, _nodeId, false);
    
    if (RIGHT)
        CreateNodeGraph(file, RIGHT, _nodeId, true);
}

#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#undef CALC
#include "..\MathExpression\UndefMathDSL.h"