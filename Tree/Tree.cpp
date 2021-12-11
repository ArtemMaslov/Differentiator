#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <Windows.h>


#include "Tree.h"
#include "..\Differentiator\Config.h"


#include "..\Logs\Logs.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
/// Уважаемый пользователь, следующие определения нужно изменить для конкретной задачи
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |   \\\
///  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |   \\\
/// \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \\\
///  *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *   \\\


void PrintNodeToFile(Node* root)
{
    assert(root);

    printf(treeTypeFormat "\n", root->value.ptr);
}

static size_t SizeofTreeType(const char* value)
{
    assert(value);

    return strlen(value) + 1;
}

static int CompareObject(treeType obj1, treeType obj2)
{
    size_t min = (obj1.length < obj2.length)? obj1.length: obj2.length;
    size_t index = 0;
    bool res = true;

    while (res && index < min)
    {
        if (obj1.ptr[index] == obj2.ptr[index])
        {
            index++;
        }
        else if (obj1.ptr[index] < obj2.ptr[index])
        {
            return -1;
        }
        else
        { 
            return 1;
        }
    }

    if (obj1.length == obj2.length)
        return 0;
    else if (obj1.length < obj2.length)
        return -1;
    else
        return 1;
}

///  *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *   \\\
/// /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  \\\
///  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |   \\\
///  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |   \\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

static void CreateNodeGraph(FILE* file, Node* node, size_t parentId, bool IsRight);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeConstructor(Tree* tree)
{
    LogLine("Вызван TreeConstructor()", LOG_DEBUG);
    assert(tree);

    tree->root = nullptr;

    return true;
}

Node* TreeNodeConstructor(const treeType* value)
{
    LogLine("Вызван TreeNodeConstructor()", LOG_DEBUG);
    //assert(value);

    Node* node = (Node*)calloc(1, sizeof(Node));

    if (!node)
    {
        LogLine("NodeConstructor: не хватает памяти для создания нового узла", LOG_ERROR, true);
        return nullptr;
    }

    node->nodeLeft = nullptr;
    node->nodeRight = nullptr;

    if (value)
        memcpy(&node->value, value, sizeof(treeType));

    return node;
}

bool TreeDestructor(Tree* tree) 
{
    LogLine("Вызван TreeDestructor()", LOG_DEBUG);
    // assert(tree);
    
    if (tree)
        TreeNodeDestructor(tree->root);

    return true;
}

bool TreeNodeDestructor(Node* node)
{
    LogLine("Вызван TreeNodeDestructor()", LOG_DEBUG);
    // assert(node);

    if (node)
    {
        if (node->nodeLeft)
            TreeNodeDestructor(node->nodeLeft);
        if (node->nodeRight)
            TreeNodeDestructor(node->nodeRight);

        if (node->value.ptr)
            free (node->value.ptr);
        free(node);
    }

    return true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeAddLeftNode(Node* parent, Node* child)
{
    LogLine("Вызван TreeAddLeftNode()", LOG_DEBUG);
    assert(parent);
    assert(child);

    parent->nodeLeft = child;
    child->parent = parent;

    return true;
}

bool TreeAddRightNode(Node* parent, Node* child)
{
    LogLine("Вызван TreeAddRightNode()", LOG_DEBUG);
    assert(parent);
    assert(child);

    parent->nodeRight = child;
    child->parent = parent;

    return true;
}

void TreeMeasure(Tree* tree, Node* node, size_t length)
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

bool IsLeaf(Node* node)
{
    if (node && (node->nodeLeft == nullptr || node->nodeRight == nullptr))
        return true;
    else
        return false;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

void TreeVisit(Node* node, void (*nodeFunction)(Node*), VisitCallType type)
{
    LogLine("Вызван TreeVisit()", LOG_DEBUG);
    assert(node);
    assert(nodeFunction);
    
    if (type & VisitCallType::PREORDER)
        printf(treeTypeFormat, node->value.ptr);

    if (node->nodeLeft)
        TreeVisit(node->nodeLeft, nodeFunction, type);
    
    if (type & VisitCallType::INORDER)
        printf(treeTypeFormat, node->value.ptr);

    if (node->nodeRight)
        TreeVisit(node->nodeRight, nodeFunction, type);
    
    if (type & VisitCallType::POSTORDER)
        printf(treeTypeFormat, node->value.ptr);
}

Node* TreeFindObject(Node* node, treeType object)
{
    LogLine("Вызван TreeFindObject()", LOG_DEBUG);
    assert(node);
    
    if (CompareObject(object, node->value) == 0)
        return node;
    
    Node* result = nullptr;

    if (node->nodeLeft)
        result = TreeFindObject(node->nodeLeft, object);
    
    if (node->nodeRight && !result)
        result = TreeFindObject(node->nodeRight, object);
    
    return result;
}

Node* TreeFindObjectStack(Node* node, treeType object, Stack* stk)
{
    LogLine("Вызван TreeFindObjectStack()", LOG_DEBUG);
    assert(stk);
    assert(node);

    StackPush(stk, &node);

    if (CompareObject(object, node->value) == 0)
        return node;

    Node* result = nullptr;

    if (node->nodeLeft)
        result = TreeFindObjectStack(node->nodeLeft, object, stk);
        
    if (node->nodeRight && !result)
        result = TreeFindObjectStack(node->nodeRight, object, stk);

    if (!result)
        StackPop(stk, nullptr);

    return result;
}

Node* GetNodeFromStack(Stack* stk, size_t index)
{
    LogLine("Вызван GetNodeFromStack()", LOG_DEBUG);
    assert(stk);

    Node** nodeptr = (Node**)StackGetElemAt(stk, index);

    if (nodeptr)
        return *nodeptr;

    return nullptr;
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

    MultiByteToWideChar(CP_ACP, 0, node->value.ptr, node->value.length + 1, wcharStr, node->value.length + 1);
    wcharStr[node->value.length] = '\0';
    
    fwprintf(file, L"    node%zd[label = <<TABLE BORDER=\"0\" CELLBORDER=\"1\" COLOR=\"#292C37\" CELLSPACING=\"0\"><TR>"
                   L"<TD PORT=\"noPort\" BGCOLOR=\"red2\">no</TD>"
                   L"<TD PORT=\"questionPort\" BGCOLOR=\"papayawhip\">%.*s</TD>"
                   L"<TD PORT=\"yesPort\" BGCOLOR=\"olivedrab3\">yes</TD></TR></TABLE>>];\n", _nodeId, node->value.length, wcharStr);

    free(wcharStr);
    nodeId++;

    if (parentId != 0)
        fwprintf(file, L"        \"node%zd\":%s -> \"node%zd\":questionPort [color=\"%s\"];\n",
            parentId, IsRight?L"yesPort":L"noPort", _nodeId, IsRight?L"olivedrab3":L"red2");

    if (node->nodeLeft)
        CreateNodeGraph(file, node->nodeLeft, _nodeId, false);
    
    if (node->nodeRight)
        CreateNodeGraph(file, node->nodeRight, _nodeId, true);
}

#endif