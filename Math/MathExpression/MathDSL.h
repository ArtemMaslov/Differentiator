
#define SET_NUMBER(number, node) (node)->expression.me_number = number

#define SET_FUNCTION(function, node) (node)->expression.me_function = function

#define SET_OPERATOR(operator, node) (node)->expression.me_operator = operator

#define SET_CONSTANT(constant, node) (node)->expression.me_constant = constant

#define SET_TYPE(expr_type, node) (node)->expression.type = expr_type

#define GET_TYPE(node) (node)->expression.type
#define TYPE_EQUAL(node, type) (GET_TYPE(node) == type)
#define GET_NUM(node) (node)->expression.me_number
#define GET_OPER(node) (node)->expression.me_operation
#define GET_FUNC(node) (node)->expression.me_function
#define GET_CONST(node) (node)->expression.me_constant
#define GET_VAR(node) (node)->expression.me_variable

#define LEFT node->nodeLeft
#define RIGHT node->nodeRight
#define X node->nodeRight
#define F node

#define FUNC(type, arg)                                         \
    NewFunctionNode(type, arg)

#define NUM(number)                                             \
    NewNumberNode(number)

#define NODE_EXP NewConstantNode(ME_EXP)

#define NODE_PI  NewConstantNode(ME_PI)

#define COPY(node)                                              \
    TreeCopyRecursive(node)

#define ADD(loper, roper)                                       \
    NewOperatorNode(ME_ADDITION, loper, roper)

#define SUB(loper, roper)                                       \
    NewOperatorNode(ME_SUBTRACTION, loper, roper)

#define MUL(loper, roper)                                       \
    NewOperatorNode(ME_MULTIPLICATION, loper, roper)

#define DIV(loper, roper)                                       \
    NewOperatorNode(ME_DIVISION, loper, roper)

#define POW(loper, roper)                                       \
    NewOperatorNode(ME_POWER, loper, roper)

#define POW2(loper)                                             \
    NewOperatorNode(ME_POWER, loper, NUM(2))

#define NEW_NODE(value, type, leftNode, rightNode)              \
    {                                                           \
        MathNode* node = TreeNodeConstructor(nullptr);          \
                                                                \
        if (!node)                                              \
            return nullptr;                                     \
                                                                \
        if (leftNode)                                           \
            TreeAddLeftNode(node, leftNode);                    \
        if (rightNode)                                          \
            TreeAddRightNode(node, rightNode);                  \
                                                                \
        SET_##type(value, node);                                \
        SET_TYPE(ME_##type, node);                              \
        return node;                                            \
    }


static MathNode* NewConstantNode(MathConstant constant)
{
    NEW_NODE(constant, CONSTANT, nullptr, nullptr);
}

static MathNode* NewFunctionNode(MathFunction function, MathNode* argument)
{
    if (!argument)
        return nullptr;

    NEW_NODE(function, FUNCTION, nullptr, argument);
}

static MathNode* NewOperatorNode(MathOperator oper, MathNode* lOperand, MathNode* rOperand)
{
    if (!lOperand || !rOperand)
    {
        free(lOperand);
        free(rOperand);
        return nullptr;
    }

    NEW_NODE(oper, OPERATOR, lOperand, rOperand);
}

static MathNode* NewNumberNode(double number)
{
    NEW_NODE(number, NUMBER, nullptr, nullptr);
}
