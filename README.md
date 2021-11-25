# Q-learning_with_Arduino
This is a demonstration of Q-learning algorithm with Ardino Uno and 8x8 LED-matrix implemented in C++ using Arduino IDE.

## About the demo

Code/demonstration shows how the virtual robot examines the maze, Q-learning algorithm calculates the shortest path from starting point to target point, and the virtual robot then walks the found path. Four LED's are blinking in the middle of the matrix while the Q-learning algorithm is being trained. It may take a few seconds. When the training is done, the robot starts repeatedly walking the found path. The blinking LED is the target point and the moving LED is the robot.

Workflow is somewhat like this:
1) Random maze is created
2) Robot explores the maze
3) Q-learning algorithm is trained
4) Shortest path from start to end is counted using the q-values which the Q-learning algorithm now knows after the training
5) Robot walks the shortest path

The main code is in file `q_learning_with_LED_matrix_ver.2.1.ino` and the included Maze-object file is `Maze.cpp`.


![LED-matrix_and_Arduino](https://user-images.githubusercontent.com/65365440/143456081-babba0c5-a8b1-4537-a11c-fe88dee45ad7.jpg)
