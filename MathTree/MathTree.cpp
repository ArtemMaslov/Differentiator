#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <Windows.h>


#include "MathTree.h"
#include "..\Differentiator\Config.h"


#include "..\Logs\Logs.h"


static size_t SizeofTreeType(const char* expression);

bool PrintNodeToFile(MathNode* node);

static void CreateNodeGraph(FILE* file, MathNode* node, size_t parentId, bool IsRight);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeConstructor(MathTree* tree)
{
    LogLine("Вызван TreeConstructor()", LOG_DEBUG);
    assert(tree);

    tree->root = nullptr;

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

    node->nodeLeft = nullptr;
    node->nodeRight = nullptr;

    if (expression)
        memcpy(&node->expression, expression, sizeof(MathExpression));

    return node;
}

bool TreeDestructor(MathTree* tree) 
{
    LogLine("Вызван TreeDestructor()", LOG_DEBUG);
    // assert(tree);
    
    if (tree)
        TreeNodeDestructor(tree->root);

    return true;
}

bool TreeNodeDestructor(MathNode* node)
{
    LogLine("Вызван TreeNodeDestructor()", LOG_DEBUG);
    // assert(node);

    if (node)
    {
        if (node->nodeLeft)
            TreeNodeDestructor(node->nodeLeft);
        if (node->nodeRight)
            TreeNodeDestructor(node->nodeRight);
        free(node);
    }

    return true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeAddLeftNode(MathNode* parent, MathNode* child)
{
    LogLine("Вызван TreeAddLeftNode()", LOG_DEBUG);
    assert(parent);
    assert(child);

    parent->nodeLeft = child;
    child->parent = parent;

    return true;
}

bool TreeAddRightNode(MathNode* parent, MathNode* child)
{
    LogLine("Вызван TreeAddRightNode()", LOG_DEBUG);
    assert(parent);
    assert(child);

    parent->nodeRight = child;
    child->parent = parent;

    return true;
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

    if (node->nodeLeft)
        TreeMeasure(tree, node->nodeLeft, length);

    if (node->nodeRight)
        TreeMeasure(tree, node->nodeRight, length);

    if (tree->treeLength < length)
        tree->treeLength = length;
}

bool IsLeaf(MathNode* node)
{
    if (node && (node->nodeLeft == nullptr || node->nodeRight == nullptr))
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
    
    if (MathExpressionsEqual(&object, &node->expression) == 0)
        return node;
    
    MathNode* result = nullptr;

    if (node->nodeLeft)
        result = TreeFindObject(node->nodeLeft, object);
    
    if (node->nodeRight && !result)
        result = TreeFindObject(node->nodeRight, object);
    
    return result;
}

MathNode* TreeFindObjectStack(MathNode* node, MathExpression object, Stack* stk)
{
    LogLine("Вызван TreeFindObjectStack()", LOG_DEBUG);
    assert(stk);
    assert(node);

    StackPush(stk, &node);

    if (MathExpressionsEqual(&object, &node->expression) == 0)
        return node;

    MathNode* result = nullptr;

    if (node->nodeLeft)
        result = TreeFindObjectStack(node->nodeLeft, object, stk);
        
    if (node->nodeRight && !result)
        result = TreeFindObjectStack(node->nodeRight, object, stk);

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

bool MathExpressionsEqual(MathExpression* expr1, MathExpression* expr2)
{
    if (expr1->type != expr2->type)
    {
        return false;
    }

    switch (expr1->type)
    {
        case ME_NUMBER:
            if (expr1->me_number == expr2->me_number)
                return true;
            break;
        case ME_CONST:
            if (expr1->me_constant == expr2->me_constant)
                return true;
            break;
        case ME_OPERATOR:
            if (expr1->me_operator == expr2->me_operator)
                return true;
            break;
        case ME_VARIABLE:
            if (expr1->me_variable == expr2->me_variable)
                return true;
            break;
        default:
            break;
    }
    return false;
}

bool MathExpressionParseString(MathExpression* expr, const char* ptr)
{
    assert(expr);

    if (sscanf(ptr, "%lf", &expr->me_number))
    {
        expr->type = ME_NUMBER;
        return true;
    }


    
    return false;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#ifdef GRAPHVIZ

bool CreateTreeGraph(const char* outFileName, Tree* tree)
{
    LogLine("Вызван CreateTreeGraph()", LOG_DEBUG);
    assert(outFileName);
    assert(tree);

    FILE* file = fopen("treeGraph.gv", "w+,ccs=UTF-8");

    if (!file)
    {
        LogLine("CreateTreeGraph: ошибка открытия файла.", LOG_ERROR, true);
        return false;
    }

    fseek(file, 0, SEEK_SET);
    fputws(L"digraph G\n{\n    node [shape = plaintext, fontcolor=\"#292C37\"];\n", file);
    CreateNodeGraph(file, tree->root, 0, true);
    fputws(L"}", file);

    fclose(file);

    size_t cmdSize = strlen(outFileName) + 1 + 4;
    char* cmd = (char*)calloc(cmdSize + 30, sizeof(char));

    if (!cmd)
    {
        LogLine("CreateNodeGraph: не хватает памяти для генерации команды.", LOG_ERROR, true);
        return false;
    }

    sprintf(cmd, "dot \"treeGraph.gv\" -Tpng > \"%s\"", outFileName);
    system(cmd);

    free(cmd);

    remove("treeGraph.gv");

    system(outFileName);

    return true;
}

static void CreateNodeGraph(FILE* file, Node* node, size_t parentId, bool IsRight)
{
    LogLine("Вызван CreateNodeGraph()", LOG_DEBUG);
    assert(file);
    assert(node);

    static size_t nodeId = 1;
    size_t _nodeId = nodeId;

    if (!(node->value.ptr))
        return;
    
    wchar_t* wcharStr = (wchar_t*)calloc(node->value.length + 1, sizeof(wchar_t));

    if (!wcharStr)
    {
        LogLine("CreateNodeGraph: не хватает памяти для генерации строки.", LOG_ERROR, true);
        return;
    }

    MultiByteToWideChar(CP_ACP, 0, (char*)node->value.ptr, node->value.length + 1, wcharStr, node->value.length + 1);
    wcharStr[node->value.length] = '\0';
    
    fwprintf(file, L"    node%zd[label = <<TABLE BORDER=\"0\" CELLBORDER=\"1\" COLOR=\"#292C37\" CELLSPACING=\"0\"><TR>"
                   L"<TD BGCOLOR=\"%s\">%.*s</TD>"
                   L"</TR></TABLE>>];\n", _nodeId, "", node->value.length, wcharStr);

    free(wcharStr);
    nodeId++;

    if (parentId != 0)
        fwprintf(file, L"        \"node%zd\" -> \"node%zd\":questionPort;\n", parentId, _nodeId);

    if (node->nodeLeft)
        CreateNodeGraph(file, node->nodeLeft, _nodeId, false);
    
    if (node->nodeRight)
        CreateNodeGraph(file, node->nodeRight, _nodeId, true);
}

#endif


///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool PrintNodeToFile(MathNode* node)
{
    assert(node);

    switch (node->expression.type)
    {
        case ME_NUMBER:
            printf("%lf", node->expression.me_number);
            break;
        case ME_CONST:
            switch (node->expression.me_constant)
            {
                case ME_PI:
                    puts("Pi");
                    break;
                case ME_EXP:
                    puts("Exp");
                    break;
                default:
                    return false;
            }
            break;
        case ME_OPERATOR:
            printf("%c", node->expression.me_operator);
            break;
        case ME_VARIABLE:
            printf("%c", node->expression.me_variable);
            break;
        default:
            return false;
    }
    return true;
}

static size_t SizeofTreeType(const char* expression)
{
    assert(expression);

    return strlen(expression) + 1;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

