# Robot in Maze

## Description
Robot in Maze is a C program simulating a robot navigating a maze to collect markers. The program features a dynamically generated maze with blocks and markers, and uses a modified Breadth-First Search (BFS) algorithm for pathfinding. The robot, represented as a green triangle, starts from a user-defined home square.

## Compilation and Execution
Compile and run the program with:

```bash
gcc -o maze maze.c graphics.c
./maze | java -jar drawapp-3.0.jar
```
Example usage:
```bash
./maze 12 12 north | java -jar drawapp-3.0.jar
```
Sets the home square at cell (12, 12) with the robot facing north.

## Further Improvements
The program runs until all reachable markers are collected, stopping at the home square when no markers are available for pickup. However, the robot cannot pick and return the markers that are trapped/surrounded by blocks. Furthermore, the robot cannot collect the markers when the robot itself is surrounded by blocks. The program should be optimized further to prevent the aforementioned cases from occurring.
