# TCG 2023 hw2
## Description
This is an assignment where an AI, written using an MCTS-based program with UCB, competes against other players or local AI under the condition of a known dice sequence.

## Compile
```
make
```
It will generate an executable file "test" in the current folder.

```
cd hw2/baseline
make
```
It will generate baseline executables implementing the algorithms using random moves, alpha-beta pruning with a depth of 2, and alpha-beta pruning with a depth of 8.

```
cd hw2/game
make
```
It will generate a main program for playing Einstein chess in the terminal.


## Execution
In the root directory of this assignment,
```
./hw2/game -p0 test -p1 hw2/baseline/xxx -r 20
```
It will play 20 matches between your AI and the selected baseline AI (xxx).
