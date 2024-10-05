/*
************************************************************************
  OPTIMIZER.C : IMPLEMENT THE OPTIMIZER FUNCTION HERE
************************************************************************
*/

#include "optimizer.h"
#include "constfolding.h"
#include "constprop.h"
#include "deadassign.h"

/*
*************************************************************************************
  YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL 
  UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO 
  DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

void Optimizer(NodeList *funcdecls) {
    bool changed;
    do {
        changed = false;

        // Apply Constant Folding
        if (ConstantFolding(funcdecls)) {
            changed = true;
        }

        // Apply Constant Propagation
        if (ConstProp(funcdecls)) {
            changed = true;
        }

        // Apply Dead Code Elimination
        if (DeadAssign(funcdecls)) {
            changed = true;
        }

    } while (changed);
}

/*
************************************************************************
  ADD IMPLEMENTATIONS OF ANY FUNCTIONS YOU ADD BELOW THIS LINE
************************************************************************
*/