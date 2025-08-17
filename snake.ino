// LIBRARIES
#include <TFT_eSPI.h>  // Display library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

// CONSTANTS
#define SPEED 200  // Time in ms between frames
#define CELL_SIZE 8
#define GRID_WIDTH  (320 / CELL_SIZE)
#define GRID_HEIGHT (170 / CELL_SIZE)

#define BTN_UP     1
#define BTN_DOWN   16
#define BTN_LEFT   21
#define BTN_RIGHT  3

// STRUCTS
typedef struct {
  int x, y;
} Point;

// ENUMS
enum Cell {
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
int grid[GRID_WIDTH][GRID_HEIGHT]; // Agora X antes de Y
Point snake[100];
bool gameOver;
Direction currentDirection;
Direction pendingDirection;
unsigned long now;
unsigned long lastUpdate;

// SETUP
void setup() {
  tft.init();
  tft.setRotation(1);
  Serial.begin(115200);
  Serial.println("Game started.");

  lastUpdate = 0;

  pinMode(BTN_UP, INPUT_PULLDOWN);
  pinMode(BTN_DOWN, INPUT_PULLDOWN);
  pinMode(BTN_LEFT, INPUT_PULLDOWN);
  pinMode(BTN_RIGHT, INPUT_PULLDOWN);

  initGame();
}

// LOOP
void loop() {
  if (gameOver) {
    gameOverScreen();
    delay(3000);
    initGame();
  }

  readInput();

  now = millis();
  if (now - lastUpdate >= SPEED) {
    update();
    lastUpdate = now;
  }
}

// INIT GAME
void initGame() {
  gameOver = false;
  snakeLength = 4;
  currentDirection = RIGHT;
  pendingDirection = RIGHT;

  // Inicializa grid
  for (int x = 0; x < GRID_WIDTH; x++)
    for (int y = 0; y < GRID_HEIGHT; y++)
      grid[x][y] = EMPTY;

  // Define cobra no centro
  for (int i = 0; i < snakeLength; i++) {
    snake[i].x = GRID_WIDTH / 2 - i;
    snake[i].y = GRID_HEIGHT / 2;
    grid[snake[i].x][snake[i].y] = SNAKE;
  }

  // Paredes
  for (int y = 0; y < GRID_HEIGHT; y++) {
    grid[0][y] = WALL;
    grid[GRID_WIDTH - 1][y] = WALL;
  }
  for (int x = 0; x < GRID_WIDTH; x++) {
    grid[x][0] = WALL;
    grid[x][GRID_HEIGHT - 1] = WALL;
  }

  spawnApple();
  titleScreen();
  drawGrid();
}

// UPDATE
void update() {
  // Apaga a cauda
  grid[snake[snakeLength - 1].x][snake[snakeLength - 1].y] = EMPTY;

  tft.fillRect(
    snake[snakeLength - 1].x * CELL_SIZE,
    snake[snakeLength - 1].y * CELL_SIZE,
    CELL_SIZE,
    CELL_SIZE,
    TFT_BLACK
  );
  
  // Move corpo
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  // Calcula a posição da cabeça
  int dx = 0, dy = 0;
  currentDirection = pendingDirection;
  switch (currentDirection) {
    case UP:    dy = -1; break;
    case DOWN:  dy =  1; break;
    case LEFT:  dx = -1; break;
    case RIGHT: dx =  1; break;
  }

  int newX = snake[0].x + dx;
  int newY = snake[0].y + dy;

  // Verifica colisão
  int collided = grid[newX][newY];
  if (collided == SNAKE || collided == WALL) {
    gameOver = true;
    return;
    // todo: um frame de coyote time
  }

  if (collided == APPLE) {
    snakeLength++;
    snake[snakeLength - 1] = snake[snakeLength - 2];
    spawnApple();
  }

  // Move cabeça
  snake[0].x = newX;
  snake[0].y = newY;
  grid[newX][newY] = SNAKE;

  // Desenha cabeça
  tft.fillRect(
    snake[0].x * CELL_SIZE,
    snake[0].y * CELL_SIZE,
    CELL_SIZE,
    CELL_SIZE,
    TFT_GREEN
  );
}

// DRAW GRID
void drawGrid() {
  for (int x = 0; x < GRID_WIDTH; x++) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
      uint16_t color;
      if (grid[x][y] == EMPTY)      color = TFT_BLACK;
      else if (grid[x][y] == SNAKE) color = TFT_GREEN;
      else if (grid[x][y] == APPLE) color = TFT_RED;
      else if (grid[x][y] == WALL)  color = TFT_WHITE;

      tft.fillRect(
        x * CELL_SIZE,
        y * CELL_SIZE,
        CELL_SIZE,
        CELL_SIZE,
        color
      );
    }
  }
}

// TITLE SCREEN
void titleScreen() {
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

// GAME OVER SCREEN
void gameOverScreen() {
  tft.fillScreen(TFT_RED);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("GAME OVER", tft.width() / 2, tft.height() / 2);
}

// SPAWN APPLE
void spawnApple() {
  Point apple;
  do {
    apple.x = random(1, GRID_WIDTH - 1);
    apple.y = random(1, GRID_HEIGHT - 1);
  } while (grid[apple.x][apple.y] != EMPTY);

  grid[apple.x][apple.y] = APPLE;

  // Desenha maçã
  tft.fillRect(
    apple.x * CELL_SIZE,
    apple.y * CELL_SIZE,
    CELL_SIZE,
    CELL_SIZE,
    TFT_RED
  );
}

// READ INPUT
void readInput() {
  if (digitalRead(BTN_UP) == HIGH && currentDirection != DOWN) {
    pendingDirection = UP;
    //Serial.println("UP");
  }
  if (digitalRead(BTN_DOWN) == HIGH && currentDirection != UP) {
    pendingDirection = DOWN;
    //Serial.println("DOWN");
  }
  if (digitalRead(BTN_LEFT) == HIGH && currentDirection != RIGHT) {
    pendingDirection = LEFT;
//Serial.println("LEFT");
  }
  if (digitalRead(BTN_RIGHT) == HIGH && currentDirection != LEFT) {
    pendingDirection = RIGHT;
    //Serial.println("RIGHT");
  }
}
