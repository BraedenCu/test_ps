/*
********************************************************************************
  CONSTPROP.C : IMPLEMENT THE DOWNSTREAM CONSTANT PROPAGATION OPTIMIZATION HERE
*********************************************************************************
*/

#include "constprop.h"

refConst *lastNode = NULL, *headNode = NULL;
//bool madeChange = false;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO FREE THE CONSTANTS-ASSOCIATED VARIABLES LIST
************************************************************************
*/
void FreeConstList() {
    refConst* tmp;
    while (headNode != NULL) {
        tmp = headNode;
        headNode = headNode->next;
        free(tmp);
    }
}

/*
*************************************************************************
  FUNCTION TO ADD A CONSTANT VALUE AND THE ASSOCIATED VARIABLE TO THE LIST
**************************************************************************
*/
void UpdateConstList(char* name, long val) {
    refConst* node = malloc(sizeof(refConst));
    if (node == NULL) return;
    node->name = name;
    node->val = val;
    node->next = NULL;
    if (headNode == NULL) {
        lastNode = node;
        headNode = node;
    } else {
        lastNode->next = node;
        lastNode = node;
    }
}

/*
*****************************************************************************
  FUNCTION TO LOOKUP IF A CONSTANT ASSOCIATED VARIABLE IS ALREADY IN THE LIST
******************************************************************************
*/
refConst* LookupConstList(char* name) {
    refConst *node = headNode;
    while (node != NULL) {
        if (!strcmp(name, node->name))
            return node;
        node = node->next;
    }
    return NULL;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/

/*
************************************************************************************
  FUNCTION TO PROPAGATE CONSTANTS IN EXPRESSIONS
*************************************************************************************
*/
Node* PropagateExpr(Node* node) {
    if (node == NULL) return NULL;

    if (node->exprCode == VARIABLE) {
        refConst* constNode = LookupConstList(node->name);
        if (constNode != NULL) {
            // Replace variable with constant
            Node* newConstNode = CreateNumber(constNode->val);
            // Free the variable node (do not free node->left, as it may be referenced)
            free(node->name);
            free(node);
            madeChange = true;
            return newConstNode;
        }
    } else if (node->exprCode == OPERATION) {
        if (node->opCode == FUNCTIONCALL) {
            // Propagate constants in function call arguments
            NodeList* args = node->arguments;
            while (args != NULL) {
                args->node = PropagateExpr(args->node);
                args = args->next;
            }
        } else {
            // Propagate constants in left and right operands
            node->left = PropagateExpr(node->left);
            node->right = PropagateExpr(node->right);
        }
    }
    // For CONSTANT and PARAMETER nodes, do nothing
    return node;
}

/*
************************************************************************************
  THIS FUNCTION IS MEANT TO UPDATE THE CONSTANT LIST WITH THE ASSOCIATED VARIABLE
  AND CONSTANT VALUE WHEN ONE IS SEEN. IT SHOULD ALSO PROPAGATE THE CONSTANTS WHEN 
  APPLICABLE.
*************************************************************************************
*/
void TrackConst(NodeList* statements) {
    Node* node;
    while (statements != NULL) {
        node = statements->node;
        if (node->stmtCode == ASSIGN) {
            // For assignment statements
            Node* rhs = node->right;
            // Propagate constants in the RHS expression
            node->right = PropagateExpr(rhs);

            // If RHS is a constant, add to the constant list
            if (node->right->exprCode == CONSTANT) {
                UpdateConstList(node->name, node->right->value);
            } else {
                // If variable is being reassigned a non-constant value, remove from const list
                refConst* existing = LookupConstList(node->name);
                if (existing != NULL) {
                    // Remove from const list
                    refConst* prev = NULL;
                    refConst* curr = headNode;
                    while (curr != NULL) {
                        if (!strcmp(curr->name, node->name)) {
                            if (prev == NULL) {
                                headNode = curr->next;
                            } else {
                                prev->next = curr->next;
                            }
                            if (curr == lastNode) {
                                lastNode = prev;
                            }
                            free(curr);
                            break;
                        }
                        prev = curr;
                        curr = curr->next;
                    }
                }
            }
        } else if (node->stmtCode == RETURN) {
            // For return statements
            node->left = PropagateExpr(node->left);
        }
        statements = statements->next;
    }
}

/*
************************************************************************************
  TOP-LEVEL FUNCTION TO PERFORM CONSTANT PROPAGATION ON ALL FUNCTIONS
*************************************************************************************
*/
bool ConstProp(NodeList* worklist) {
    madeChange = false;
    while (worklist != NULL) {
        // Initialize constant list for each function
        FreeConstList();
        headNode = NULL;
        lastNode = NULL;
        Node* funcNode = worklist->node;
        TrackConst(funcNode->statements);
        worklist = worklist->next;
    }
    // Clean up after all functions
    FreeConstList();
    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/