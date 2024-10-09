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
void LongToCharOffset() 
{
     lastUsedOffset = lastUsedOffset - 8;
     snprintf(lastOffsetUsed, 100,"%d", lastUsedOffset);
     strcat(lastOffsetUsed,"(%rbp)");
}

/*
***************************************************************************
  FUNCTION TO CONVERT CONSTANT VALUE TO CHAR STRING
****************************************************************************
*/
void ProcessConstant(Node* opNode) 
{
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
void SaveValInRax(char* name) 
{
    char *tempReg;
    tempReg = GetNextAvailReg(true);
    if(!(strcmp(tempReg, "NoReg"))) 
    {
        LongToCharOffset();
        fprintf(fptr, "\n movq %%rax, %s", lastOffsetUsed);
        UpdateVarInfo(name, lastOffsetUsed, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
    else 
    {
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
void AddVarInfo(char* varName, char* location, long val, bool isConst) 
{
   varStoreInfo* node = malloc(sizeof(varStoreInfo));
   node->varName = varName;
   node->value = val;
   strcpy(node->location,location);
   node->isConst = isConst;
   node->next = NULL;
   node->prev = varLast;
   if(varHead==NULL) 
   {
       varHead = node;
       varLast = node;;
       varList = node;
   }
   else 
   {
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
char* LookUpVarInfo(char* name, long val) 
{
    varList = varLast;
    if(varList == NULL) printf("NULL varlist");
    while(varList!=NULL) 
    {
        if(varList->isConst == true) 
        {
            if(varList->value == val) return varList->location;
        }
        else 
        {
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
void UpdateVarInfo(char* varName, char* location, long val, bool isConst) 
{
  
   if(!(strcmp(LookUpVarInfo(varName, val), ""))) 
   {
       AddVarInfo(varName, location, val, isConst);
   }
   else 
   {
       varList = varHead;
       if(varList == NULL) 
        {
            printf("NULL varlist");
        }
       while(varList!=NULL) 
       {
           if(!strcmp(varList->varName,varName)) 
           {
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
void PrintVarListInfo() 
{
    varList = varHead;
    if(varList == NULL) printf("NULL varlist");
    while(varList!=NULL) 
    {
        if(!varList->isConst) 
        {
            printf("\t %s : %s", varList->varName, varList->location);
        }
        else 
        {
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
void AddRegInfo(char* name, int avail) 
{
   regInfo* node = malloc(sizeof(regInfo));
   node->regName = name;
   node->avail = avail;
   node->next = NULL; 

   if(regHead==NULL) 
   {
       regHead = node;
       regList = node;
       regLast = node;
   } 
   else 
   {
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
void UpdateRegInfo(char* regName, int avail) 
{
    regList = regHead;
    while(regList!=NULL) 
    {
        if(!strcmp(regName, regList->regName)) 
        {
            regList->avail = avail;
            break;
        }
        regList = regList->next;
    }
    regList = regHead;
}

// added this function
int CountVariables(NodeList* statements) 
{
    int count = 0;
    while (statements != NULL) 
    {
        Node* stmt = statements->node;
        if (stmt->stmtCode == ASSIGN) 
        {
            count++;
        }
        statements = statements->next;
    }
    return count;
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
bool IsAvailReg(char* name) 
{
    regList = regHead;
    if(regList == NULL) 
    {
        printf("NULL reglist");
    }
    while(regList!=NULL) 
    {
        if(!strcmp(regList->regName, name)) 
        {
           if(regList->avail == 1) 
           {
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
void PrintRegListInfo() 
{
    regList = regHead;
    if(regList == NULL)
    {
        printf("NULL reglist");
    }
    while(regList!=NULL) 
    {
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

    // Create the initial reglist which can be used to store variables.
    // 4 general purpose registers : AX, BX, CX, DX
    // 4 special purpose : SP, BP, SI , DI. 
    // Other registers: r8, r9
    // You need to decide which registers you will add in the register list 
    // use. Can you use all of the above registers?

// MODIFIED
void CreateRegList() 
{
    AddRegInfo("%rbx", 1);
    AddRegInfo("%r10", 1);
    AddRegInfo("%r11", 1);
    AddRegInfo("%r12", 1);
    AddRegInfo("%r13", 1);
    AddRegInfo("%r14", 1);
    AddRegInfo("%r15", 1);
    AddRegInfo("%rax", 1);  
    AddRegInfo("%rdx", 1);  
    AddRegInfo("%rcx", 1);  
}

/*
***********************************************************************
  THIS FUNCTION IS MEANT TO PUT THE FUNCTION ARGUMENTS ON STACK
************************************************************************
*/
// MODIFIED
int PutArgumentsFromStack(NodeList* arguments) 
{
    char* arg_registers[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int argument_index = 0;
    NodeList* argument_nodes = arguments;
    while (argument_nodes != NULL && argument_index < 6) 
    {
        Node* parameter_nodes = argument_nodes->node; 
        char* paramer_name_property = parameter_nodes->name;
        LongToCharOffset(); 
        AddVarInfo(paramer_name_property, lastOffsetUsed, INVAL, false);
        fprintf(fptr, "\nmovq %s, %s", arg_registers[argument_index], lastOffsetUsed);

        argument_index = argument_index + 1;
        argument_nodes = argument_nodes->next;
    }

    return argument_index;
}   

/*
*************************************************************************
  THIS FUNCTION IS MEANT TO GET THE FUNCTION ARGUMENTS FROM THE  STACK
**************************************************************************
*/
// MODIFIED
void PutArgumentsOnStack(NodeList* arguments) 
{
    char* arg_registers[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int argument_index = 0;
    NodeList* argument_nodes = arguments;
    
    while (argument_nodes != NULL && argument_index < 6) 
    {
        Node* expr = argument_nodes->node; 
        if (expr->exprCode == VARIABLE) 
        {
            char* var_name = expr->name;
            char* var_loc = LookUpVarInfo(var_name, INVAL);
            if (strcmp(var_loc, "") == 0) 
            {
                printf("Variable %s not found\n", var_name);
            }
            fprintf(fptr, "\nmovq %s, %s", var_loc, arg_registers[argument_index]);
        } 
        else if (expr->exprCode == CONSTANT) 
        {
            long const_value = expr->value;
            fprintf(fptr, "\nmovq $%ld, %s", const_value, arg_registers[argument_index]);
        } 
        else 
        {
            printf("Unsupported argument type\n");
        }

        argument_index += 1;
        argument_nodes = argument_nodes->next;
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
// MODIFIED
void ProcessStatements(NodeList* statements) 
{   
    while(statements != NULL) 
    {
        Node* stmt_node = statements->node;
        if (stmt_node->stmtCode == ASSIGN) 
        {
            char* var_name = stmt_node->name;
            Node* expr = stmt_node->right;
            char* result_loc = ComputeExpr(expr);

            LongToCharOffset();
            AddVarInfo(var_name, lastOffsetUsed, INVAL, false);
            fprintf(fptr, "\nmovq %s, %s", result_loc, lastOffsetUsed);
            if (result_loc[0] == '%') 
            {
                UpdateRegInfo(result_loc, 1);
            }
        } 
        else if (stmt_node->stmtCode == RETURN) 
        {
            Node* expr = stmt_node->left;
            char* result_loc = ComputeExpr(expr);
            fprintf(fptr, "\nmovq %s, %%rax", result_loc);
            if (result_loc[0] == '%') 
            {
                UpdateRegInfo(result_loc, 1);
            }
        } 
        else
        {
            printf("Unknown statement type\n");
        }
        statements = statements->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO DO CODEGEN FOR ALL THE FUNCTIONS IN THE FILE
 ************************************************************************
*/
// Modify the Codegen function in codegen.c
// MODIFIED
void Codegen(NodeList* worklist) 
{
    fptr = fopen("assembly.s", "w+");
    if(fptr == NULL) 
    {
        printf("\n Could not create assembly file");
        return; 
    }
    NodeList* func_list = worklist;
    while(func_list != NULL) 
    {
        Node* func_node = func_list->node; 

        int num_vars = CountVariables(func_node->statements);
        int stack_space = num_vars * 8; 

        InitAsm(func_node->name);

        if (stack_space > 0) 
        {
            fprintf(fptr, "\nsubq $%d, %%rsp", stack_space);
        }

        regList = regHead = regLast = NULL;
        varList = varHead = varLast = NULL;
        CreateRegList();

        lastUsedOffset = 0;
        for(int i = 0; i < num_vars; i++) 
        {
            LongToCharOffset();
        }

        PutArgumentsFromStack(func_node->arguments);

        ProcessStatements(func_node->statements);

        if (stack_space > 0) 
        {
            fprintf(fptr, "\naddq $%d, %%rsp", stack_space);
        }

        RetAsm();

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

char* ComputeExpr(Node* expr) 
{
    if (expr->exprCode == VARIABLE)
    {
        char* var_name = expr->name;
        char* var_loc = LookUpVarInfo(var_name, INVAL);
        if (strcmp(var_loc, "") == 0) 
        {
            printf("Variable %s not found\n", var_name);
        }
        return var_loc;
    } 
    else if (expr->exprCode == CONSTANT) 
    {
        long const_value = expr->value;
        char* reg = GetNextAvailReg(false);
        if (strcmp(reg, "NoReg") != 0) 
        {
            fprintf(fptr, "\nmovq $%ld, %s", const_value, reg);
            UpdateRegInfo(reg, 0); 
            return reg;
        } 
        else 
        {
            LongToCharOffset();
            fprintf(fptr, "\nmovq $%ld, %s", const_value, lastOffsetUsed);
            return lastOffsetUsed;
        }
    } 
    else if (expr->exprCode == OPERATION) 
    {
        if (expr->opCode == FUNCTIONCALL) 
        {
            Node* func_decl_node = expr->left;
            NodeList* arguments = expr->arguments;
            PutArgumentsOnStack(arguments);
            fprintf(fptr, "\ncall %s", func_decl_node->name);

            char* reg = GetNextAvailReg(true); 
            if (strcmp(reg, "NoReg") != 0) 
            {
                fprintf(fptr, "\nmovq %%rax, %s", reg);
                UpdateRegInfo(reg, 0);
                return reg;
            } 
            else 
            {
                LongToCharOffset();
                fprintf(fptr, "\nmovq %%rax, %s", lastOffsetUsed);
                return lastOffsetUsed;
            }
        } 
        else if (expr->opCode == NEGATE) 
        {
            Node* operand_node = expr->left;
            char* operand_loc = ComputeExpr(operand_node);
            char* reg = GetNextAvailReg(false);

            if (strcmp(reg, "NoReg") != 0) 
            {
                fprintf(fptr, "\nmovq %s, %s", operand_loc, reg);
                fprintf(fptr, "\nnegq %s", reg);
                UpdateRegInfo(reg, 0);
                return reg;
            } 
            else 
            {
                fprintf(fptr, "\nmovq %s, %%rax", operand_loc);
                fprintf(fptr, "\nnegq %%rax");
                LongToCharOffset();
                fprintf(fptr, "\nmovq %%rax, %s", lastOffsetUsed);
                return lastOffsetUsed;
            }
        } 
        else 
        {
            Node* left_node = expr->left;
            Node* right_node = expr->right;
            char* left_loc = ComputeExpr(left_node);
            char* right_loc = ComputeExpr(right_node);
            char* reg = GetNextAvailReg(false);
            if (strcmp(reg, "NoReg") != 0) 
            {
                fprintf(fptr, "\nmovq %s, %s", left_loc, reg);
                switch (expr->opCode) 
                {
                    case ADD:
                        fprintf(fptr, "\naddq %s, %s", right_loc, reg);
                        break;

                    case SUBTRACT:
                        fprintf(fptr, "\nsubq %s, %s", right_loc, reg);
                        break;

                    case MULTIPLY:
                        fprintf(fptr, "\nimulq %s, %s", right_loc, reg);
                        break;

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
                        if (expr->right->exprCode == CONSTANT) 
                        {
                            long shift_count = expr->right->value;
                            fprintf(fptr, "\nshlq $%ld, %s", shift_count, reg);
                        } 
                        else 
                        {
                            char* right_loc = ComputeExpr(expr->right);
                            fprintf(fptr, "\npushq %%rcx");
                            fprintf(fptr, "\nmovq %s, %%rcx", right_loc);
                            fprintf(fptr, "\nshlq %%cl, %s", reg);
                            fprintf(fptr, "\npopq %%rcx");
                            if (right_loc[0] == '%') 
                            {
                                UpdateRegInfo(right_loc, 1);
                            }
                        }
                        break;

                    case BSHR:
                        if (expr->right->exprCode == CONSTANT) 
                        {
                            long shift_count = expr->right->value;
                            fprintf(fptr, "\nsarq $%ld, %s", shift_count, reg);
                        } 
                        else 
                        {
                            char* right_loc = ComputeExpr(expr->right);
                            fprintf(fptr, "\npushq %%rcx");
                            fprintf(fptr, "\nmovq %s, %%rcx", right_loc);
                            fprintf(fptr, "\nsarq %%cl, %s", reg);
                            fprintf(fptr, "\npopq %%rcx");
                            if (right_loc[0] == '%') 
                            {
                                UpdateRegInfo(right_loc, 1);
                            }
                        }
                        break;

                    default:
                        printf("Unsupported binary operator\n");
                        break;
                }
                UpdateRegInfo(reg, 0);

                if (left_loc[0] == '%') 
                {
                    UpdateRegInfo(left_loc, 1);
                }
                if (right_loc[0] == '%') 
                {
                    UpdateRegInfo(right_loc, 1); 
                }
                return reg;
            } 
            else 
            {
                printf("Unsupported expression type\n");
                return "";
            }
        }
    } 
    else 
    {
        printf("Unsupported expression type\n");
        return "";
    }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILIARY FUNCTIONS ABOVE THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/