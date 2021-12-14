#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <Windows.h>
#include <cmath>


#include "MathTree.h"
#include "..\..\Logs\Logs.h"


#define NUM node->expression.me_number
#define LEFT node->nodeLeft
#define RIGHT node->nodeRight
#define CALC(node) CalculateNode(node, tree, canCalculate)


static bool TreeNodeFindVariables(MathNode* node, MathTree* tree);

static double CalculateNode(MathNode* node, MathTree* tree, bool* canCalculate);

static bool TreeAddVariable(MathTree* tree, char variable);

static void OptimizeNodeToNumber(MathNode* node, double number);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeConstructor(MathTree* tree)
{
    LogLine("Вызван TreeConstructor()", LOG_DEBUG);
    assert(tree);

    tree->root = nullptr;

    tree->varCapacity  = MATH_AVERAGE_VAR_COUNT;
    tree->variables = (char*)calloc(tree->varCapacity, sizeof(char));
    tree->values    = (double*)calloc(tree->varCapacity, sizeof(double));

    return true;
}

MathNode* TreeNodeConstructor(const MathExpression* expression)
{
    LogLine("Вызван TreeNodeConstructor()", LOG_DEBUG);
    //assert(value);

    MathNode* node = (MathNode*)calloc(1, sizeof(MathNode));

    if (!node)
    {
        LogLine("NodeConstructor: не хватает памяти для создания нового узла", LOG_ERROR, true);
        return nullptr;
    }

    LEFT = nullptr;
    RIGHT = nullptr;

    if (expression)
        memcpy(&node->expression, expression, sizeof(MathExpression));

    return node;
}

bool TreeDestructor(MathTree* tree) 
{
    LogLine("Вызван TreeDestructor()", LOG_DEBUG);
    // assert(tree);
    
    if (tree)
    {
        TreeNodeDestructor(tree->root);

        free(tree->values);
        free(tree->variables);
    }

    return true;
}

bool TreeNodeDestructor(MathNode* node)
{
    LogLine("Вызван TreeNodeDestructor()", LOG_DEBUG);
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

void TreeAddLeftNode(MathNode* parent, MathNode* child)
{
    LogLine("Вызван TreeAddLeftNode()", LOG_DEBUG);
    assert(parent);
    assert(child);

    parent->nodeLeft = child;
    child->parent = parent;
}

void TreeAddRightNode(MathNode* parent, MathNode* child)
{
    LogLine("Вызван TreeAddRightNode()", LOG_DEBUG);
    assert(parent);
    assert(child);

    parent->nodeRight = child;
    child->parent = parent;
}

void TreeMeasure(MathTree* tree, MathNode* node, size_t length)
{
    LogLine("Вызван TreeMeasure()", LOG_DEBUG);
    //assert(tree);
    //assert(node);

    if (!node || !tree)
    {
        tree->treeLength = 0;
        return;
    }

    length++;

    if (LEFT)
        TreeMeasure(tree, LEFT, length);

    if (RIGHT)
        TreeMeasure(tree, RIGHT, length);

    if (tree->treeLength < length)
        tree->treeLength = length;
}

#define CHECK_NODE(node)                                        \
    if (!(node))                                                \
    {                                                           \
        TreeNodeDestructor(node);                               \
        return nullptr;                                         \
    }

/**
 * @brief               Рекурсивно копирует дерево.
 * @param nodeSrc       Исходное дерево.
 * @param nodeParentDst Указатель, куда надо скопировать дерево.
 * @return              false в случае ошибки, true в случае успеха.
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

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

MathNode* TreeFindObject(MathNode* node, MathExpression object)
{
    LogLine("Вызван TreeFindObject()", LOG_DEBUG);
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
    LogLine("Вызван TreeFindObjectStack()", LOG_DEBUG);
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
    LogLine("Вызван GetNodeFromStack()", LOG_DEBUG);
    assert(stk);

    MathNode** nodeptr = (MathNode**)StackGetElemAt(stk, index);

    if (nodeptr)
        return *nodeptr;

    return nullptr;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeFindVariables(MathTree* tree)
{
    return TreeNodeFindVariables(tree->root, tree);
}

static bool TreeNodeFindVariables(MathNode* node, MathTree* tree)
{
    if (node->expression.type == ME_VARIABLE)
    {
        if (!TreeFindVariable(tree, node->expression.me_variable) &&
            !TreeAddVariable(tree, node->expression.me_variable))
                return false;
    }

    bool res = true;

    if (LEFT)
        res = TreeNodeFindVariables(LEFT, tree);
    if (RIGHT)
        res = TreeNodeFindVariables(RIGHT, tree);

    return res;
}

bool TreeFindVariable(MathTree* tree, char variable)
{
    size_t capacity = tree->varCapacity;
    for (size_t index = 0; index < capacity; index++)
    {
        if (tree->variables[index] == variable)
            return true;
    }
    return false;
}

static bool TreeAddVariable(MathTree* tree, char variable)
{
    size_t index = 0;
    for (; index < tree->varCapacity; index++)
    {
        if (tree->variables[index] == '\0')
            break;
    }

    if (index == tree->varCapacity)
    {
        char* newVar = (char*)realloc(tree->variables, tree->varCapacity * MATH_VAR_COUNT_SCALE);
        char* newVal = (char*)realloc(tree->values, tree->varCapacity *MATH_VAR_COUNT_SCALE);

        if (!newVar || !newVal)
        {
            free(newVar);
            free(newVal);
            return false;
        }
    }

    tree->variables[index] = variable;
    return true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

double TreeCalculate(MathTree* tree)
{
    return CalculateNode(tree->root, tree, nullptr);
}

static double CalculateNode(MathNode* node, MathTree* tree, bool* canCalculate)
{
    assert(node);
    //assert(tree);
    //assert(canCalculate);

    switch (node->expression.type)
    {
        case ME_NUMBER:
            return NUM;
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
            if (tree)
            {
                for (int st = 0; st < tree->varCapacity; st++)
                {
                    if (tree->variables[st] == node->expression.me_variable)
                        return tree->values[st];
                }
            }
            break;
        case ME_FUNCTION:
            switch (node->expression.me_function)
            {
                case ME_SIN:
                    return sin(NUM);
                case ME_COS:
                    return cos(NUM);
                case ME_TG:
                    return tan(NUM);
                case ME_CTG:
                    return 1.0 / tan(NUM);
                case ME_SH:
                    return sinh(NUM);
                case ME_CH:
                    return cosh(NUM);
                case ME_LN:
                    return log(NUM);
                case ME_SQRT:
                    return sqrt(NUM);
                case ME_CBRT:
                    return cbrt(NUM);
                case ME_ARCSIN:
                    return asin(NUM);
                case ME_ARCCOS:
                    return acos(NUM);
                case ME_ARCTG:
                    return atan(NUM);
                case ME_ARCCTG:
                    return Pi/2 - atan(NUM);
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

bool OptimizeCalculatedNodes(MathNode* node)
{
    bool result = false;
    if (LEFT)
        result |= OptimizeCalculatedNodes(LEFT);
    if (RIGHT)
        result |= OptimizeCalculatedNodes(RIGHT);

    if (!result)
        return false;

    if (node->expression.type == ME_OPERATOR || node->expression.type == ME_FUNCTION)
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

bool OptimizeWorthlessNodes(MathNode* node)
{

    return false;
}

bool OptimizeWorthlessTrees(MathNode* node)
{

    return false;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#ifdef GRAPHVIZ

static void CreateNodeGraph(FILE* file, MathNode* node, size_t parentId, bool IsRight);

bool CreateTreeGraph(const char* outImagefileName, MathTree* tree)
{
    LogLine("Вызван CreateTreeGraph()", LOG_DEBUG);
    assert(outImagefileName);
    assert(tree);

    FILE* file = fopen("treeGraph.gv", "w");

    if (!file)
    {
        LogLine("CreateTreeGraph: ошибка открытия файла.", LOG_ERROR, true);
        return false;
    }

    fseek(file, 0, SEEK_SET);
    fputs("digraph G\n"
          "{\n"
          "    bgcolor=\"lightblue\"\n"
          "    node [fontcolor=gray11, style=filled, fontsize = 18];\n", file);

    CreateNodeGraph(file, tree->root, 0, true);

    fputs("}", file);

    fclose(file);

    const size_t cmdDelta = 35;
    size_t cmdSize = strlen(outImagefileName);
    char* cmd = (char*)calloc(cmdSize + cmdDelta, sizeof(char));

    if (!cmd)
    {
        LogLine("CreateNodeGraph: не хватает памяти для генерации команды.", LOG_ERROR, true);
        return false;
    }

    sprintf(cmd, "dot \"treeGraph.gv\" -Tpng > \"%s\"", outImagefileName);
    system(cmd);

    free(cmd);

    //remove("treeGraph.gv");

    system(outImagefileName);

    return true;
}

static void CreateNodeGraph(FILE* file, MathNode* node, size_t parentId, bool IsRight)
{
    LogLine("Вызван CreateNodeGraph()", LOG_DEBUG);
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

#undef NUM
#undef LEFT
#undef RIGHT
#undef CALC