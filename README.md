# PSET3

## Part 1.1

# TODO List for Toy Compiler Part 1.1: Optimizations

## 1. Understand the Intermediate Representation (IR)

### 1.1 Review IR Data Structures
- [ ] Open and read `expression.h` to understand data structures for expressions, statements, and functions.
- [ ] Examine `expression.c` to see how IR nodes are created using the `Create*` functions.
- [ ] Identify different node types (e.g., constants, variables, operations, function calls).

### 1.2 Explore Helper Functions
- [ ] Locate `print*` functions in `expression.c` for debugging and visualizing IR nodes.
- [ ] Step through `Create*` functions to understand the construction and linking of IR nodes.

## 2. Set Up the Optimizer Framework

### 2.1 Create Optimizer Files
- [ ] Create `optimizer.c`.
- [ ] Create `optimizer.h`.
- [ ] In `optimizer.h`, declare the optimizer function (e.g., `void OptimizeIR(IR *ir);`).
- [ ] In `optimizer.c`, define the `OptimizeIR` function skeleton to apply optimizations sequentially.

### 2.2 Include Necessary Headers
- [ ] Include `expression.h` in `optimizer.c` to access IR structures.
- [ ] Include optimization headers:
  - `constfolding.h`
  - `constprop.h`
  - `deadassign.h`

## 3. Implement Constant Folding

### 3.1 Create Constant Folding Files
- [ ] Create `constfolding.c`.
- [ ] Create `constfolding.h`.
- [ ] In `constfolding.h`, declare the `void ConstantFold(IR *ir);` function.
- [ ] In `constfolding.c`, implement the logic to evaluate and fold constant expressions.

### 3.2 Implement `ConstantFold` Function
- [ ] Traverse each statement in the IR.
- [ ] Identify expressions where all operands are constants.
- [ ] Use `CalcExprValue` to compute the result of constant expressions.
- [ ] Replace folded expressions with a single constant node in the IR.

### 3.3 Handle Supported Operations
- [ ] Ensure `CalcExprValue` supports `+`, `-`, `*`, `/`, and negation.
- [ ] Extend `CalcExprValue` if additional operations are introduced.

## 4. Implement Constant Propagation

### 4.1 Create Constant Propagation Files
- [ ] Create `constprop.c`.
- [ ] Create `constprop.h`.
- [ ] In `constprop.h`, declare the `void ConstantPropagate(IR *ir);` function.
- [ ] In `constprop.c`, implement the logic to replace variable references with their constant values.

### 4.2 Implement `ConstantPropagate` Function
- [ ] Maintain a mapping of variables to their constant values within each function.
- [ ] Replace variable references with constant values when possible.
- [ ] Ensure constant propagation is confined within individual functions.

### 4.3 Update IR Nodes
- [ ] Modify assignment statements to record constant assignments.
- [ ] Replace usages of variables with their constant values in subsequent statements.

## 5. Implement Dead Code Elimination

### 5.1 Create Dead Code Elimination Files
- [ ] Create `deadassign.c`.
- [ ] Create `deadassign.h`.
- [ ] In `deadassign.h`, declare the `void DeadCodeEliminate(IR *ir);` function.
- [ ] In `deadassign.c`, implement the logic to remove statements that do not affect the program's outcome.

### 5.2 Implement `DeadCodeEliminate` Function
- [ ] Analyze variable usage to determine unused variables.
- [ ] Identify dead assignments where variables are not used in subsequent statements or the return value.
- [ ] Remove dead assignments from the IR.

### 5.3 Ensure Local Scope
- [ ] Confine dead code elimination to statements within a single function.
- [ ] Avoid considering inter-function dependencies.

## 6. Integrate Optimizations into the Optimizer

### 6.1 Implement Iterative Optimization Loop
- [ ] In `optimizer.c`, set up a loop to apply optimizations until no further changes occur.
- [ ] Apply optimizations in the following sequence:
  1. **Constant Folding**
  2. **Constant Propagation**
  3. **Dead Code Elimination**
- [ ] After each pass, check if any changes were made; exit the loop if not.

### 6.2 Handle Optimization Dependencies
- [ ] Apply constant folding before constant propagation to maximize folding opportunities.
- [ ] Apply dead code elimination after folding and propagation to clean up any new dead code.

### 6.3 Ensure Correctness
- [ ] Validate IR integrity after each optimization pass.
- [ ] Use `print*` functions from `expression.c` to visualize IR changes and verify correctness.

## 7. Test Your Optimizations

### 7.1 Create Test Cases
- [ ] Write toy programs with constant expressions to test constant folding.
- [ ] Include variable assignments with constant values to test constant propagation.
- [ ] Add assignments that are never used to test dead code elimination.

### 7.2 Run Optimizer and Inspect IR
- [ ] Apply the optimizer to your test IRs.
- [ ] Use `print*` functions to print the IR before and after optimization to ensure correct transformations.

### 7.3 Validate Against Expected Output
- [ ] Compare the results to ensure the optimized IR produces the same outcomes as the original IR.
- [ ] Verify that optimizations reduce IR complexity without altering program behavior.

## 8. Finalize and Clean Up

### 8.1 Ensure No Memory Leaks
- [ ] Use provided utility functions in `expression.h` and `expression.c` to manage memory.
- [ ] Free any additional data structures created during optimization.

### 8.2 Document Your Code
- [ ] Add clear comments explaining the logic behind each optimization step.
- [ ] Ensure all header files (`*.h`) correctly declare their corresponding implementation functions.

### 8.3 Final Testing
- [ ] Run all provided and custom test cases to ensure all optimizations work as intended.
- [ ] Test edge cases, such as functions with the maximum number of arguments or complex expression trees.

## 9. Summary of File Structure for Part 1.1