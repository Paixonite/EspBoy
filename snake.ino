//LIBRARIES
  #include <TFT_eSPI.h>  // Display library
  #include <SPI.h>

  TFT_eSPI tft = TFT_eSPI();

//CONSTANTS

  // Time in miliseconds between frames
  #define SPEED 100

  #define TILE_SIZE 8
  #define GRID_WIDTH (320 / TILE_SIZE)
  #define GRID_HEIGHT (170 / TILE_SIZE)

//STRUCTS

  typedef struct{
    int x, y;
  } Point;

//ENUMS

  enum Tile{
    EMPTY,
    WALL,
    SNAKE,
    APPLE
  };

  enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
  };

// GLOBAL VARIABLES
  
  int snakeLength;
  int grid [GRID_HEIGHT][GRID_WIDTH];
  
  Point snake[100];
  Point apple;

  bool gameOver;

  Direction currentDirection;

// SETUP

  void setup() {
    tft.init();
    tft.setRotation(1);
    Serial.begin(115200);
    Serial.println("Game started.");
    initGame();
  }

// LOOP

  void loop() {
    if(gameOver){
      gameOverScreen();
      delay(2000);
      initGame();
    }

    readInput();
    update();
    delay(SPEED);

    titleScreen();
  }

// FUNCTION

  void titleScreen(){
    String msg;
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);

    const char* countdown[] = { "Get ready!", "3...", "2...", "1...", "GO!" };
    for (int i = 0; i < 5; ++i) {
      msg = countdown[i];
      tft.drawString(msg, tft.width() / 2, tft.height() / 2);
      delay(1000);
      tft.fillScreen(TFT_BLACK);
    }
  }

  void initGame(){
    gameOver = false;
    snakeLength = 4;
    currentDirection = RIGHT;

    // Initializes grid
    for (int i = 0; i < GRID_HEIGHT; ++i)
      for (int j = 0; j < GRID_WIDTH; ++j)
          grid[i][j] = EMPTY;

    // Sets snake (is this right?)
    for (int i = 0; i < snakeLength; i++) {
      snake[i].x = GRID_WIDTH / 2 - i;
      snake[i].y = GRID_HEIGHT / 2;
      grid[snake[i].y][snake[i].x] = SNAKE;
    }

    // Sets walls
    for (int i = 0; i < GRID_HEIGHT; ++i){
        grid[i][0] = WALL;
        grid[i][GRID_WIDTH - 1] = WALL;    
    }
    for (int i = 0; i < GRID_WIDTH; ++i){
        grid[0][i] = WALL;
        grid[GRID_HEIGHT - 1][i] = WALL; 
    }

    apple = spawnApple();

    titleScreen();
  }

  void update(){};

  void gameOverScreen(){
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("GAME OVER", tft.width() / 2, tft.height() / 2);
  };

  Point spawnApple(){
    Point a;
    do {
      a.x = random(1, GRID_HEIGHT - 1);
      a.y = random(1, GRID_WIDTH - 1);
    } while (grid[a.x][a.y] != EMPTY);

    grid[a.x][a.y] = APPLE;
    return a;
  }

  void readInput() {
    if (Serial.available()) {
      char key = Serial.read();
      switch (key) {
        case 'w': currentDirection = UP; break;
        case 's': currentDirection = DOWN; break;
        case 'a': currentDirection = LEFT; break;
        case 'd': currentDirection = RIGHT; break;
      }
    }
  }
