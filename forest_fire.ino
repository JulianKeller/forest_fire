#include <MD_MAX72xx.h>
#include <SPI.h>

// display pins
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW   // Set display type  so that  MD_MAX72xx library treets it properly
#define MAX_DEVICES  4    // Total number display modules
#define CLK_PIN   13      // Clock pin to communicate with display
#define DATA_PIN  11      // Data pin to communicate with display
#define CS_PIN    3       // Control pin to communicate with display
#define DEBUG 1

// potentiometer
#define POT A2

// display dimensions
#define MAX_Y 8
#define MAX_X 32

// Define max iterations
#define MAX_ITERATIONS 10

// cell states
#define EMPTY 0
#define BURNING 1
#define TREE 2


int hasChanged = 1;
int iterations = 0;
int previousSum = 0;
int sum_count = 0;
int average = 0;
int previousAverage = 0;


int one = 0;
int two = 0;
int three = 0;




// init game board so we can easily input a starting pattern
int board[MAX_Y][MAX_X] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

int board_aux[MAX_Y][MAX_X] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};



MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);   // display object

void setup() {
  mx.begin();             // initialize display
  mx.control(MD_MAX72XX::INTENSITY, 0);     // SET DISPLAY brightness
  Serial.begin(9600);     // open the serial port at 9600 bps:

  // get noise as long as analog 1 is not in use
  randomSeed(analogRead(1));

  //  initBoard(board, 1);
  int totalGridSize = MAX_X * MAX_Y;
  int treesToPlant = totalGridSize / 4;
  random_init_board(board, treesToPlant, TREE);
  displayBoard(board);
  //  set_random_point_near_neighbor(TREE, 3);
  delay(1500);
  flashBoard(3);

}


void loop() {
  // enable this to control speed with pot
  //  int val = analogRead(POT);
  //  Serial.println(val);
  //  delay(val);

  // enable for hard coded speed
  delay(1000);
  if (DEBUG) printBoard();
  displayBoard(board);
  forestFire();

}

// TODO some problem where the game stops working and freezes. Prob memory leak or something
void forestFire() {


  // create a new board
  int nextBoard[MAX_Y][MAX_X] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };


  int count = 0;


  /*
     RULES:
        A burning cell turns into an empty cell
        A tree will burn if at least one neighbor is burning
        A tree ignites with probability   f   even if no neighbor is burning
        An empty space fills with a tree with probability   p
  */

  // # TODO ALMOST always getting TREE

  // calculate the state of the next board
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {

      // A burning cell turns into an empty cell
      if (board[y][x] == BURNING) {
        nextBoard[y][x] = EMPTY;
        if (DEBUG) Serial.println("BURNING --> EMPTY");
      }

      // Any dead cell with three live neighbours becomes a live cell.
      else if (board[y][x] == TREE) {
        // if any cell neighbor is on fire
        int onFire = neighborsOnFire(y, x);
        if (onFire) {
          nextBoard[y][x] = BURNING;
          if (DEBUG) Serial.println("TREE --> BURNING");
        }
        else if (spontaneouslyCombust()) {
          // ignite tree with a probability
          nextBoard[y][x] = BURNING;
          if (DEBUG) Serial.println("TREE --> BURNING - SPONT");
        }
        else {
          if (DEBUG) Serial.println("TREE --> TREE");
          nextBoard[y][x] = board[y][x];
        }
      }

      // All other live cells die in the next generation. Similarly, all other dead cells stay dead.
      else if (board[y][x] == EMPTY) {
        // plant tree with probability
        if (plantTree()) {
          nextBoard[y][x] = TREE;
          if (DEBUG) Serial.println("EMPTY --> TREE - SPONT");
        }
        else {
          if (DEBUG) Serial.println("EMPTY --> EMPTY");
          nextBoard[y][x] = board[y][x];
        }
      }
    }
  }
  // update the board
    copyBoard(nextBoard, board);
}


// return true if we should plant a tree
int plantTree() {
  int p = random(0, 100);
  if (p < 1) {
    return 1;
  }
  return 0;
}

// return true if the tree should burst into flames
int spontaneouslyCombust() {
  int p = random(0, 100);
  if (p < 1) {
    return 1;
  }
  return 0;
}

// copy the src board to the destination board
void copyBoard(int src[MAX_Y][MAX_X], int dest[MAX_Y][MAX_X]) {
  hasChanged = 0;
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      // detect if board has changed so it doesn't get stuck
      if (dest[y][x] != src[y][x]) {
        hasChanged = 1;
      }
      dest[y][x] = src[y][x];
    }
  }
}

// flash board on and off number of times
void flashBoard(int times) {
  int delay_time = 500;
  for (int i = 0; i < times; i++) {
    // clear the board with a flash
    displayBoard(board_aux);
    delay(delay_time);
    displayBoard(board);
    delay(delay_time);
  }
}

// TODO this almost works, but fails to return correct count
int neighborsOnFire(int y, int x) {
  int count = 0;
  // perp and vertical
  for (int j = -1; j < 2; j++) {
    for (int i = -1; i < 2; i++) {
      // don't count self
      if (i == 0 && j == 0) continue;
      // else if neighbor on fire, return true
      if (board[j][i] == BURNING) {
        return 1;
      }
    }
  }
  return 0;
}

void displayBoard(int the_board[MAX_Y][MAX_X]) {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      // if burning flash the led
      if (the_board[y][x] == BURNING) {
        flashIndividualLED(y, x, 4);
        mx.setPoint(y, x, 0);
      }
      // empty turn off LED
      else if (the_board[y][x] == EMPTY) {
        mx.setPoint(y, x, 0);
      }
      // TREE, turn on led
      else if (the_board[y][x] == TREE) {
        mx.setPoint(y, x, 1);
      }
    }
  }
}

// flash individual LED
void flashIndividualLED(int y, int x, int repeat) {
  int delayTime = 80;
  for (int i = 0; i < repeat; i++) {
    mx.setPoint(y, x, 0);
    delay(delayTime);
    mx.setPoint(y, x, 1);
    delay(delayTime);
  }
}

// add up the total value of the board
int sumBoard() {
  int sum = 0;
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      sum += board[y][x];
    }
  }
  return sum;
}

void initBoard(int board[MAX_Y][MAX_X], int value) {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      board[y][x] = value;
    }
  }
}

void resetBoard() {
  //  // clear the board with a flash
  ////    initBoard(board, 1);
  //    setBoardOutline(board);
  //    displayBoard(board);
  //    delay(100);
  //    initBoard(board, 0);
  //    displayBoard(board);

  // reset the board with 100 random points
  //  for (int i = 0; i < 10; i++) randomDisplay();

  set_random_point_near_neighbor(1, 3);

  //  set_random_point(1, 100);

  // set random board
  //    random_init_board(board, 100);
  sum_count = 0;
}



void setBoardOutline(int board[MAX_Y][MAX_X]) {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      if (y == 0 || y == MAX_Y - 1) board[y][x] = 1;
      if (x == 0 || x == MAX_X - 1) board[y][x] = 1;
    }
  }
}

void random_init_board(int board[MAX_Y][MAX_X], int num_points, int state) {
  int x = 0;
  int y = 0;
  for (int i = 0; i < num_points; i++) {
    y = random(0, MAX_Y);
    x = random(0, MAX_X);
    board[y][x] = state;
  }
}


// standalone method that can be called in loop
void randomDisplay() {
  set_random_point(1, 1);
  set_random_point(0, 2);
  delay(100);
}


// repeate should be greater than the number of points to set
void set_random_point_near_neighbor(int state, int repeat) {
  int x = 0;
  int y = 0;
  int points = 4;

  int sum = sumBoard();

  // set a couple of random points if board is empty
  if (sum == 0) {
    for (int i = 0; i < points; i++) {
      y = random(0, MAX_Y);
      x = random(0, MAX_X);
      board[y][x] = state;
      mx.setPoint(y, x, state);
    }
    sum = points;
  }

  // set points only near neighbors
  int i = 0;
  int neighbors = 0;
  while (i < points * repeat) {
    y = random(0, MAX_Y);
    x = random(0, MAX_X);
    if (neighborsOnFire(y, x) > 0) {
      board[y][x] = state;
      mx.setPoint(y, x, state);
      i++;
      delay(50);
    }

  }
}

void set_random_point(int state, int repeat) {
  int x = 0;
  int y = 0;
  for (int i = 0; i < repeat; i++) {
    y = random(0, MAX_Y);
    x = random(0, MAX_X);
    board[y][x] = state;
    mx.setPoint(y, x, state);
  }
}

void printLine(char* str, int value) {
  if (DEBUG) {
    Serial.print(str);
    Serial.println(value);
  }
}


void printit(char* str, int value) {
  if (DEBUG) {
    Serial.print(str);
    Serial.print(value);
  }
}

// debug board
void printBoard() {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      Serial.print(board[y][x]);
      Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.println("##########################################################################################");
}

void printBoardValues() {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      Serial.print(y);
      Serial.print(":");
      Serial.print(x);
      Serial.print(" = ");
      Serial.println(board[y][x]);
    }
  }
}
