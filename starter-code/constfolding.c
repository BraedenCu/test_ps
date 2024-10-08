/*
   CONSTFOLDING.C : THIS FILE IMPLEMENTS THE CONSTANT FOLDING OPTIMIZATION
*/

#include "constfolding.h"

bool madeChange;

/*
******************************************************************************************
FUNCTION TO CALCULATE THE CONSTANT EXPRESSION VALUE 
OBSERVE THAT THIS IMPLEMENTATION CONSIDERS ADDITIONAL OPTIMIZATIONS SUCH AS:
1. IDENTITY MULTIPLY = 1 * ANY_VALUE = ANY_VALUE - AVOID MULTIPLICATION CYCLE IN THIS CASE
2. ZERO MULTIPLY = 0 * ANY_VALUE = 0 - AVOID MULTIPLICATION CYCLE
3. DIVIDE BY ONE = ORIGINAL_VALUE - AVOID DIVISION CYCLE
4. SUBTRACT BY ZERO = ORIGINAL_VALUE - AVOID SUBTRACTION
5. MULTIPLICATION BY 2 = ADDITION BY SAME VALUE [STRENGTH REDUCTION]
******************************************************************************************
*/
long CalcExprValue(Node* node)
{
    Node *left_node, *right_node;
    left_node = node->left;
    right_node = node->right; 
    long output = 0;
    switch(node->opCode)
    {
        case MULTIPLY:
            if(left_node->value == 1) 
            {
                output = right_node->value;
            } 
            else if(right_node->value == 1) 
            {
                output = left_node->value;
            }
            else if(left_node->value == 0 || right_node->value == 0) 
            {
                output = 0;
            }
            else if(left_node->value == 2) 
            {
                output = right_node->value + right_node->value;
            }              
            else if(right_node->value == 2) 
            {
                output = left_node->value + left_node->value;
            }
            else 
            {
                output = left_node->value * right_node->value;
            }
            break;

        case DIVIDE:
            if(right_node->value == 1) 
            {
                output = left_node->value;
            }
            else 
            {
                output = left_node->value / right_node->value;
            }
            break;

        case ADD:
            output = left_node->value + right_node->value;
            break;

        case SUBTRACT:
            if(right_node->value == 0) 
            {
                output = left_node->value;
            } 
            else 
            {
                output = left_node->value - right_node->value;
            }
            break;
            
        case NEGATE:
            output = -left_node->value;
            break;

        case BAND:
            output = left_node->value & right_node->value;
            break;

        case BOR:
            output = left_node->value | right_node->value;
            break;

        case BXOR:
            output = left_node->value ^ right_node->value;
            break;

        case BSHL:
            output = left_node->value << right_node->value;
            break;

        case BSHR:
            output = left_node->value >> right_node->value;
            break;

        default:
            output = 0;
            break;
    }
    return output;
}

/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO PROCESS THE EXPRESSION AND PERFORM CONSTANT FOLDING WHEREVER APPLICABLE.
******************************************************************************************************
*/
Node* ConstFoldPerStatement(Node* node) 
{
    if (node == NULL) 
    {
        return NULL;
    }

    if (node->exprCode == OPERATION) 
    {
        node->left = ConstFoldPerStatement(node->left);

        node->right = ConstFoldPerStatement(node->right);

        if (node->opCode == NEGATE)
         {
            if (node->left && node->left->exprCode == CONSTANT) 
            {
                long output = CalcExprValue(node);
                Node* constNode = CreateNumber(output);
                FreeExpression(node);
                madeChange = true;

                return constNode;
            }
        } 
        else if (node->left && node->right && node->left->exprCode == CONSTANT && node->right->exprCode == CONSTANT) 
        {
            long output = CalcExprValue(node);
            Node* constNode = CreateNumber(output);
            FreeExpression(node);
            madeChange = true;

            return constNode;
        }
    } 
    else if (node->exprCode == VARIABLE || node->exprCode == CONSTANT) 
    {
        return node;
    }
    
    return node;
}

/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO IDENTIFY THE STATEMENTS THAT ARE ACTUAL CANDIDATES FOR CONSTANT FOLDING
AND CALL THE APPROPRIATE FUNCTION FOR THE IDENTIFIED CANDIDATE'S CONSTANT FOLDING
******************************************************************************************************
*/
void ConstFoldPerFunction(Node* function_node) 
{
    if (function_node == NULL) 
    {
        return;
    }
    if (function_node->stmtCode == ASSIGN) 
    {
        function_node->right = ConstFoldPerStatement(function_node->right);
    }
    else if (function_node->stmtCode == RETURN) 
    {
        function_node->left = ConstFoldPerStatement(function_node->left);
    }
}

/*
*****************************************************************************************************
THIS FUNCTION ENSURES THAT THE CONSTANT FOLDING OPTIMIZATION IS DONE FOR EVERY FUNCTION IN THE PROGRAM
******************************************************************************************************
*/
bool ConstantFolding(NodeList* list) 
{
    madeChange = false;
    while(list != NULL) 
    {
        NodeList* statements = list->node->statements;
        while(statements != NULL) 
        {
            ConstFoldPerFunction(statements->node);
            statements = statements->next;
        }
        list = list->next;
    }
    return madeChange;
}

/*
****************************************************************************************************************************
 END OF CONSTANT FOLDING
*****************************************************************************************************************************
*/