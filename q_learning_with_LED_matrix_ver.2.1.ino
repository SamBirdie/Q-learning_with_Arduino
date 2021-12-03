#include "Maze.cpp"

// LED-matrix pins:
const byte ROWS_PINS[ROWS] = {2, 7, 19, 5, 13, 18, 12, 16};
const byte COLS_PINS[COLS] = {6, 11, 10, 3, 17, 4, 8, 9};

// This variable is for setting the prize's blinking and robot's moving nice and visible:
const short int REFRESH_TIMER_FREQ = 5000;

// Robot's starting coordinate indices:
const short int STARTING_ROW = 6;
const short int STARTING_COL = 2;

// Maximum length of the shortest path (i.e. max number of free point in the matrix,
// area minus borders):
const short int MAX_PATH_LENGTH = ROWS * COLS - (ROWS * 2 + (COLS - 2) * 2);

Maze maze;

bool blinkTarget;
bool surroundings[4];
short int refreshTimer;
short int showStepTimer;
short int targetsCoordinates[2];
short int robotsCoordinates[2]; // These should be changed when Robot is moved
short int shortestPath[40][2];
short int shortestPathCounter; // Counter to iterate the shortest path
short int zerosCoordinates[ROWS * COLS][2];
short int exploredMazeRewards[ROWS][COLS];
short int nearestZero[2];
short int obstaclesRow[MAX_OBSTACLES];
short int obstaclesCol[MAX_OBSTACLES];
int qValuesMatrix[ROWS][COLS][4]; // This needs to be integer (not double), because Arduino Uno doesn't have enough memory

// 2-dimensional array of pixels:
short int pixels[ROWS][COLS];
short int pixelsMaze[ROWS][COLS];

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  generateObstacles();
  maze = Maze(obstaclesRow, obstaclesCol);
  blinkTarget = false;
  targetsCoordinates[0] = maze.getPrizesRowCoordinate();
  targetsCoordinates[1] = maze.getPrizesColCoordinate();
  robotsCoordinates[0] = STARTING_ROW;
  robotsCoordinates[1] = STARTING_COL;
  refreshTimer = 0;
  showStepTimer = 0;

  for (int thisPin = 0; thisPin < 8; thisPin++) {
    pinMode(COLS_PINS[thisPin], OUTPUT);
    pinMode(ROWS_PINS[thisPin], OUTPUT);
    // Set the col pins (i.e. the cathodes)HIGH to ensure that the LEDS are off:
    digitalWrite(COLS_PINS[thisPin], HIGH);
  }

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (maze.getPrize(row, col) < -1) // Set LED on if prize < -1
        pixels[row][col] = LOW;
      else pixels[row][col] = HIGH; // else set LED off
      exploredMazeRewards[row][col] = -200;
      for (int k = 0; k < 4; k++)
        qValuesMatrix[row][col][k] = 0;
    }
  }

  for (int i = 0; i < ROWS * COLS; i++) {
    zerosCoordinates[i][0] = -1; // row
    zerosCoordinates[i][1] = -1; // column
  }

  for (int i = 0; i < 4; i++)
    surroundings[i] = true;

  exploreMaze();
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (exploredMazeRewards[row][col] < -1) // set LED on if prize < -1
        pixelsMaze[row][col] = LOW;
      else pixelsMaze[row][col] = HIGH; // else set LED off
    }
  }
  
  qTrain();
  for (int row = 0; row < ROWS; row++)
    for (int col = 0; col < COLS; col++)
      pixels[row][col] = pixelsMaze[row][col];
  blinkTarget = true;

  getShortestPath();
  robotsCoordinates[0] = STARTING_ROW;
  robotsCoordinates[1] = STARTING_COL;
}

void loop() {
  // Keep on showing the found shortest path from starting point to target point:
  walkShortestPath();
  refreshScreen();
}

void generateObstacles() {
  int i = 0;
  int obstacles = 0;
  int r, c;
  while (obstacles <= MAX_OBSTACLES && i < MAX_OBSTACLES * 10) {
    r = random(ROWS - 2) + 1; // Ignore walls
    c = random(COLS - 2) + 1; // Ignore walls
    if (!(r == STARTING_ROW && c == STARTING_COL)) { // Robot's starting point is not allowed
      obstaclesRow[i] = r;
      obstaclesCol[i] = c;
      obstacles++;
    }
    i++;
  }
}

void walkShortestPath() {
  if (showStepTimer < REFRESH_TIMER_FREQ / 5) {
    showStepTimer++;
  }
  else if (showStepTimer == REFRESH_TIMER_FREQ / 5) {
    if (shortestPath[shortestPathCounter][0] != -1) {
      moveOnSP();
    }
    else shortestPathCounter = 0;
    showStepTimer = 0;
  }
}

void getShortestPath() {
  for (int i = 0; i < MAX_PATH_LENGTH; i++)
    for (int j = 0 ; j < 2; j++)
      shortestPath[i][j] = -1;
  int i = 0;
  int action;
  robotsCoordinates[0] = STARTING_ROW;
  robotsCoordinates[1] = STARTING_COL;
  while (!isTerminalState()) {
    shortestPath[i][0] = robotsCoordinates[0];
    shortestPath[i][1] = robotsCoordinates[1];
    i++;
    action = nextAction(1);
    nextLocation(action);
  }
  shortestPath[i][0] = targetsCoordinates[0];
  shortestPath[i][1] = targetsCoordinates[1];
}

void nextLocation(int action) {
  if (action == 0) robotsCoordinates[0] -= 1;
  else if (action == 1) robotsCoordinates[1] += 1;
  else if (action == 2) robotsCoordinates[0] += 1;
  else if (action == 3) robotsCoordinates[1] -= 1;
}

int nextAction(double epsilon) {
  int max = 0;
  if ((float) rand() / RAND_MAX < epsilon) {
    double maxQValue = qValuesMatrix[robotsCoordinates[0]][robotsCoordinates[1]][0];
    for (int i = 1; i < 4; i++) {
      if (qValuesMatrix[robotsCoordinates[0]][robotsCoordinates[1]][i] > maxQValue) {
        maxQValue = qValuesMatrix[robotsCoordinates[0]][robotsCoordinates[1]][i];
        max = i;
      }
    }
    return max;
  }
  else return rand() % 4;
}

bool isTerminalState() {
  if (exploredMazeRewards[robotsCoordinates[0]][robotsCoordinates[1]] == -1)
    return false;
  else return true;
}

void qTrain() {
  double discountFactor = 0.9;
  double learningRate = 0.9;
  int oldRow;
  int oldCol;
  refreshTimer = 0;

  for (int r = 0; r < ROWS; r++)
    for (int c = 0; c < COLS; c++) {
      pixels[r][c] = HIGH;
    }

  for (int i = 0; i < 1000; i++) {
    // Blink four LED's in the middle of the matrix while training the algorithm:
    if (refreshTimer < 100) {
      pixels[4][4] = LOW;
      pixels[4][3] = LOW;
      pixels[3][4] = LOW;
      pixels[3][3] = LOW;
      refreshScreen();
      refreshTimer++;
    }
    else if (refreshTimer < 200) {
      pixels[4][4] = HIGH;
      pixels[4][3] = HIGH;
      pixels[3][4] = HIGH;
      pixels[3][3] = HIGH;
      refreshScreen();
      refreshTimer++;
    }
    else refreshTimer = 0;
    // end of blinking code

    setStartingLocation();
    while (!isTerminalState()) {
      short int trainingAction = nextAction(0.9);
      oldRow = robotsCoordinates[0];
      oldCol = robotsCoordinates[1];
      nextLocation(trainingAction);
      int reward = exploredMazeRewards[robotsCoordinates[0]][robotsCoordinates[1]];
      double oldQValue = qValuesMatrix[oldRow][oldCol][trainingAction];
      double maxQValue = qValuesMatrix[robotsCoordinates[0]][robotsCoordinates[1]][0];

      for (int i = 1; i < 4; i++)
        if (qValuesMatrix[robotsCoordinates[0]][robotsCoordinates[1]][i] > maxQValue)
          maxQValue = qValuesMatrix[robotsCoordinates[0]][robotsCoordinates[1]][i];
      double temporalDifference = reward + (discountFactor * maxQValue) - oldQValue;
      double newQValue = oldQValue + (learningRate * temporalDifference);
      qValuesMatrix[oldRow][oldCol][trainingAction] = newQValue;
    }
  }
  pixels[4][4] = HIGH;
}

void setStartingLocation() {
  robotsCoordinates[0] = rand() % ROWS;
  robotsCoordinates[1] = rand() % COLS;
  while (isTerminalState()) {
    robotsCoordinates[0] = rand() % ROWS;
    robotsCoordinates[1] = rand() % COLS;
  }
}

void exploreMaze() {
  exploreNeighbours();
  while (zerosCoordinates[0][0] != -1) {

    for (int i = ROWS * COLS - 1; i >= 0; i--) {
      if (zerosCoordinates[i][0] != -1) {
        nearestZero[0] = zerosCoordinates[i][0];
        zerosCoordinates[i][0] = -1;
        nearestZero[1] = zerosCoordinates[i][1];
        zerosCoordinates[i][1] = -1;
        break;
      }
    }
    // Move to nearest zero:
    robotsCoordinates[0] = nearestZero[0];
    robotsCoordinates[1] = nearestZero[1];
    exploreNeighbours();
  }
}

void exploreNeighbours() {
  short int robotsRow = robotsCoordinates[0];
  short int robotsCol = robotsCoordinates[1];
  exploredMazeRewards[robotsRow][robotsCol] = -1;

  if (robotsRow > 0)
    setReward(robotsRow - 1, robotsCol);
  if (robotsRow + 1 < ROWS)
    setReward(robotsRow + 1, robotsCol);
  if (robotsCol > 0)
    setReward(robotsRow, robotsCol - 1);
  if (robotsCol + 1 < COLS)
    setReward(robotsRow, robotsCol + 1);
}

void setReward(int r, int c) {
  if (exploredMazeRewards[r][c] == -200 && maze.getPrize(r, c) == -1) {
    exploredMazeRewards[r][c] = 0;
    addZerosCoordinates(r, c);
  }
  else if (maze.getPrize(r, c) == 100) {
    exploredMazeRewards[r][c] = 100;
  }
  else if (exploredMazeRewards[r][c] != -1)
    exploredMazeRewards[r][c] = -100;
}

void addZerosCoordinates(int r, int c) {
  for (int i = 0; i < ROWS * COLS; i++)
    if (zerosCoordinates[i][0] == -1) {
      zerosCoordinates[i][0] = r;
      zerosCoordinates[i][1] = c;
      break;
    }
}

void moveOnSP() { // Move robot to next node on shortest path
  pixels[robotsCoordinates[0]][robotsCoordinates[1]] = HIGH; // Turn the previous led off
  robotsCoordinates[0] = shortestPath[shortestPathCounter][0];
  robotsCoordinates[1] = shortestPath[shortestPathCounter][1];
  shortestPathCounter++;
  showSP();
}

void showSP() {
  if (refreshTimer < REFRESH_TIMER_FREQ / 20) {
    pixels[robotsCoordinates[0]][robotsCoordinates[1]] = LOW;
    refreshTimer++;
  }
  else if (refreshTimer < REFRESH_TIMER_FREQ / 10) {
    pixels[robotsCoordinates[0]][robotsCoordinates[1]] = HIGH;
    refreshTimer++;
  }
  else refreshTimer = 0;
}

void refreshScreen() {

  // Iteration over the ROWS_PINS (anodes):
  for (int thisRow = 0; thisRow < ROWS; thisRow++) {
    // take the row pin (anode) high:
    digitalWrite(ROWS_PINS[thisRow], HIGH);
    // Iteration over the COLS_PINS (cathodes):
    for (int thisCol = 0; thisCol < COLS; thisCol++) {
      // Get the state of the current pixel;
      int thisPixel = pixels[thisRow][thisCol];
      // When the row is HIGH and the col is LOW, the LED where they meet turns on:
      digitalWrite(COLS_PINS[thisCol], thisPixel);
      // Turn the pixel off:
      if (thisPixel == LOW) {
        digitalWrite(COLS_PINS[thisCol], HIGH);
      }
    }
    // Set the row pin LOW to turn off the whole row:
    digitalWrite(ROWS_PINS[thisRow], LOW);
    // Blink target-LED:
    if (blinkTarget) {
      if (refreshTimer < REFRESH_TIMER_FREQ) {
        pixels[targetsCoordinates[0]][targetsCoordinates[1]] = LOW;
        pixels[robotsCoordinates[0]][robotsCoordinates[1]] = LOW;
        refreshTimer++;
      }
      else if (refreshTimer < REFRESH_TIMER_FREQ * 2) {
        pixels[targetsCoordinates[0]][targetsCoordinates[1]] = HIGH;
        pixels[robotsCoordinates[0]][robotsCoordinates[1]] = LOW;
        refreshTimer++;
      }
      else refreshTimer = 0;
    }
  }
}
