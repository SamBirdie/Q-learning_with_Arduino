//#include <Arduino.h> // If you need to print to console from this class, uncomment this line

// Maze's target coordinate indices:
const short int TARGET_ROW = 1;
const short int TARGET_COL = 5;
const short int ALLOWED_NEIGHBOURS = 3;

// LED-matrix's dimensions:
const short int ROWS = 8;
const short int COLS = 8;

// Number of MAX_OBSTACLES in maze:
const unsigned char MAX_OBSTACLES = 32;

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
          bool isTargetOrNeighbourIsTarget = (obstaclesRow[k] == TARGET_ROW && obstaclesCol[k] == TARGET_COL)
                                             || (obstaclesRow[k] + 1 == TARGET_ROW && obstaclesCol[k] == TARGET_COL)
                                             || (obstaclesRow[k] + 1 == TARGET_ROW && obstaclesCol[k] + 1 == TARGET_COL)
                                             || (obstaclesRow[k] == TARGET_ROW && obstaclesCol[k] + 1 == TARGET_COL)
                                             || (obstaclesRow[k] == TARGET_ROW && obstaclesCol[k] - 1 == TARGET_COL)
                                             || (obstaclesRow[k] + 1 == TARGET_ROW && obstaclesCol[k] - 1 == TARGET_COL)
                                             || (obstaclesRow[k] - 1 == TARGET_ROW && obstaclesCol[k] - 1 == TARGET_COL)
                                             || (obstaclesRow[k] - 1 == TARGET_ROW && obstaclesCol[k] == TARGET_COL)
                                             || (obstaclesRow[k] - 1 == TARGET_ROW && obstaclesCol[k] + 1 == TARGET_COL);
          if (neighbourObstacles(obstaclesRow[k], obstaclesCol[k]) < ALLOWED_NEIGHBOURS && !isTargetOrNeighbourIsTarget) {
            mazeRewards[obstaclesRow[k]][obstaclesCol[k]] = -100;
            obstacleCount++;
          }
        }
      }
      mazeRewards[TARGET_ROW][TARGET_COL] = 100; // target
    }

    int neighbourObstacles(int r, int c) {
      int count = 0;
      if (neighboursNeighbourObstacles(r + 1, c) < ALLOWED_NEIGHBOURS) {
        if (mazeRewards[r + 1][c] == -100)
          count++;
      }
      else return ALLOWED_NEIGHBOURS;

      if (neighboursNeighbourObstacles(r + 1, c) < ALLOWED_NEIGHBOURS) {
        if (mazeRewards[r + 1][c + 1] == -100)
          count++;
      }
      else return ALLOWED_NEIGHBOURS;

      if (neighboursNeighbourObstacles(r + 1, c) < ALLOWED_NEIGHBOURS) {
        if (mazeRewards[r][c + 1] == -100)
          count++;
      }
      else return ALLOWED_NEIGHBOURS;

      if (neighboursNeighbourObstacles(r + 1, c) < ALLOWED_NEIGHBOURS) {
        if (mazeRewards[r][c - 1] == -100)
          count++;
      }
      else return ALLOWED_NEIGHBOURS;

      if (neighboursNeighbourObstacles(r + 1, c) < ALLOWED_NEIGHBOURS) {
        if (mazeRewards[r + 1][c - 1] == -100)
          count++;
      }
      else return ALLOWED_NEIGHBOURS;

      if (neighboursNeighbourObstacles(r + 1, c) < ALLOWED_NEIGHBOURS) {
        if (mazeRewards[r - 1][c - 1] == -100)
          count++;
      }
      else return ALLOWED_NEIGHBOURS;

      if (neighboursNeighbourObstacles(r + 1, c) < ALLOWED_NEIGHBOURS) {
        if (mazeRewards[r - 1][c] == -100)
          count++;
      }
      else return ALLOWED_NEIGHBOURS;

      if (neighboursNeighbourObstacles(r + 1, c) < ALLOWED_NEIGHBOURS) {
        if (mazeRewards[r - 1][c + 1] == -100)
          count++;
      }
      else return ALLOWED_NEIGHBOURS;

      return count;
    }

    int neighboursNeighbourObstacles(int r, int c) {
      int count = 0;
      if (r < ROWS - 1 && mazeRewards[r + 1][c] == -100) count++;
      if (r < ROWS - 1 && c < COLS - 1 && mazeRewards[r + 1][c + 1] == -100) count++;
      if (c < COLS - 1 && mazeRewards[r][c + 1] == -100) count++;
      if (c > 0 && mazeRewards[r][c - 1] == -100) count++;
      if (r < ROWS - 1 && c > 0 && mazeRewards[r + 1][c - 1] == -100) count++;
      if (r > 0 && c > 0 && mazeRewards[r - 1][c - 1] == -100) count++;
      if (r > 0 && mazeRewards[r - 1][c] == -100) count++;
      if (r > 0 && c < COLS - 1 && mazeRewards[r - 1][c + 1] == -100) count++;

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
