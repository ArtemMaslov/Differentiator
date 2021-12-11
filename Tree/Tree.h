#ifndef MY_TREE_H
#define MY_TREE_H

#include "..\Differentiator\Config.h"
#include "..\StringLibrary\StringLibrary.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
/// Уважаемый пользователь, следующие определения нужно изменить для конкретной задачи
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |   \\\
///  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |   \\\
/// \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \|/  \\\
///  *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *   \\\

typedef String treeType;

#define treeTypeFormat "%s"

#include "..\StackLibrary\Stack.h"


///  *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *   \\\
/// /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  /|\  \\\
///  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |   \\\
///  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |   \\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

enum VisitCallType
{
    PREORDER  = 1,
    INORDER   = 2,
    POSTORDER = 4
};

struct Node
{
    /// Значение узла.
    treeType value;
    /// Родитель узла.
    Node* parent;
    /// Левый потомок.
    Node* nodeLeft;
    /// Правый потомок.
    Node* nodeRight;
    /// true, если была выделена динамическая память для value.
    bool allocated;
};

struct Tree
{
    /// Корень дерева.
    Node* root;
    /// Наибольшее количество узлов дерева в глубину, чтобы изменить нужно вызвать TreeMeasure().
    size_t treeLength;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeConstructor(Tree* tree);

Node* TreeNodeConstructor(const treeType* value);

bool TreeDestructor(Tree* tree);

bool TreeNodeDestructor(Node* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeAddRightNode(Node* parent, Node* child);

bool TreeAddLeftNode(Node* parent, Node* child);

void TreeMeasure(Tree* tree, Node* node, size_t length);

bool IsLeaf(Node* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

void TreeVisit(Node* node, void (*nodeFunction)(Node*), VisitCallType type);

Node* TreeFindObject(Node* node, treeType object);

Node* TreeFindObjectStack(Node* node, treeType object, Stack* stk);

Node* GetNodeFromStack(Stack* stk, size_t index);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#ifdef GRAPHVIZ

bool CreateTreeGraph(const char* fileName, Tree* tree);
#else

#define CreateTreeGraph(fileName, tree) false;

#endif

#endif
