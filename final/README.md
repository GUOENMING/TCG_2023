# TCG 2023 final
## Description
This is a project where you pit your own Einstein chess AI against other players or AI.

## Compile
```
make
```
It will generate an executable file "agent" in the current folder.

## Execution
There are two ways to open the GUI.
1. Create your own game room:
    ```
    cd windows/open
    java -jar Launcher.jar
    ```
2. Join someone else's game room
    ```
    cd windows/enter
    java -jar Launcher.jar
    ```
In the GUI, set the "AI Path" to the agent that was just compiled, and you can start playing matches with others.