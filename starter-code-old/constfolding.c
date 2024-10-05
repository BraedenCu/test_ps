/*
   CONSTFOLDING.C : THIS FILE IMPLEMENTS THE CONSTANT FOLDING OPTIMIZATION
*/

#include "constfolding.h"
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL 
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO 
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/                                                                                                          
bool madeChange;

/*
******************************************************************************************
FUNCTION TO CALCULATE THE CONSTANT EXPRESSION VALUE 
OBSERVE THAT THIS IMPLEMENTATION CONSIDERS ADDITIONAL OPTIMIZATIONS SUCH AS:
1.  IDENTITY MULTIPLY = 1 * ANY_VALUE = ANY_VALUE - AVOID MULTIPLICATION CYCLE IN THIS CASE
2.  ZERO MULTIPLY = 0 * ANY_VALUE = 0 - AVOID MULTIPLICATION CYCLE
3.  DIVIDE BY ONE = ORIGINAL_VALUE - AVOID DIVISION CYCLE
4.  SUBTRACT BY ZERO = ORIGINAL_VALUE - AVOID SUBTRACTION
5.  MULTIPLICATION BY 2 = ADDITION BY SAME VALUE [STRENGTH REDUCTION]
******************************************************************************************
*/
long CalcExprValue(Node* node)
{
     long result;
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
             result = leftNode->value - rightNode->value;
             break;
         case NEGATE:
             result = -leftNode->value;
             break;
         default:
             break;
     }
     return result;
}


/*
**********************************************************************************************************************************
// YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/

void ConstFoldPerExpression(Node* right_node) {
    if (right_node == NULL) {
        return;
    }

    if (right_node->exprCode == OPERATION) {
        // Process left and right operands
        if (right_node->left != NULL) {
            ConstFoldPerExpression(right_node->left);
        }
        if (right_node->right != NULL) {
            ConstFoldPerExpression(right_node->right);
        }

        // Check if operands are constants
        bool isUnary = (right_node->opCode == NEGATE);
        bool leftIsConst = (right_node->left && right_node->left->exprCode == CONSTANT);
        bool rightIsConst = (right_node->right && right_node->right->exprCode == CONSTANT);

        if (isUnary && leftIsConst) {
            // Unary operation with constant operand
            long result = CalcExprValue(right_node);
            // Replace with constant node
            right_node->exprCode = CONSTANT;
            right_node->opCode = O_NONE;
            right_node->value = result;
            FreeExpression(right_node->left);
            right_node->left = NULL;
            madeChange = true;
        } else if (leftIsConst && rightIsConst) {
            // Binary operation with constant operands
            long result = CalcExprValue(right_node);
            // Replace with constant node
            right_node->exprCode = CONSTANT;
            right_node->opCode = O_NONE;
            right_node->value = result;
            FreeExpression(right_node->left);
            FreeExpression(right_node->right);
            right_node->left = NULL;
            right_node->right = NULL;
            madeChange = true;
        }
    } else if (right_node->exprCode == VARIABLE || right_node->exprCode == PARAMETER || right_node->exprCode == CONSTANT) {
        // Base case: do nothing
        return;
    }
}

/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO PROCESS THE CANDIDATE STATEMENTS AND PERFORM CONSTANT FOLDING 
WHEREVER APPLICABLE.
******************************************************************************************************
*/
void ConstFoldPerStatement(Node* stmtNode) {
    if (stmtNode == NULL) {
        return;
    }

    if (stmtNode->stmtCode == ASSIGN) {
        // Process the right-hand side expression
        ConstFoldPerExpression(stmtNode->right);
    } else if (stmtNode->stmtCode == RETURN) {
        // Process the return expression
        ConstFoldPerExpression(stmtNode->left);
    }
}


/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO IDENTIFY THE STATEMENTS THAT ARE ACTUAL CANDIDATES FOR CONSTANT FOLDING
AND CALL THE APPROPRIATE FUNCTION FOR THE IDENTIFIED CANDIDATE'S CONSTANT FOLDING
******************************************************************************************************
*/
void ConstFoldPerFunction(Node* funcNode) {
    Node *rightNode, *leftNode, *stmtNodeRight;
    long result;
    NodeList* statements = funcNode->statements;
    while(statements != NULL) {
        //stmtNodeRight = statements->node->right;
          /*
          *************************************************************************************
		TODO: YOUR CODE HERE
          **************************************************************************************
          */          
        Node* stmtNode = statements->node;
        ConstFoldPerStatement(stmtNode);
        statements = statements->next;                                                                                                                     
        //statements = statements->next;
        } 
}



/*
**********************************************************************************************************************************
// YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
********************************************************************************************************************************
*/

/*
*****************************************************************************************************
THIS FUNCTION ENSURES THAT THE CONSTANT FOLDING OPTIMIZATION IS DONE FOR EVERY FUNCTION IN THE PROGRAM
******************************************************************************************************
*/

bool ConstantFolding(NodeList* list) {
    madeChange = false;
    /*
    while(list != NULL) {
          /*
          *************************************************************************************
		TODO: YOUR CODE HERE
          **************************************************************************************
          
	    list = list->next;
    }
    */
    NodeList* current = list;

    while (current != NULL) {
        Node* funcNode = current->node;
        ConstFoldPerFunction(funcNode);
        current = current->next;
    }
    return madeChange;
}

/*
****************************************************************************************************************************
 END OF CONSTANT FOLDING
*****************************************************************************************************************************
*/                
