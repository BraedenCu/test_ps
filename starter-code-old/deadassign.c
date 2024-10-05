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
    if(node==NULL) {
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
      if(node->right != NULL && node->right->exprCode == VARIABLE) {
          if(!VarExists(node->right->name)) {
              UpdateRefVarList(node->right->name);
          }
      }
      if(node->left != NULL && node->left->exprCode == VARIABLE) {
          if(!VarExists(node->left->name)) {
              UpdateRefVarList(node->left->name);
          }
      }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/

void TrackExpr(Node* node) {
    if(node == NULL) return;

    if(node->exprCode == VARIABLE) {
        UpdateRefVarList(node->name);
    }
    else if(node->exprCode == OPERATION) {
        if(node->left != NULL) {
            TrackExpr(node->left);
        }
        if(node->right != NULL) {
            TrackExpr(node->right);
        }
    }
}

void RemoveVar(char* name) {
    refVar* prev = NULL;
    refVar* current = head;
    while(current != NULL) {
        if(strcmp(current->name, name) == 0) {
            if(prev == NULL) {
                head = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}


/*
********************************************************************
  THIS FUNCTION IS MEANT TO TRACK THE REFERENCES OF EACH VARIABLE
  TO HELP DETERMINE IF IT WAS USED OR NOT LATER
********************************************************************
*/

void TrackRef(Node* funcNode) {
    init(); // Initialize the reference variable list

    // Calculate the number of statements
    int stmtCount = NodeListLen(funcNode->statements);
    if(stmtCount == 0) return;

    // Allocate an array to hold all statements for reverse traversal
    Node** stmtArray = malloc(stmtCount * sizeof(Node*));
    if(stmtArray == NULL) {
        perror("Failed to allocate memory for statement array");
        exit(EXIT_FAILURE);
    }

    // Populate the array with statements
    int index = 0;
    NodeList* current = funcNode->statements;
    while(current != NULL) {
        stmtArray[index++] = current->node;
        current = current->next;
    }

    // Traverse statements from end to start
    for(int i = stmtCount -1; i >=0; i--) {
        Node* stmt = stmtArray[i];
        if(stmt->stmtCode == RETURN) {
            // The variable being returned is referenced
            Node* expr = stmt->left;
            if(expr->exprCode == VARIABLE) {
                UpdateRefVarList(expr->name);
            }
            else if(expr->exprCode == OPERATION) {
                // Traverse the expression to find all variables used
                TrackExpr(expr);
            }
        }
        else if(stmt->stmtCode == ASSIGN) {
            char* varName = stmt->name;
            if(VarExists(varName)) {
                // The assigned variable is referenced; add variables used in RHS
                Node* expr = stmt->right;
                if(expr->exprCode == VARIABLE) {
                    UpdateRefVarList(expr->name);
                }
                else if(expr->exprCode == OPERATION) {
                    TrackExpr(expr);
                }
                // Remove the variable from reference list as it's now defined here
                RemoveVar(varName);
            }
            // Else: The assignment is dead; handled in RemoveDead
        }
    }

    // Free the statement array
    free(stmtArray);
}

/*
***************************************************************
  THIS FUNCTION IS MEANT TO DO THE ACTUAL DEADCODE REMOVAL
  BASED ON THE INFORMATION OF TRACKED REFERENCES
****************************************************************
*/

NodeList* RemoveDead(NodeList* statements) {
    refVar* varNode;
    NodeList *prev, *tmp, *first;
    
    // Initialize the new statement list
    first = NULL;
    NodeList* newLast = NULL; // Use a different name to avoid conflict with global 'last'

    NodeList* current = statements;
    while(current != NULL) {
        Node* stmt = current->node;
        bool isDead = false;

        if(stmt->stmtCode == ASSIGN) {
            char* varName = stmt->name;
            if(!VarExists(varName)) {
                // Assignment is dead; do not include it in the new list
                isDead = true;
                change = 1; // Indicate that a change has been made
            }
            else {
                // Variable is used; keep the assignment
                // Variables used in RHS have already been added to refVar in TrackRef
            }
        }

        if(!isDead) {
            // Include the statement in the new list
            NodeList* newNode = AllocateNodeList();
            if (newNode == NULL) {
                perror("Failed to allocate memory for new statement list");
                exit(EXIT_FAILURE);
            }
            newNode->node = stmt;
            newNode->next = NULL;

            if(first == NULL) {
                first = newNode;
                newLast = newNode;
            }
            else {
                newLast->next = newNode;
                newLast = newNode;
            }
        }

        current = current->next;
    }

    return first;
}

/*
********************************************************************
  THIS FUNCTION SHOULD ENSURE THAT THE DEAD CODE REMOVAL PROCESS
  OCCURS CORRECTLY FOR ALL THE FUNCTIONS IN THE PROGRAM
********************************************************************
*/

bool DeadAssign(NodeList* worklist) {
   bool madeChange = false;
   change = 0; // Reset change flag

   NodeList* current = worklist;
   while(current != NULL) {
        Node* funcNode = current->node;

        // Track variable references in the function
        TrackRef(funcNode);

        // Remove dead assignments based on tracked references
        NodeList* optimizedStatements = RemoveDead(funcNode->statements);

        // Update the function's statements with the optimized list
        funcNode->statements = optimizedStatements;

        if(change == 1) {
            madeChange = true;
            change = 0; // Reset for the next function
        }

        current = current->next;
    }

    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
 
