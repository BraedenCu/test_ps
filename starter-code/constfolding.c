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
     long result = 0;
     Node *leftNode, *rightNode;
     leftNode = node->left;
     rightNode = node->right; 
     switch(node->opCode){
         case MULTIPLY:
             if(leftNode->value == 1) {
                 result = rightNode->value;
             } 
             else if(rightNode->value == 1) {
                 result = leftNode->value;
             }
             else if(leftNode->value == 0 || rightNode->value == 0) {
                 result = 0;
             }
             else if(leftNode->value == 2) {
                 result = rightNode->value + rightNode->value;
             }              
             else if(rightNode->value == 2) {
                 result = leftNode->value + leftNode->value;
             }
             else {
                 result = leftNode->value * rightNode->value;
             }
             break;
         case DIVIDE:
             if(rightNode->value == 1) {
                 result = leftNode->value;
             }
             else {
                 result = leftNode->value / rightNode->value;
             }
             break;
         case ADD:
             result = leftNode->value + rightNode->value;
             break;
         case SUBTRACT:
             if(rightNode->value == 0) {
                 result = leftNode->value;
             } else {
                 result = leftNode->value - rightNode->value;
             }
             break;
         case NEGATE:
             result = -leftNode->value;
             break;
         case BAND:
             result = leftNode->value & rightNode->value;
             break;
         case BOR:
             result = leftNode->value | rightNode->value;
             break;
         case BXOR:
             result = leftNode->value ^ rightNode->value;
             break;
         case BSHL:
             result = leftNode->value << rightNode->value;
             break;
         case BSHR:
             result = leftNode->value >> rightNode->value;
             break;
         default:
             // For unhandled operations, return 0
             result = 0;
             break;
     }
     return result;
}

/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO PROCESS THE EXPRESSION AND PERFORM CONSTANT FOLDING WHEREVER APPLICABLE.
******************************************************************************************************
*/
Node* FoldExpr(Node* node) {
    if (node == NULL) return NULL;

    if (node->exprCode == OPERATION) {
        // Recursively fold the left and right sub-expressions
        node->left = FoldExpr(node->left);
        node->right = FoldExpr(node->right);

        // Check if the operation can be folded
        if (node->opCode == NEGATE) {
            if (node->left && node->left->exprCode == CONSTANT) {
                long result = CalcExprValue(node);
                Node* constNode = CreateNumber(result);
                FreeExpression(node);
                madeChange = true;
                return constNode;
            }
        } else if (node->left && node->right &&
                   node->left->exprCode == CONSTANT && node->right->exprCode == CONSTANT) {
            long result = CalcExprValue(node);
            Node* constNode = CreateNumber(result);
            FreeExpression(node);
            madeChange = true;
            return constNode;
        }
    } else if (node->exprCode == VARIABLE || node->exprCode == CONSTANT) {
        // No folding needed for variables or constants
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
void FoldStmt(Node* stmt) {
    if (stmt == NULL) return;

    if (stmt->stmtCode == ASSIGN) {
        stmt->right = FoldExpr(stmt->right);
    }
    else if (stmt->stmtCode == RETURN) {
        stmt->left = FoldExpr(stmt->left);
    }
}

/*
*****************************************************************************************************
THIS FUNCTION PROCESSES EACH FUNCTION AND APPLIES CONSTANT FOLDING TO ITS STATEMENTS
******************************************************************************************************
*/
void FoldFunc(Node* funcNode) {
    NodeList* statements = funcNode->statements;
    while(statements != NULL) {
        FoldStmt(statements->node);
        statements = statements->next;
    }
}

/*
*****************************************************************************************************
THIS FUNCTION ENSURES THAT THE CONSTANT FOLDING OPTIMIZATION IS DONE FOR EVERY FUNCTION IN THE PROGRAM
******************************************************************************************************
*/
bool ConstantFolding(NodeList* list) {
    madeChange = false;
    while(list != NULL) {
        FoldFunc(list->node);
        list = list->next;
    }
    return madeChange;
}

/*
****************************************************************************************************************************
 END OF CONSTANT FOLDING
*****************************************************************************************************************************
*/