#include <stdio.h>
#include <assert.h>
#include <ctype.h>


#include "MathParser.h"
#include "..\..\Logs\Logs.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

static bool ParseTreeNodeValue(char** ptr, MathNode* node);

static bool CheckCorrectInput(const Text* text);

static void WriteTreeNodeToFile(const MathNode* node, FILE* file, const size_t recursiveLength);


bool ParseMathTree(const Text* text, MathTree* tree)
{
    assert(text);
    assert(tree);

    char* ptr = text->buffer;

    if (!CheckCorrectInput(text))
        return false;

    Stack stk = {};
    StackConstructor(&stk, sizeof(MathNode*));
    bool isLeft = true;

    while (*ptr)
    {
        ptr = SkipSpaceSymbolsLeft(ptr);

        if (*ptr == '(')
        {
            MathNode* node = TreeNodeConstructor(nullptr);

            if (!node)
                return false;
            
            StackPush(&stk, &node);
            isLeft = true;
        }
        else if (*ptr == ')')
        {
            MathNode** _childNode  = (MathNode**)StackPop(&stk);
            MathNode** _parentNode = (MathNode**)StackPop(&stk);

            if (!_childNode)
                return false;

            if (!_parentNode) // Корень дерева
            {
                tree->root = *_childNode;
                break;
            }

            MathNode* childNode  = *_childNode;
            MathNode* parentNode = *_parentNode;

            if (parentNode->nodeLeft == nullptr)
                TreeAddLeftNode(parentNode, childNode);
            else
                TreeAddRightNode(parentNode, childNode);

            StackPush(&stk, &parentNode);
        }
        else
        {
            MathNode** _childNode  = (MathNode**)StackPop(&stk);

            if (!_childNode)
                return false;
            
            if (!ParseTreeNodeValue(&ptr, *_childNode))
                return false;

            if ((*_childNode)->nodeLeft == nullptr && (*_childNode)->expression.type == ME_FUNCTION)
            {
                MathNode* nullNode = TreeNodeConstructor(nullptr);
                
                if (!nullNode)
                    return false;

                nullNode->expression.type = ME_UNKNOWN;
                (*_childNode)->nodeLeft = nullNode;
            }

            ptr--;
            StackPush(&stk, _childNode);
        }
        ptr++;
    }
    StackDestructor(&stk);

    return true;
}

void WriteTreeToFile(const MathTree* problem, FILE* file)
{
    assert(problem);
    assert(file);

    if (problem->root)
        WriteTreeNodeToFile(problem->root, file, 0);
}

bool ReadTreeFromFile(MathTree* tree, Text* text, FILE* file)
{
    assert(tree);
    assert(file);
    assert(text);

    TreeConstructor(tree);

    if (!ReadFile(text, file))
        return false;

    if (!ParseMathTree(text, tree))
        return false;

    return true;
}

static bool CheckCorrectInput(const Text* text)
{
    assert(text);

    const char* ptr = text->buffer;
    size_t lBracketsCount = 0;
    size_t rBracketsCount = 0;

    while (*ptr)
    {
        if (*ptr == '(')
            lBracketsCount++;
        else if (*ptr == ')')
            rBracketsCount++;

        if (lBracketsCount < rBracketsCount)
        {
            LogLine("Ошибочная скобочная последовательность.\n"
                    "Количество закрывающих скобок '{' больше количества открывающих '}'", LOG_ERROR, true);
            return false;
        }

        ptr++;
    }

    return true;
}

static bool ParseTreeNodeValue(char** ptr, MathNode* node)
{
    assert(ptr);
    assert(*ptr);
    assert(node);

    *ptr = SkipSpaceSymbolsLeft(*ptr);

    char* string = *ptr;

    while (**ptr && **ptr != '(' && **ptr != ')')
        (*ptr)++;

    size_t strLength = (size_t)(*ptr - string);

    strLength = SkipSpaceSymbolsRight(string, strLength);

    if (!MathExpressionParseString(&node->expression, string, strLength))
    {
        LogLine("Ошибка чтения узла", LOG_ERROR, true);
        return false;
    }
    
    return true;
}

static void WriteTreeNodeToFile(const MathNode* node, FILE* file, const size_t recursiveLength)
{
    assert(node);
    assert(file);

    fprintf(file, "%*s(\n", recursiveLength * 4, "");
    
    if (node->nodeLeft && node->nodeLeft->expression.type != ME_UNKNOWN)
        WriteTreeNodeToFile(node->nodeLeft, file, recursiveLength + 1);

    fprintf(file, "%*s", (recursiveLength + 1) * 4, "");
    PrintMathExpression(&node->expression, file);
    fputc('\n', file);

    if (node->nodeRight && node->nodeRight->expression.type != ME_UNKNOWN)
        WriteTreeNodeToFile(node->nodeRight, file, recursiveLength + 1);
    
    fprintf(file, "%*s)\n", recursiveLength * 4, "");
}