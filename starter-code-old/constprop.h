#pragma once
/*
*******************************************************************************************
     CONSTPROP.H : DECLARATION OF FUNCTIONS ADDED IN CONSTPROP.C HERE
********************************************************************************************
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Expression.h"

/*
*******************************************************************************************
  THIS STRUCTURE CAN BE USED TO STORE THE CONSTANT VALUE AND THE VARIABLE IT IS ASSIGNED TO.
*********************************************************************************************
*/
typedef struct _refConst {
    char* name;
    long val;
    struct _refConst* next;
} refConst;

extern refConst *lastNode, *headNode;
extern bool madeChange;

/*
*******************************************************************************************
  FUNCTION DECLARATIONS
********************************************************************************************
*/
void FreeConstList();
void UpdateConstList(char* name, long val);
refConst* LookupConstList(char* name);
void TrackConst(NodeList* statements);
bool ConstProp(NodeList* funcdecls);
void RemoveConst(char* name);

/*
******************************************************************************************
  ADD DECLARATIONS OF ANY FUNCTIONS YOU ADD BELOW THIS LINE
*******************************************************************************************
*/