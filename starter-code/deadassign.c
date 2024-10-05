/*
***********************************************************************
  DEADASSIGN.C : IMPLEMENT THE DEAD CODE ELIMINATION OPTIMIZATION HERE
************************************************************************
*/

#include "deadassign.h"

int change;
refVar *last, *head;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE HEAD AND LAST POINTERS FOR THE REFERENCED 
  VARIABLE LIST.
************************************************************************
*/

void init()
{ 
    head = NULL;
    last = head;
}

/*
***********************************************************************
  FUNCTION TO FREE THE REFERENCED VARIABLE LIST
************************************************************************
*/

void FreeList()
{
   refVar* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }
}

/*
***********************************************************************
  FUNCTION TO IDENTIFY IF A VARIABLE'S REFERENCE IS ALREADY TRACKED
************************************************************************
*/
bool VarExists(char* name) {
   refVar *node;
   node = head;
   while(node != NULL) {
       if(!strcmp(name, node->name)) {
           return true;
       }
       node = node->next;
    }
    return false;
}

/*
***********************************************************************
  FUNCTION TO ADD A REFERENCE TO THE REFERENCE LIST
************************************************************************
*/
void UpdateRefVarList(char* name) {
    refVar* node = malloc(sizeof(refVar));
    if (node == NULL) return;
    node->name = name;
    node->next = NULL;
    if(head == NULL) {
        last = node;
        head = node;
    }
    else {
        last->next = node;
        last = node;
    }
}

/*
****************************************************************************
  FUNCTION TO PRINT OUT THE LIST TO SEE ALL VARIABLES THAT ARE USED/REFERRED
  AFTER THEIR ASSIGNMENT. YOU CAN USE THIS FOR DEBUGGING PURPOSES OR TO CHECK
  IF YOUR LIST IS GETTING UPDATED CORRECTLY
******************************************************************************
*/
void PrintRefVarList() {
    refVar *node;
    node = head;
    if(node == NULL) {
        printf("\nList is empty"); 
        return;
    }
    while(node != NULL) {
        printf("\t %s", node->name);
        node = node->next;
    }
}

/*
***********************************************************************
  FUNCTION TO UPDATE THE REFERENCE LIST WHEN A VARIABLE IS REFERENCED 
  IF NOT DONE SO ALREADY.
************************************************************************
*/
void UpdateRef(Node* node) {
    if(node == NULL) return;

    if(node->exprCode == VARIABLE) {
        if(!VarExists(node->name)) {
            UpdateRefVarList(node->name);
        }
    } else if(node->exprCode == OPERATION) {
        if(node->opCode == FUNCTIONCALL) {
            NodeList* args = node->arguments;
            while(args != NULL) {
                UpdateRef(args->node);
                args = args->next;
            }
        } else {
            UpdateRef(node->left);
            UpdateRef(node->right);
        }
    }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/

/*
********************************************************************
  THIS FUNCTION IS MEANT TO TRACK THE REFERENCES OF EACH VARIABLE
  TO HELP DETERMINE IF IT WAS USED OR NOT LATER
********************************************************************
*/

void TrackExpr(Node* node) {
    if(node == NULL) return;

    if(node->exprCode == VARIABLE) {
        if(!VarExists(node->name)) {
            UpdateRefVarList(node->name);
        }
    } else if(node->exprCode == OPERATION) {
        if(node->opCode == FUNCTIONCALL) {
            NodeList* args = node->arguments;
            while(args != NULL) {
                TrackExpr(args->node);
                args = args->next;
            }
        } else {
            TrackExpr(node->left);
            TrackExpr(node->right);
        }
    }
    // No action needed for CONSTANT and PARAMETER nodes
}

void TrackRef(Node* funcNode) {
     NodeList* statements = funcNode->statements;
     Node *node;
     while(statements != NULL) {
        node = statements->node;
        if(node->stmtCode == ASSIGN) {
            // For assignment statements, analyze the right-hand side expression
            TrackExpr(node->right);
        } else if(node->stmtCode == RETURN) {
            // For return statements, analyze the expression being returned
            TrackExpr(node->left);
        }
        statements = statements->next;
     }
}

/*
***************************************************************
  THIS FUNCTION IS MEANT TO DO THE ACTUAL DEAD CODE REMOVAL
  BASED ON THE INFORMATION OF TRACKED REFERENCES
****************************************************************
*/
NodeList* RemoveDead(NodeList* statements) {
    NodeList* newStatements = NULL;
    NodeList** lastPtr = &newStatements;
    NodeList* curr = statements;

    while(curr != NULL) {
        Node* node = curr->node;
        if(node->stmtCode == ASSIGN) {
            if(!VarExists(node->name)) {
                // Variable is not used; remove the assignment
                change = 1;
                NodeList* tmp = curr;
                curr = curr->next;
                FreeStatement(tmp->node);
                free(tmp);
                // Do not advance lastPtr
            } else {
                // Variable is used; keep the assignment
                *lastPtr = curr;
                lastPtr = &(curr->next);
                curr = curr->next;
            }
        } else {
            // For other statements, keep them
            *lastPtr = curr;
            lastPtr = &(curr->next);
            curr = curr->next;
        }
    }
    *lastPtr = NULL; // Terminate the new list
    return newStatements;
}

/*
********************************************************************
  THIS FUNCTION SHOULD ENSURE THAT THE DEAD CODE REMOVAL PROCESS
  OCCURS CORRECTLY FOR ALL THE FUNCTIONS IN THE PROGRAM
********************************************************************
*/
bool DeadAssign(NodeList* worklist) {
   bool madeChange = false;
   while(worklist != NULL) {
        change = 0;
        Node* funcNode = worklist->node;
        init(); // Initialize the reference list
        TrackRef(funcNode);
        funcNode->statements = RemoveDead(funcNode->statements);
        FreeList();
        if(change == 1) {
            madeChange = true;
        }
        worklist = worklist->next;
    }
    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/