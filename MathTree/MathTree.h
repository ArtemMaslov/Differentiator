#ifndef MY_TREE_H
#define MY_TREE_H


#include "..\Differentiator\Config.h"
#include "..\StackLibrary\Stack.h"


enum MathConstant
{
    ME_PI  = 0,
    ME_EXP = 1,
};

const char MathConstantStrings[][4] = 
{
    "pi",
    "exp"
};

enum NodeType
{
    ME_NUMBER   = 0,
    ME_CONST    = 1,
    ME_VARIABLE = 2,
    ME_OPERATOR = 3,
};

struct MathExpression
{
    union
    {    
        double       me_number;
        char         me_operator;
        char         me_variable;
        MathConstant me_constant;
    };
    NodeType type;
};


struct MathNode
{
    /// Значение узла.
    MathExpression expression;
    /// Родитель узла.
    MathNode* parent;
    /// Левый потомок.
    MathNode* nodeLeft;
    /// Правый потомок.
    MathNode* nodeRight;
};

struct MathTree
{
    /// Корень дерева.
    MathNode* root;
    /// Наибольшее количество узлов дерева в глубину, чтобы изменить нужно вызвать TreeMeasure().
    size_t treeLength;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeConstructor(MathTree* tree);

MathNode* TreeNodeConstructor(const MathExpression* expression);

bool TreeDestructor(MathTree* tree);

bool TreeNodeDestructor(MathNode* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeAddRightNode(MathNode* parent, MathNode* child);

bool TreeAddLeftNode(MathNode* parent, MathNode* child);

void TreeMeasure(MathTree* tree, MathNode* node, size_t length);

bool IsLeaf(MathNode* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

MathNode* TreeFindObject(MathNode* node, MathExpression object);

MathNode* TreeFindObjectStack(MathNode* node, MathExpression object, Stack* stk);

MathNode* GetNodeFromStack(Stack* stk, size_t index);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool MathExpressionsEqual(MathExpression* expr1, MathExpression* expr2);

bool MathExpressionParseString(MathExpression* expr, const char* ptr);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#ifdef GRAPHVIZ

bool CreateTreeGraph(const char* fileName, Tree* tree);
#else

#define CreateTreeGraph(fileName, tree) false;

#endif

#endif
