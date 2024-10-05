/*
*************************************************************************************
     CONSTFOLDING.H : DECLARATION OF FUNCTIONS ADDED IN CONSTFOLDING.C HERE
**************************************************************************************
*/

#include "Expression.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern bool madeChange;

/*
***********************************************************************
  FUNCTION DECLARATIONS
************************************************************************
*/

long CalcExprValue(Node* node);
Node* FoldExpr(Node* node);
void FoldStmt(Node* stmt);
void FoldFunc(Node* funcNode);
bool ConstantFolding(NodeList* list);

/*
***********************************************************************
  ADD DECLARATIONS OF ANY FUNCTIONS YOU ADD BELOW THIS LINE
************************************************************************
*/

/* You can add additional function declarations here if needed */