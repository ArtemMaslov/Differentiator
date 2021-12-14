#include <stdio.h>
#include <assert.h>
#include <ctype.h>


#include "MathParser.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

static bool ParseTreeNodeValue(char** ptr, MathNode* node);

static bool CheckCorrectInput(Text* text);

static void WriteTreeNodeToFile(MathNode* node, FILE* file, size_t recursiveLength);


bool ParseMathTree(Text* text, MathTree* tree)
{
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

void WriteTreeToFile(MathTree* tree, FILE* file)
{
    assert(tree);
    assert(file);

    if (tree->root)
        WriteTreeNodeToFile(tree->root, file, 0);
}

bool ReadTreeFromFile(MathTree* tree, Text* text, FILE* file)
{
    assert(tree);
    assert(file);

    TreeConstructor(tree);

    if (!ReadFile(text, file))
        return false;

    if (!ParseMathTree(text, tree))
        return false;

    return true;
}

static bool CheckCorrectInput(Text* text)
{
    char*  ptr = text->buffer;
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
            //puts("Ошибочная скобочная последовательность.\n"
            //     "Количество закрывающих скобок '{' больше количества открывающих '}'");
            return false;
        }

        ptr++;
    }

    return true;
}

static bool ParseTreeNodeValue(char** ptr, MathNode* node)
{
    *ptr = SkipSpaceSymbolsLeft(*ptr);

    char* string = *ptr;

    while (**ptr && **ptr != '(' && **ptr != ')')
        (*ptr)++;

    size_t strLength = (size_t)(*ptr - string);

    strLength = SkipSpaceSymbolsRight(string, strLength);

    if (!MathExpressionParseString(&node->expression, string, strLength))
    {
        printf("Ошибка чтения узла \"%.*s\".", strLength, string);
        return false;
    }
    
    return true;
}

static void WriteTreeNodeToFile(MathNode* node, FILE* file, size_t recursiveLength)
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