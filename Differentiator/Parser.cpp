#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#include "..\MathTree\MathTree.h"
#include "..\StackLibrary\Stack.h"
#include "Differentiator.h"


static char* SkipSpaceSymbols(char* ptr);

static void ParseNodeValue(char** ptr, MathNode* node);

static bool CheckCorrectInput(Text* text);


bool ParseDiffTree(Differentiator* diff, Text* text)
{
    char* ptr = text->buffer;

    if (!CheckCorrectInput(text))
        return false;

    Stack stk = {};
    StackConstructor(&stk, sizeof(MathNode*));

    while (*ptr)
    {
        ptr = SkipSpaceSymbols(ptr);

        if (*ptr == '(')
        {
            MathNode* node = TreeNodeConstructor(nullptr);

            if (!node)
                return false;
            
            StackPush(&stk, &node);
        }
        else if (*ptr == ')')
        {
            MathNode** _childNode  = (MathNode**)StackPop(&stk);
            MathNode** _parentNode = (MathNode**)StackPop(&stk);

            if (!_childNode)
                return false;

            if (!_parentNode) //  орень дерева
            {
                diff->tree.root = *_childNode;
                break;
            }

            MathNode* childNode  = *_childNode;
            MathNode* parentNode = *_parentNode;

            if (parentNode->nodeRight == nullptr)
                TreeAddRightNode(parentNode, childNode);
            else
                TreeAddLeftNode(parentNode, childNode);

            StackPush(&stk, &parentNode);
        }
        else
        {
            MathNode** _childNode  = (MathNode**)StackPop(&stk);

            if (!_childNode)
                return false;
            
            ParseNodeValue(&ptr, *_childNode);

            StackPush(&stk, _childNode);
        }
        ptr++;
    }
    StackDestructor(&stk);
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
            //puts("ќшибочна€ скобочна€ последовательность.\n"
            //     " оличество закрывающих скобок '{' больше количества открывающих '}'");
            return false;
        }

        ptr++;
    }

    return false;
}

static void ParseNodeValue(char** ptr, MathNode* node)
{
    *ptr = SkipSpaceSymbols(*ptr);

    char* string = *ptr;

    while (**ptr && **ptr != '(' || **ptr != ')')
        (*ptr)++;

    size_t strLength = (size_t)(*ptr - string + 1);
    string = *ptr;

    // Skip spaces right edge
    while (strLength > 0 && isspace((unsigned char)string[strLength - 1]))
        strLength--;

    if (sscanf(string, "%lf", &node->expression.))

    node->expression = string;
}

static char* SkipSpaceSymbols(char* ptr)
{
    while (*ptr && isspace((unsigned char)*ptr))
        ptr++;
    return ptr;
}