#include "optimizer.h"

void Optimizer(NodeList *funcdecls) 
{
    bool changed = true;

    while (changed)
    {
        changed = ConstantFolding(funcdecls) || ConstProp(funcdecls) || DeadAssign(funcdecls);
    }
}
