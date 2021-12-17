#include <stdio.h>
#include <assert.h>
#include <Windows.h>
#include <stdarg.h>
#include <ctime>


#include "Latex.h"
#include "..\Logs\Logs.h"


static bool AddNodeToLatexVariables(MathNode* node, Latex* latex);

static void ChangeNodeToVariable(Latex* latex, MathNode* node, char symbol);

static bool DoNodeReplacement(MathNode* node, Latex* latex);

static void DoNodeReverseRaplacement(Latex* latex);

/**
 * @brief       ���������� ���������� ����� ��������� � ����, ��������� ��������� Replacements.
 * @param latex ��������� �� ��������� Latex.
*/
void PrintReplacedNodes(Latex* latex)
{
    assert(latex);

    size_t index = latex->repl.index;
    if (index <= 0)
    {
        fputs("\n\n", latex->file);
        return;
    }

    fputs("� ������ ������� ������� ��������� �����������:\n\n", latex->file);

    for (size_t st = 0; st < index; st++)
    {
        fprintf(latex->file, "\\begin{equation}\n%c = ", 'A' + st);

        PrintMathNodeLatex(latex->repl.nodes[st].replacedTree, latex->file);
    
        fprintf(latex->file, "\n\\end{equation}\n\n", latex->formulaCounter++);
    }
    
    DoNodeReverseRaplacement(latex);
}

/**
 * @brief            ���������� ������ ������� ���������.
 * ������������� � ������� MaxNodesCount.
 * @param node       ��������� �� ������ ���������.
 * @param latex      ��������� �� ��������� Latex.
 * @param clearIndex ���������� � true, ���� ��� ������ ������.
*/
void DoReplacement(MathNode* node, Latex* latex, bool clearIndex)
{
    assert(node);
    assert(latex);

    if (clearIndex)
        latex->repl.index = 0;
    
    TreeMeasure(node);
    DoNodeReplacement(node, latex);
}

/**
 * @brief       ��������� �������� ������, ����� ����������� ����������� ���������
 * @param latex ��������� �� ��������� Latex.
*/
static void DoNodeReverseRaplacement(Latex* latex)
{
    assert(latex);

    for (size_t st = 0; st < latex->repl.index; st++)
    {
        ReplNode replacement = latex->repl.nodes[st];
        
        memcpy(replacement.treeRoot, replacement.replacedTree, sizeof(MathNode));
        replacement.replacedTree->nodeLeft = replacement.replacedTree->nodeRight = nullptr;

        TreeNodeDestructor(replacement.replacedTree);
    }
}

/**
 * @brief       ��������� ������ ���������. DoReplacement �������� ������� ���� �������.
 * @param node  ��������� �� ���������.
 * @param latex ��������� �� ��������� Latex.
 * @return      false, ���� �� ������� ������.
*/
static bool DoNodeReplacement(MathNode* node, Latex* latex)
{
    assert(node);
    assert(latex);

    bool replaced = false;
    if (node->nodeLeft)
        replaced =  DoNodeReplacement(node->nodeLeft, latex);
    if (node->nodeRight)
        replaced |= DoNodeReplacement(node->nodeRight, latex);

    if (replaced)
    {
        node->childCount = 1;
        if (node->nodeLeft)
            node->childCount += node->nodeLeft->childCount;
        if (node->nodeRight)
            node->childCount += node->nodeRight->childCount;
    }
    if (node->expression.type == ME_OPERATOR && node->childCount > MaxNodesCount && node->parent)
    {
        if (!AddNodeToLatexVariables(node, latex))
            return false;
        replaced = true;
    }

    return replaced;
}

/**
 * @brief       �������� ��� ���� �� ���������� � �������� �����, ������� � 'A'.
 * @param node  ��������� �� ���������.
 * @param latex ��������� �� ��������� Latex.
*/
static void ChangeNodeToVariable(Latex* latex, MathNode* node, char symbol)
{
    assert(latex);
    assert(node);

    node->expression.me_variable = symbol;
    node->expression.type = ME_VARIABLE;
    node->childCount = 1;
    node->nodeLeft = node->nodeRight = nullptr;
}

/**
 * @brief         ��������� ���� � ��������� Replacements.
 * @param node    ��������� �� ������ ���������.
 * @param latex   ��������� �� ��������� Latex.
 * @return        false, ���� �� ������� ������.
*/
static bool AddNodeToLatexVariables(MathNode* node, Latex* latex)
{
    assert(node);
    assert(latex);

    size_t index = latex->repl.index;
    if (index >= latex->repl.size)
    {
        ReplNode* arr = (ReplNode*)calloc(latex->repl.size * 2, sizeof(ReplNode));

        if (!arr)
        {
            LogLine("������ ��������� ������.", LOG_ERROR, true);
            return false;
        }
        latex->repl.nodes = arr;
    }
    
    MathNode* newNode = TreeNodeConstructor(nullptr);
    if (!newNode)
        return false;

    latex->repl.nodes[index].replacedTree = newNode;
    latex->repl.nodes[index].treeRoot = node;
    latex->repl.index++;
    
    memcpy(newNode, node, sizeof(MathNode));
    newNode->parent = nullptr;

    ChangeNodeToVariable(latex, node, 'A' + latex->repl.index - 1);
    return true;
}