#include "optimizer.h"

void Optimizer(NodeList *funcdecls) {
    // Apply optimizations until no more changes occur
    bool changed;
    do {
        changed = false;
        // Apply Constant Folding optimization
        if (ConstantFolding(funcdecls)) {
            changed = true;
        }
        // Apply Constant Propagation optimization
        if (ConstProp(funcdecls)) {
            changed = true;
        }
        // Apply Dead Code Elimination optimization
        if (DeadAssign(funcdecls)) {
            changed = true;
        }
    } while (changed);
}