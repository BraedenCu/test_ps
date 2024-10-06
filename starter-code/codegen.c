/*
***********************************************************************
  CODEGEN.C : IMPLEMENT CODE GENERATION HERE
************************************************************************
*/
#include "codegen.h"

int argCounter;
int lastUsedOffset;
char lastOffsetUsed[100];
FILE *fptr;
regInfo *regList, *regHead, *regLast;
varStoreInfo *varList, *varHead, *varLast;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE THE ASSEMBLY FILE WITH FUNCTION DETAILS
************************************************************************
*/
void InitAsm(char* funcName) {
    fprintf(fptr, "\n.globl %s", funcName);
    fprintf(fptr, "\n%s:", funcName); 

    // Init stack and base ptr
    fprintf(fptr, "\npushq %%rbp");  
    fprintf(fptr, "\nmovq %%rsp, %%rbp"); 
}

/*
***************************************************************************
   FUNCTION TO WRITE THE RETURNING CODE OF A FUNCTION IN THE ASSEMBLY FILE
****************************************************************************
*/
void RetAsm() {
    fprintf(fptr,"\npopq  %%rbp");
    fprintf(fptr, "\nretq\n");
} 

/*
***************************************************************************
  FUNCTION TO CONVERT OFFSET FROM LONG TO CHAR STRING 
****************************************************************************
*/
void LongToCharOffset() {
     lastUsedOffset = lastUsedOffset - 8;
     snprintf(lastOffsetUsed, 100,"%d", lastUsedOffset);
     strcat(lastOffsetUsed,"(%rbp)");
}

/*
***************************************************************************
  FUNCTION TO CONVERT CONSTANT VALUE TO CHAR STRING
****************************************************************************
*/
void ProcessConstant(Node* opNode) {
     char value[10];
     LongToCharOffset();
     snprintf(value, 10,"%ld", opNode->value);
     char str[100];
     snprintf(str, 100,"%d", lastUsedOffset);
     strcat(str,"(%rbp)");
     AddVarInfo("", str, opNode->value, true);
     fprintf(fptr, "\nmovq  $%s, %s", value, str);
}

/*
***************************************************************************
  FUNCTION TO SAVE VALUE IN ACCUMULATOR (RAX)
****************************************************************************
*/
void SaveValInRax(char* name) {
    char *tempReg;
    tempReg = GetNextAvailReg(true);
    if(!(strcmp(tempReg, "NoReg"))) {
        LongToCharOffset();
        fprintf(fptr, "\n movq %%rax, %s", lastOffsetUsed);
        UpdateVarInfo(name, lastOffsetUsed, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
    else {
        fprintf(fptr, "\nmovq %%rax, %s", tempReg);
        UpdateRegInfo(tempReg, 0);
        UpdateVarInfo(name, tempReg, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
}



/*
***********************************************************************
  FUNCTION TO ADD VARIABLE INFORMATION TO THE VARIABLE INFO LIST
************************************************************************
*/
void AddVarInfo(char* varName, char* location, long val, bool isConst) {
   varStoreInfo* node = malloc(sizeof(varStoreInfo));
   node->varName = varName;
   node->value = val;
   strcpy(node->location,location);
   node->isConst = isConst;
   node->next = NULL;
   node->prev = varLast;
   if(varHead==NULL) {
       varHead = node;
       varLast = node;;
       varList = node;
   } else {
       //node->prev = varLast;
       varLast->next = node;
       varLast = varLast->next;
   }
   varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO FREE THE VARIABLE INFORMATION LIST
************************************************************************
*/
void FreeVarList()
{  
   varStoreInfo* tmp;
   while (varHead != NULL)
    {  
       tmp = varHead;
       varHead = varHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO LOOKUP VARIABLE INFORMATION FROM THE VARINFO LIST
************************************************************************
*/
char* LookUpVarInfo(char* name, long val) {
    varList = varLast;
    if(varList == NULL) printf("NULL varlist");
    while(varList!=NULL) {
        if(varList->isConst == true) {
            if(varList->value == val) return varList->location;
        }
        else {
            if(!strcmp(name,varList->varName)) return varList->location;
        }
        varList = varList->prev;
    }
    varList = varHead;
    return "";
}

/*
***********************************************************************
  FUNCTION TO UPDATE VARIABLE INFORMATION 
************************************************************************
*/
void UpdateVarInfo(char* varName, char* location, long val, bool isConst) {
  
   if(!(strcmp(LookUpVarInfo(varName, val), ""))) {
       AddVarInfo(varName, location, val, isConst);
   }
   else {
       varList = varHead;
       if(varList == NULL) printf("NULL varlist");
       while(varList!=NULL) {
           if(!strcmp(varList->varName,varName)) {
               varList->value = val;
               strcpy(varList->location,location);
               varList->isConst = isConst;
               break;
        }
        varList = varList->next;
       }
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE VARIABLE INFORMATION LIST
************************************************************************
*/
void PrintVarListInfo() {
    varList = varHead;
    if(varList == NULL) printf("NULL varlist");
    while(varList!=NULL) {
        if(!varList->isConst) {
            printf("\t %s : %s", varList->varName, varList->location);
        }
        else {
            printf("\t %ld : %s", varList->value, varList->location);
        }
        varList = varList->next;
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO ADD NEW REGISTER INFORMATION TO THE REGISTER INFO LIST
************************************************************************
*/
void AddRegInfo(char* name, int avail) {

   regInfo* node = malloc(sizeof(regInfo));
   node->regName = name;
   node->avail = avail;
   node->next = NULL; 

   if(regHead==NULL) {
       regHead = node;
       regList = node;
       regLast = node;
   } else {
       regLast->next = node;
       regLast = node;
   }
   regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO FREE REGISTER INFORMATION LIST
************************************************************************
*/
void FreeRegList()
{  
   regInfo* tmp;
   while (regHead != NULL)
    {  
       tmp = regHead;
       regHead = regHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO UPDATE THE AVAILIBILITY OF REGISTERS IN THE REG INFO LIST
************************************************************************
*/
void UpdateRegInfo(char* regName, int avail) {
    regList = regHead;
    while(regList!=NULL) {
        if(!strcmp(regName, regList->regName)) {
            regList->avail = avail;
            break;
        }
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO RETURN THE NEXT AVAILABLE REGISTER
************************************************************************
*/
char* GetNextAvailReg(bool noAcc) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(regList->avail == 1) {
            if(!noAcc) return regList->regName;
            // if not rax and dont return accumulator set to true, return the other reg
            // if rax and noAcc == true, skip to next avail
            if(noAcc && strcmp(regList->regName, "%rax")) { 
                return regList->regName;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return "NoReg";
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF ANY REGISTER APART FROM OR INCLUDING 
  THE ACCUMULATOR(RAX) IS AVAILABLE
************************************************************************
*/
int IfAvailReg(bool noAcc) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(regList->avail == 1) {
            // registers available
            if(!noAcc) return 1;
            if(noAcc && strcmp(regList->regName, "%rax")) {
                return 1;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return 0;
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF A SPECIFIC REGISTER IS AVAILABLE
************************************************************************
*/
bool IsAvailReg(char* name) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(!strcmp(regList->regName, name)) {
           if(regList->avail == 1) {
               return true;
           } 
        }
        regList = regList->next;
    }
    regList = regHead;
    return false;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE REGISTER INFORMATION
************************************************************************
*/
void PrintRegListInfo() {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        printf("\t %s : %d", regList->regName, regList->avail);
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO CREATE THE REGISTER LIST
************************************************************************
*/
void CreateRegList() {
    // Create the initial reglist which can be used to store variables.
    // Let's use %rbx, %r10, %r11, %r12, %r13, %r14, %r15 as general-purpose registers.
    AddRegInfo("%rbx", 1); // Make it available
    AddRegInfo("%r10", 1);
    AddRegInfo("%r11", 1);
    AddRegInfo("%r12", 1);
    AddRegInfo("%r13", 1);
    AddRegInfo("%r14", 1);
    AddRegInfo("%r15", 1);
}

/*
***********************************************************************
  THIS FUNCTION IS MEANT TO PUT THE FUNCTION ARGUMENTS ON STACK
************************************************************************
*/
int PutArgumentsFromStack(NodeList* arguments) {
    // Arguments are in %rdi, %rsi, %rdx, %rcx, %r8, %r9
    char* arg_registers[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int arg_index = 0;
    NodeList* arg_node = arguments;
    while (arg_node != NULL && arg_index < 6) {
        Node* param_node = arg_node->node; // Should be a PARAMETER node
        char* var_name = param_node->name;
        // Allocate space on stack for this variable
        LongToCharOffset(); // Updates lastUsedOffset and lastOffsetUsed
        // Now lastOffsetUsed contains the stack location, e.g., "-8(%rbp)"
        // Add variable info
        AddVarInfo(var_name, lastOffsetUsed, INVAL, false);
        // Move argument register into stack location
        fprintf(fptr, "\nmovq %s, %s", arg_registers[arg_index], lastOffsetUsed);
        arg_index++;
        arg_node = arg_node->next;
    }
    return arg_index; // Returning the number of arguments processed
}   

/*
*************************************************************************
  THIS FUNCTION IS MEANT TO GET THE FUNCTION ARGUMENTS FROM THE  STACK
**************************************************************************
*/
void PutArgumentsOnStack(NodeList* arguments) {
    // Arguments need to be moved into %rdi, %rsi, %rdx, %rcx, %r8, %r9
    char* arg_registers[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int arg_index = 0;
    NodeList* arg_node = arguments;
    while (arg_node != NULL && arg_index < 6) {
        Node* expr_node = arg_node->node; // Should be an expression node (variable or constant)
        // We need to get the value of expr_node into arg_registers[arg_index]
        if (expr_node->exprCode == VARIABLE) {
            char* var_name = expr_node->name;
            // Look up variable location
            char* var_loc = LookUpVarInfo(var_name, INVAL);
            if (strcmp(var_loc, "") == 0) {
                printf("Error: Variable %s not found\n", var_name);
                // Handle error
            }
            // Move variable into argument register
            fprintf(fptr, "\nmovq %s, %s", var_loc, arg_registers[arg_index]);
        } else if (expr_node->exprCode == CONSTANT) {
            long const_value = expr_node->value;
            // Move constant into argument register
            fprintf(fptr, "\nmovq $%ld, %s", const_value, arg_registers[arg_index]);
        } else {
            // Should not happen, as arguments are simple variables or constants
            printf("Error: Unsupported argument type\n");
        }
        arg_index++;
        arg_node = arg_node->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO PROCESS EACH CODE STATEMENT AND GENERATE 
  ASSEMBLY FOR IT. 
  TIP: YOU CAN MODULARIZE BETTER AND ADD NEW SMALLER FUNCTIONS IF YOU 
  WANT THAT CAN BE CALLED FROM HERE.
 ************************************************************************
 */  
void ProcessStatements(NodeList* statements) {   
    while(statements != NULL) {
        Node* stmt_node = statements->node;
        if (stmt_node->stmtCode == ASSIGN) {
            // Handle assignment
            char* var_name = stmt_node->name;
            Node* expr_node = stmt_node->right;
            // Evaluate expr_node and store result in a location
            char* result_loc = EvaluateExpression(expr_node);
            // Now, we need to store the result in a variable location
            // Allocate space on the stack for the variable
            LongToCharOffset();
            // lastOffsetUsed now has the stack location
            AddVarInfo(var_name, lastOffsetUsed, INVAL, false);
            // Move result from result_loc to variable location
            fprintf(fptr, "\nmovq %s, %s", result_loc, lastOffsetUsed);
            // Mark registers used in result_loc as available again
            if (result_loc[0] == '%') {
                UpdateRegInfo(result_loc, 1);
            }
        } else if (stmt_node->stmtCode == RETURN) {
            // Handle return
            Node* expr_node = stmt_node->left;
            // Evaluate expr_node and move result into %rax
            char* result_loc = EvaluateExpression(expr_node);
            // Move result into %rax
            fprintf(fptr, "\nmovq %s, %%rax", result_loc);
            // Mark registers used in result_loc as available again
            if (result_loc[0] == '%') {
                UpdateRegInfo(result_loc, 1);
            }
            // We don't call RetAsm() here; it will be called after all statements
        } else {
            printf("Error: Unknown statement type\n");
        }
        statements = statements->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO DO CODEGEN FOR ALL THE FUNCTIONS IN THE FILE
 ************************************************************************
*/
void Codegen(NodeList* worklist) {
    fptr = fopen("assembly.s", "w+");
    if(fptr == NULL) {
        printf("\n Could not create assembly file");
        return; 
    }
    NodeList* func_list = worklist;
    while(func_list != NULL) {
        Node* func_node = func_list->node; // Function declaration node
        // Initialize function
        InitAsm(func_node->name);
        // Reset lastUsedOffset
        lastUsedOffset = 0;
        // Initialize regList and varList
        regList = regHead = regLast = NULL;
        varList = varHead = varLast = NULL;
        CreateRegList();
        // Process function arguments
        PutArgumentsFromStack(func_node->arguments);
        // Process statements
        ProcessStatements(func_node->statements);
        // Function epilogue
        RetAsm();
        // Clean up
        FreeRegList();
        FreeVarList();
        func_list = func_list->next; 
    }
    fclose(fptr);
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILIARY FUNCTIONS BELOW THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/

char* EvaluateExpression(Node* expr_node) {
    if (expr_node->exprCode == VARIABLE) {
        char* var_name = expr_node->name;
        // Look up variable location
        char* var_loc = LookUpVarInfo(var_name, INVAL);
        if (strcmp(var_loc, "") == 0) {
            printf("Error: Variable %s not found\n", var_name);
            // Handle error
        }
        return var_loc;
    } else if (expr_node->exprCode == CONSTANT) {
        // Move constant into a register or stack location
        long const_value = expr_node->value;
        // Allocate a register or stack location
        char* reg = GetNextAvailReg(false);
        if (strcmp(reg, "NoReg") != 0) {
            // Use register
            fprintf(fptr, "\nmovq $%ld, %s", const_value, reg);
            UpdateRegInfo(reg, 0); // Mark register as used
            return reg;
        } else {
            // No register available, use stack
            LongToCharOffset();
            fprintf(fptr, "\nmovq $%ld, %s", const_value, lastOffsetUsed);
            return lastOffsetUsed;
        }
    } else if (expr_node->exprCode == OPERATION) {
        if (expr_node->opCode == FUNCTIONCALL) {
            // Handle function call
            Node* func_decl_node = expr_node->left;
            NodeList* arguments = expr_node->arguments;
            // Evaluate arguments and move them into argument registers
            PutArgumentsOnStack(arguments);
            // Call function
            fprintf(fptr, "\ncall %s", func_decl_node->name);
            // Return value is in %rax
            // We need to move it to a register or stack location
            char* reg = GetNextAvailReg(true); // Exclude %rax
            if (strcmp(reg, "NoReg") != 0) {
                fprintf(fptr, "\nmovq %%rax, %s", reg);
                UpdateRegInfo(reg, 0);
                return reg;
            } else {
                // No register available, use stack
                LongToCharOffset();
                fprintf(fptr, "\nmovq %%rax, %s", lastOffsetUsed);
                return lastOffsetUsed;
            }
        } else if (expr_node->opCode == NEGATE) {
            // Unary operation
            Node* operand_node = expr_node->left;
            char* operand_loc = EvaluateExpression(operand_node);
            char* reg = GetNextAvailReg(false);
            if (strcmp(reg, "NoReg") != 0) {
                fprintf(fptr, "\nmovq %s, %s", operand_loc, reg);
                fprintf(fptr, "\nnegq %s", reg);
                UpdateRegInfo(reg, 0);
                return reg;
            } else {
                // No register available, use %rax
                fprintf(fptr, "\nmovq %s, %%rax", operand_loc);
                fprintf(fptr, "\nnegq %%rax");
                // Save result somewhere
                LongToCharOffset();
                fprintf(fptr, "\nmovq %%rax, %s", lastOffsetUsed);
                return lastOffsetUsed;
            }
        } else {
            // Binary operation
            Node* left_node = expr_node->left;
            Node* right_node = expr_node->right;
            char* left_loc = EvaluateExpression(left_node);
            char* right_loc = EvaluateExpression(right_node);
            // We need to perform operation left_node op right_node
            char* reg = GetNextAvailReg(false);
            if (strcmp(reg, "NoReg") != 0) {
                // Use reg for result
                // Move left operand into reg
                fprintf(fptr, "\nmovq %s, %s", left_loc, reg);
                // Perform operation with right operand
                switch (expr_node->opCode) {
                    case ADD:
                        fprintf(fptr, "\naddq %s, %s", right_loc, reg);
                        break;
                    case SUBTRACT:
                        fprintf(fptr, "\nsubq %s, %s", right_loc, reg);
                        break;
                    case MULTIPLY:
                        fprintf(fptr, "\nimulq %s, %s", right_loc, reg);
                        break;
                        
                        /*
                    case DIVIDE:
                        // For division, we need to use %rax and %rdx
                        //fprintf(fptr, "\npushq %%rdx");
                        //fprintf(fptr, "\npushq %%rax");
                        fprintf(fptr, "\nmovq %s, %%rax", left_loc);
                        //fprintf(fptr, "\ncqto"); // Sign extend %rax into %rdx:%rax // FIX FIX
                        fprintf(fptr, "\ncqo"); // Correct instruction
                        fprintf(fptr, "\nidivq %s", right_loc);
                        fprintf(fptr, "\nmovq %%rax, %s", reg);
                        //fprintf(fptr, "\npopq %%rax");
                        //fprintf(fptr, "\npopq %%rdx");
                        break;
                        */
                    case DIVIDE:
                        fprintf(fptr, "\nmovq %s, %%rax", left_loc);
                        fprintf(fptr, "\ncqo");
                        fprintf(fptr, "\nidivq %s", right_loc);
                        fprintf(fptr, "\nmovq %%rax, %s", reg);
                        UpdateRegInfo(reg, 0);
                        break;
                    case BAND:
                        fprintf(fptr, "\nandq %s, %s", right_loc, reg);
                        break;
                    case BOR:
                        fprintf(fptr, "\norq %s, %s", right_loc, reg);
                        break;
                    case BXOR:
                        fprintf(fptr, "\nxorq %s, %s", right_loc, reg);
                        break;
                    case BSHL:
                        // For shift left, shift count must be in %cl
                        fprintf(fptr, "\nmovq %s, %%rcx", right_loc);
                        fprintf(fptr, "\nshlq %%cl, %s", reg);
                        break;
                    case BSHR:
                        // For shift right, shift count must be in %cl
                        fprintf(fptr, "\nmovq %s, %%rcx", right_loc);
                        fprintf(fptr, "\nshrq %%cl, %s", reg);
                        break;
                    default:
                        printf("Error: Unsupported binary operator\n");
                        break;
                }
                UpdateRegInfo(reg, 0);
                return reg;
            } else {
                // No register available, use %rax
                fprintf(fptr, "\nmovq %s, %%rax", left_loc);
                switch (expr_node->opCode) {
                    case ADD:
                        fprintf(fptr, "\naddq %s, %%rax", right_loc);
                        break;
                    case SUBTRACT:
                        fprintf(fptr, "\nsubq %s, %%rax", right_loc);
                        break;
                    case MULTIPLY:
                        fprintf(fptr, "\nimulq %s, %%rax", right_loc);
                        break;
                    case DIVIDE:
                        fprintf(fptr, "\npushq %%rdx");
                        fprintf(fptr, "\ncqto"); // Sign extend %rax into %rdx:%rax
                        fprintf(fptr, "\nidivq %s", right_loc);
                        fprintf(fptr, "\npopq %%rdx");
                        break;
                    case BAND:
                        fprintf(fptr, "\nandq %s, %%rax", right_loc);
                        break;
                    case BOR:
                        fprintf(fptr, "\norq %s, %%rax", right_loc);
                        break;
                    case BXOR:
                        fprintf(fptr, "\nxorq %s, %%rax", right_loc);
                        break;
                    case BSHL:
                        fprintf(fptr, "\nmovq %s, %%rcx", right_loc);
                        fprintf(fptr, "\nshlq %%cl, %%rax");
                        break;
                    case BSHR:
                        fprintf(fptr, "\nmovq %s, %%rcx", right_loc);
                        fprintf(fptr, "\nshrq %%cl, %%rax");
                        break;
                    default:
                        printf("Error: Unsupported binary operator\n");
                        break;
                }
                // Move result to stack location
                LongToCharOffset();
                fprintf(fptr, "\nmovq %%rax, %s", lastOffsetUsed);
                return lastOffsetUsed;
            }
        }
    } else {
        printf("Error: Unsupported expression type\n");
        return "";
    }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILIARY FUNCTIONS ABOVE THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/