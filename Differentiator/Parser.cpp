#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#include "../Tree/Tree.h"


unsigned char* SkipSpaceSymbols(unsigned char* ptr);


bool CheckCorrectInput(Text* text)
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

bool ParseFile(Tree* tree, Text* text)
{
    unsigned char* ptr = text->buffer;

    if (!CheckCorrectInput(text))
        return false;

    Stack stk = {};
    StackConstructor(&stk, sizeof(Node*));

    while (*ptr)
    {
        if (*ptr == '(')
        {
            Node* node = TreeNodeConstructor(nullptr);

            if (!node)
                return false;
            
            ptr++;
            ptr = SkipSpaceSymbols(ptr);
            
            unsigned char* strStart = ptr;

            while (*ptr != '}' && *ptr != '{')
                ptr++;
            ptr--;

            String string =
            {
                (char*)strStart,
                (size_t)(ptr - strStart + 1)
            };

            while (string.length > 0 && isspace((unsigned char)string.ptr[string.length - 1]))
                string.length--;

            node->value = string;

            StackPush(&stk, &node);
        }
        else if (*ptr == ')')
        {
            Node** _childNode  = (Node**)StackPop(&stk);
            Node** _parentNode = (Node**)StackPop(&stk);

            if (!_childNode)
                return false;

            if (!_parentNode) //  орень дерева
            {
                akinator->tree.root = *_childNode;
                break;
            }

            Node* childNode  = *_childNode;
            Node* parentNode = *_parentNode;

            if (parentNode->nodeRight == nullptr)
                TreeAddRightNode(parentNode, childNode);
            else
                TreeAddLeftNode(parentNode, childNode);

            StackPush(&stk, &parentNode);

        }
        ptr++;
    }

    StackDestructor(&stk);
}

unsigned char* SkipSpaceSymbols(unsigned char* ptr)
{
    while (*ptr && isspace(*ptr))
        ptr++;
    return ptr;
}