# Q-learning_with_Arduino
This is a demonstration of Q-learning algorithm with Ardino Uno and 8x8 LED-matrix implemented in C++ using Arduino IDE.

![LED-matrix_and_Arduino](https://user-images.githubusercontent.com/65365440/143456081-babba0c5-a8b1-4537-a11c-fe88dee45ad7.jpg)


Code/demonstration shows how the virtual robot examines the maze, Q-learning algorithm calculates the shortest path from starting point to target point, and the virtual robot then walks the found path. Four LED's are blinking in the middle of the matrix while the Arduino is teaching the Q-learning algorithm. It may take a few seconds. When the teaching is done, the robot starts continuously walking the found path. The blinking LED is the target point and the moving LED is the robot.

Workflow is somewhat like this:
1) Random maze is created
2) Robot explores the maze
3) Q-learning algorithm is teached
4) Shortest path from start to end is counted using the q-values that the Q-learning algorithm has learned
5) Robot walks the shortest path

