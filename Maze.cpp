//#include <Arduino.h> // If you need to print to console from this class, uncomment this line

// Maze's target coordinate indices:
const short int TARGET_ROW = 1;
const short int TARGET_COL = 4;

// LED-matrix's dimensions:
const short int ROWS = 8;
const short int COLS = 8;

// number of MAX_OBSTACLES in maze:
const unsigned char MAX_OBSTACLES = 28; // 32 is nice number for demonstration!

class Maze {
  private:
    short int mazeRewards[ROWS][COLS];
    unsigned char obstacleCount = 0;

  public:
    Maze() { }

    Maze(int obstaclesRow[], int obstaclesCol[]) {

      for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) {
          mazeRewards[r][c] = -100;
          if (r != 0 && c != 0 && r != ROWS - 1 && c != COLS - 1)
            mazeRewards[r][c] = -1;
        }
      for (int k = 0; k < MAX_OBSTACLES; k++) {
        if (mazeRewards[obstaclesRow[k]][obstaclesCol[k]] == -1) {
          //int neighbourCount = neighbourObstacles(obstaclesRow[k], obstaclesCol[k]);
          if (neighbourObstacles(obstaclesRow[k], obstaclesCol[k]) < 4 && (obstaclesRow[k] != TARGET_ROW && obstaclesCol[k] != TARGET_COL)) { //raja-arvo 3 toimi aiemmin
            mazeRewards[obstaclesRow[k]][obstaclesCol[k]] = -100;
            obstacleCount++;
          }
        }
      }
      mazeRewards[TARGET_ROW][TARGET_COL] = 100; // target
    }

    int neighbourObstacles(int r, int c) { // I guess this isn't working properly...
      int count = 0;
      if (mazeRewards[r + 1][c] == -100) count++;
      if (mazeRewards[r + 1][c + 1] == -100) count++;
      if (mazeRewards[r][c + 1] == -100) count++;
      if (mazeRewards[r][c - 1] == -100) count++;
      if (mazeRewards[r + 1][c - 1] == -100) count++;
      if (mazeRewards[r - 1][c - 1] == -100) count++;
      if (mazeRewards[r - 1][c] == -100) count++;
      if (mazeRewards[r - 1][c + 1] == -100) count++;
    
    return count;
}

int getPrize(int r, int c) {
  return mazeRewards[r][c];
}

int getPrizesRowCoordinate() {
  return TARGET_ROW;
}

int getPrizesColCoordinate() {
  return TARGET_COL;
}
};
