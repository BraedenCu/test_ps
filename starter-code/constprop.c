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
void FreeConstList() 
{
    refConst* tmp;
    while (headNode != NULL) 
    {
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
void UpdateConstList(char* name, long val) 
{
    refConst* node = malloc(sizeof(refConst));
    if (node == NULL) 
    {
        return;
    }
    node->name = name;
    node->val = val;
    node->next = NULL;
    if (headNode == NULL) 
    {
        lastNode = node;
        headNode = node;
    } 
    else 
    {
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
Node* PropagateExpr(Node* node) 
{
    if (node == NULL) 
    {
        return NULL;
    }

    if (node->exprCode == VARIABLE) 
    {
        refConst* constNode = LookupConstList(node->name);
        if (constNode != NULL) 
        {
            Node* newConstNode = CreateNumber(constNode->val);
            free(node->name);
            free(node);
            madeChange = true;
            return newConstNode;
        }
    } 
    else if (node->exprCode == OPERATION) 
    {
        if (node->opCode == FUNCTIONCALL) 
        {
            NodeList* args = node->arguments;
            while (args != NULL) 
            {
                args->node = PropagateExpr(args->node);
                args = args->next;
            }
        } 
        else 
        {
            node->left = PropagateExpr(node->left);
            node->right = PropagateExpr(node->right);
        }
    }
    return node;
}

/*
************************************************************************************
  THIS FUNCTION IS MEANT TO UPDATE THE CONSTANT LIST WITH THE ASSOCIATED VARIABLE
  AND CONSTANT VALUE WHEN ONE IS SEEN. IT SHOULD ALSO PROPAGATE THE CONSTANTS WHEN 
  APPLICABLE.
*************************************************************************************
*/
void TrackConst(NodeList* statements) 
{
    Node* current_node;
    while (statements != NULL) 
    {
        current_node = statements->node;
        if (current_node->stmtCode == ASSIGN) 
        {
            Node* right_side = current_node->right;
            current_node->right = PropagateExpr(right_side);

            if (current_node->right->exprCode == CONSTANT) 
            {
                UpdateConstList(current_node->name, current_node->right->value);
            } 
            else 
            {
                refConst* existing = LookupConstList(current_node->name);
                if (existing != NULL) 
                {
                    refConst* prev = NULL;
                    refConst* curr = headNode;
                    while (curr != NULL) 
                    {
                        if (!strcmp(curr->name, current_node->name)) 
                        {
                            if (prev == NULL) 
                            {
                                headNode = curr->next;
                            } 
                            else 
                            {
                                prev->next = curr->next;
                            }
                            if (curr == lastNode) 
                            {
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
        } 
        else if (current_node->stmtCode == RETURN)
        {
            current_node->left = PropagateExpr(current_node->left);
        }
        statements = statements->next;
    }
}

/*
************************************************************************************
  TOP-LEVEL FUNCTION TO PERFORM CONSTANT PROPAGATION ON ALL FUNCTIONS
*************************************************************************************
*/
bool ConstProp(NodeList* worklist) 
{
    madeChange = false;
    while (worklist != NULL) 
    {
        FreeConstList();
        headNode = NULL;
        lastNode = NULL;
        Node* current_node = worklist->node;
        TrackConst(current_node->statements);
        worklist = worklist->next;
    }

    FreeConstList();
    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/