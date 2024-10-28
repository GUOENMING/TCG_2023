# TCG 2023 hw1
## Description
This is an assignment that uses the A* algorithm to search for the minimum number of moves to solve a single-player Einstein chess puzzle.

## Compile
```
make
```
It will generate an executable file "test" in the current folder.

```
cd hw1_verifier
make
```
It will generate a local test executable file called "verifier."

## Execution
In the root directory of this assignment,
```
./hw1_verifier/verifier test hw1_verifier/testcases/xx.in 
```
It will test whether your AI can generate legal moves to solve the current board state.
