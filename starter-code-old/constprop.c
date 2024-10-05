/*
********************************************************************************
  CONSTPROP.C : IMPLEMENT THE DOWNSTREAM CONSTANT PROPAGATION OPTIMIZATION HERE
*********************************************************************************
*/

#include "constprop.h"

/* Initialize global variables */
refConst *lastNode = NULL, *headNode = NULL;
bool madeChange = false;

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
       free(tmp->name); // Free the duplicated name string
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
    if (node == NULL) {
        perror("Failed to allocate memory for refConst");
        exit(EXIT_FAILURE);
    }
    node->name = strdup(name); // Duplicate the string to avoid referencing freed memory
    if(node->name == NULL) {
        perror("Failed to duplicate variable name");
        exit(EXIT_FAILURE);
    }
    node->val = val;
    node->next = NULL;
    if(headNode == NULL) {
        lastNode = node;
        headNode = node;
    }
    else {
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
    refConst *node;
    node = headNode; 
    while(node!=NULL){
        if(!strcmp(name, node->name))
            return node;
        node = node->next;
    }
    return NULL;
}

/*
************************************************************************************
  FUNCTION TO REMOVE A VARIABLE FROM THE CONSTANT LIST
*************************************************************************************
*/
void RemoveConst(char* name) {
    refConst* prev = NULL;
    refConst* current = headNode;

    while(current != NULL) {
        if(strcmp(current->name, name) == 0) {
            if(prev == NULL) {
                headNode = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current->name); // Free the duplicated name string
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

/*
************************************************************************************
  THIS FUNCTION IS MEANT TO TRACK VARIABLES USED IN EXPRESSIONS AND PROPAGATE CONSTANTS
*************************************************************************************
*/
void TrackConst(NodeList* statements) {
  Node* stmt;
  while(statements != NULL) {
      stmt = statements->node;
      
      if(stmt->stmtCode == ASSIGN) {
          char* varName = stmt->name;
          Node* expr = stmt->right;

          // Case 1: RHS is a CONSTANT
          if(expr->exprCode == CONSTANT) {
              // Update the constant list with this variable
              UpdateConstList(varName, expr->value);
              madeChange = true;
          }
          // Case 2: RHS is a VARIABLE
          else if(expr->exprCode == VARIABLE) {
              // Check if the variable on RHS has a constant value
              refConst* constNode = LookupConstList(expr->name);
              if(constNode != NULL) {
                  // Replace the variable with its constant value
                  Node* newConst = CreateNumber(constNode->val);
                  if(newConst == NULL) {
                      perror("Failed to create constant node");
                      exit(EXIT_FAILURE);
                  }
                  // Free the old expression node
                  FreeExpression(expr);
                  // Assign the new constant node
                  stmt->right = newConst;
                  // Update the constant list with the new assignment
                  UpdateConstList(varName, constNode->val);
                  // Remove the variable from the constant list if it's being reassigned
                  RemoveConst(varName);
                  madeChange = true;
              }
              else {
                  // RHS variable does not have a known constant; remove varName from const list if it exists
                  refConst* existing = LookupConstList(varName);
                  if(existing != NULL) {
                      RemoveConst(varName);
                      madeChange = true;
                  }
              }
          }
          // Case 3: RHS is an OPERATION
          else if(expr->exprCode == OPERATION) {
              bool operationChanged = false;

              // Traverse and replace variables with constants in the left operand
              if(expr->left != NULL && expr->left->exprCode == VARIABLE) {
                  refConst* constNode = LookupConstList(expr->left->name);
                  if(constNode != NULL) {
                      // Replace variable with constant
                      Node* newConst = CreateNumber(constNode->val);
                      if(newConst == NULL) {
                          perror("Failed to create constant node");
                          exit(EXIT_FAILURE);
                      }
                      FreeExpression(expr->left);
                      expr->left = newConst;
                      operationChanged = true;
                      madeChange = true;
                  }
              }

              // Traverse and replace variables with constants in the right operand
              if(expr->right != NULL && expr->right->exprCode == VARIABLE) {
                  refConst* constNode = LookupConstList(expr->right->name);
                  if(constNode != NULL) {
                      // Replace variable with constant
                      Node* newConst = CreateNumber(constNode->val);
                      if(newConst == NULL) {
                          perror("Failed to create constant node");
                          exit(EXIT_FAILURE);
                      }
                      FreeExpression(expr->right);
                      expr->right = newConst;
                      operationChanged = true;
                      madeChange = true;
                  }
              }

              // If both operands are constants, evaluate the operation
              if(expr->left != NULL && expr->left->exprCode == CONSTANT &&
                  expr->right != NULL && expr->right->exprCode == CONSTANT) {
                  
                  long leftVal = expr->left->value;
                  long rightVal = expr->right->value;
                  long result;

                  // Perform the operation based on opCode
                  switch(expr->opCode) {
                      case ADD:
                          result = leftVal + rightVal;
                          break;
                      case SUBTRACT:
                          result = leftVal - rightVal;
                          break;
                      case MULTIPLY:
                          result = leftVal * rightVal;
                          break;
                      case DIVIDE:
                          // Handle division by zero
                          if(rightVal == 0) {
                              // Cannot optimize division by zero; skip evaluation
                              break;
                          }
                          result = leftVal / rightVal;
                          break;
                      case BOR:
                          result = leftVal | rightVal;
                          break;
                      case BAND:
                          result = leftVal & rightVal;
                          break;
                      case BXOR:
                          result = leftVal ^ rightVal;
                          break;
                      case BSHR:
                          result = leftVal >> rightVal;
                          break;
                      case BSHL:
                          result = leftVal << rightVal;
                          break;
                      case NEGATE:
                          result = -leftVal;
                          break;
                      default:
                          // Unsupported operation; skip evaluation
                          break;
                  }

                  // Replace the operation node with a constant node if operation was handled
                  if(expr->opCode != O_NONE && 
                      ((expr->opCode >= ADD && expr->opCode <= BSHL) || expr->opCode == NEGATE)) {
                      Node* newConst = CreateNumber(result);
                      if(newConst == NULL) {
                          perror("Failed to create constant node");
                          exit(EXIT_FAILURE);
                      }
                      FreeExpression(expr);
                      stmt->right = newConst;
                      UpdateConstList(varName, result);
                      RemoveConst(varName);
                      madeChange = true;
                  }
              }
          }
          // Case 4: RHS is neither CONSTANT, VARIABLE, nor OPERATION
          else {
              // Remove varName from the constant list if it exists
              refConst* existing = LookupConstList(varName);
              if(existing != NULL) {
                  RemoveConst(varName);
                  madeChange = true;
              }
          }
      }
      else if(stmt->stmtCode == RETURN) {
          Node* expr = stmt->left;

          // If the return expression is a VARIABLE, check for its constant value
          if(expr->exprCode == VARIABLE) {
              refConst* constNode = LookupConstList(expr->name);
              if(constNode != NULL) {
                  // Replace variable with constant
                  Node* newConst = CreateNumber(constNode->val);
                  if(newConst == NULL) {
                      perror("Failed to create constant node");
                      exit(EXIT_FAILURE);
                  }
                  FreeExpression(expr);
                  stmt->left = newConst;
                  madeChange = true;
              }
          }
          // If the return expression is an OPERATION, attempt to simplify
          else if(expr->exprCode == OPERATION) {
              bool operationChanged = false;

              // Traverse and replace variables with constants in the left operand
              if(expr->left != NULL && expr->left->exprCode == VARIABLE) {
                  refConst* constNode = LookupConstList(expr->left->name);
                  if(constNode != NULL) {
                      // Replace variable with constant
                      Node* newConst = CreateNumber(constNode->val);
                      if(newConst == NULL) {
                          perror("Failed to create constant node");
                          exit(EXIT_FAILURE);
                      }
                      FreeExpression(expr->left);
                      expr->left = newConst;
                      operationChanged = true;
                      madeChange = true;
                  }
              }

              // Traverse and replace variables with constants in the right operand
              if(expr->right != NULL && expr->right->exprCode == VARIABLE) {
                  refConst* constNode = LookupConstList(expr->right->name);
                  if(constNode != NULL) {
                      // Replace variable with constant
                      Node* newConst = CreateNumber(constNode->val);
                      if(newConst == NULL) {
                          perror("Failed to create constant node");
                          exit(EXIT_FAILURE);
                      }
                      FreeExpression(expr->right);
                      expr->right = newConst;
                      operationChanged = true;
                      madeChange = true;
                  }
              }

              // If both operands are constants, evaluate the operation
              if(expr->left != NULL && expr->left->exprCode == CONSTANT &&
                  expr->right != NULL && expr->right->exprCode == CONSTANT) {
                  
                  long leftVal = expr->left->value;
                  long rightVal = expr->right->value;
                  long result;

                  // Perform the operation based on opCode
                  switch(expr->opCode) {
                      case ADD:
                          result = leftVal + rightVal;
                          break;
                      case SUBTRACT:
                          result = leftVal - rightVal;
                          break;
                      case MULTIPLY:
                          result = leftVal * rightVal;
                          break;
                      case DIVIDE:
                          // Handle division by zero
                          if(rightVal == 0) {
                              // Cannot optimize division by zero; skip evaluation
                              break;
                          }
                          result = leftVal / rightVal;
                          break;
                      case BOR:
                          result = leftVal | rightVal;
                          break;
                      case BAND:
                          result = leftVal & rightVal;
                          break;
                      case BXOR:
                          result = leftVal ^ rightVal;
                          break;
                      case BSHR:
                          result = leftVal >> rightVal;
                          break;
                      case BSHL:
                          result = leftVal << rightVal;
                          break;
                      case NEGATE:
                          result = -leftVal;
                          break;
                      default:
                          // Unsupported operation; skip evaluation
                          break;
                  }

                  // Replace the operation node with a constant node if operation was handled
                  if(expr->opCode != O_NONE && 
                      ((expr->opCode >= ADD && expr->opCode <= BSHL) || expr->opCode == NEGATE)) {
                      Node* newConst = CreateNumber(result);
                      if(newConst == NULL) {
                          perror("Failed to create constant node");
                          exit(EXIT_FAILURE);
                      }
                      FreeExpression(expr);
                      stmt->left = newConst;
                      madeChange = true;
                  }
              }
          }
      }

      statements = statements->next;
  }
}

/*
************************************************************************************
  THIS FUNCTION IS MEANT TO PERFORM CONSTANT PROPAGATION ON ALL FUNCTIONS
*************************************************************************************
*/
bool ConstProp(NodeList* worklist) {
    madeChange = false;
    
    NodeList* current = worklist;
    while(current != NULL) {
        Node* funcNode = current->node;
        
        // Track and propagate constants within the function
        TrackConst(funcNode->statements);
        
        current = current->next;
    }
    
    return madeChange;
}
