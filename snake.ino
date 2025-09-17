// LIBRARIES
#include <TFT_eSPI.h>  // Display library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

// CONSTANTS
// The display resolution must me manually placed
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 170

#define SPEED 200  // Time in ms between frames

#define CELL_SIZE 8

#define GRID_WIDTH  (DISPLAY_WIDTH / CELL_SIZE)
#define GRID_HEIGHT (DISPLAY_HEIGHT / CELL_SIZE)

#define Y_OFFSET CELL_SIZE  // 1 tile = 8 px vertical offset

#define BTN_UP     1
#define BTN_RIGHT  2
#define BTN_LEFT   3
#define BTN_DOWN   43
#define BTN_SELECT 44
#define BTN_START  18
#define BTN_A      17
#define BTN_B      16
#define BUZZER_PIN 21

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
int grid[GRID_WIDTH][GRID_HEIGHT];
Point snake[GRID_HEIGHT * GRID_WIDTH];
bool paused;
bool gameOver;

Direction currentDirection;
Direction pendingDirection;

// Used for frame counting
unsigned long now;
unsigned long lastUpdate;

int score;

int buzzFrames;

// SETUP
void setup() {
  // Init tft display
  tft.init();
  tft.setRotation(1);

  // Init serial monitor
  Serial.begin(115200);
  Serial.println("Game started.");

  lastUpdate = 0;

  // Setup pins
  pinMode(BTN_UP, INPUT_PULLDOWN);
  pinMode(BTN_DOWN, INPUT_PULLDOWN);
  pinMode(BTN_LEFT, INPUT_PULLDOWN);
  pinMode(BTN_RIGHT, INPUT_PULLDOWN);
  pinMode(BTN_START, INPUT_PULLDOWN);
  pinMode(BTN_SELECT, INPUT_PULLDOWN);
  pinMode(BTN_A, INPUT_PULLDOWN);
  pinMode(BTN_B, INPUT_PULLDOWN);
  pinMode(BUZZER_PIN, OUTPUT);  // Buzzer out

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
  if (!paused && now - lastUpdate >= SPEED) {
    update();
    lastUpdate = now;
    buzz();
  }
}

// INIT GAME
void initGame() {
  score = 0;
  gameOver = false;
  paused = true; // that because tha games starts with a start press
  snakeLength = 4;
  currentDirection = RIGHT;
  pendingDirection = currentDirection;

  // Inicializa grid
  for (int x = 0; x < GRID_WIDTH; x++)
    for (int y = 0; y < GRID_HEIGHT; y++)
      grid[x][y] = EMPTY;

  // Define cobra no centro
  for (int i = 0; i < snakeLength; i++) {
    snake[i].x = (GRID_WIDTH / 2) - i;
    snake[i].y = (GRID_HEIGHT / 2); // +1, já que a grid é ímpar
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
    snake[snakeLength - 1].y * CELL_SIZE + Y_OFFSET,
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
    buzzFrames++;
    score++;
  }

  // Move cabeça
  snake[0].x = newX;
  snake[0].y = newY;
  grid[newX][newY] = SNAKE;

  // Desenha cabeça
  tft.fillRect(
    snake[0].x * CELL_SIZE,
    snake[0].y * CELL_SIZE + Y_OFFSET,
    CELL_SIZE,
    CELL_SIZE,
    TFT_GREEN
  );

  drawScoreHUD();
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
        y * CELL_SIZE + Y_OFFSET,
        CELL_SIZE,
        CELL_SIZE,
        color
      );
    }
  }
  tft.fillRect(0, 0, tft.width(), Y_OFFSET, TFT_BLACK); // clear top bar

  drawScoreHUD();
}

// TITLE SCREEN
void titleScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Press START", tft.width() / 2, tft.height() / 2);

  // Espera pelo botão START
  while (digitalRead(BTN_START) == LOW) {
    delay(10);
  }
}

// GAME OVER SCREEN
void gameOverScreen() {
  tft.fillScreen(TFT_RED);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);


  tft.drawString("GAME OVER", tft.width() / 2, tft.height() / 2 - 12);
  String s = "Score: " + String(score);
  tft.drawString(s, tft.width() / 2, tft.height() / 2 + 12);
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
    apple.y * CELL_SIZE + Y_OFFSET,
    CELL_SIZE,
    CELL_SIZE,
    TFT_RED
  );
}

// READ INPUT
void readInput() {
  // Arrow keys
  if (digitalRead(BTN_UP) == HIGH && currentDirection != DOWN) {
    pendingDirection = UP;
  }
  if (digitalRead(BTN_DOWN) == HIGH && currentDirection != UP) {
    pendingDirection = DOWN;
  }
  if (digitalRead(BTN_LEFT) == HIGH && currentDirection != RIGHT) {
    pendingDirection = LEFT;
  }
  if (digitalRead(BTN_RIGHT) == HIGH && currentDirection != LEFT) {
    pendingDirection = RIGHT;
  }

  // Start and Select
  static bool startPressed = false; // Para detectar borda de subida
  static bool selectPressed = false; // Para detectar borda de subida

  if (digitalRead(BTN_START) == HIGH) {
    if (!startPressed && !gameOver) { // só entra uma vez por clique
      startPressed = true;
      paused = !paused;

      if (paused) {
        // Desenha tela de pausa
        tft.fillRect(0, 0, tft.width(), Y_OFFSET, TFT_BLACK); // limpa barra de cima
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.setTextSize(1);
        tft.setTextDatum(MC_DATUM);
        String s = "Pausado - Score: " + String(score);
        tft.drawString(s, tft.width() / 2, Y_OFFSET / 2);
      } else {
        // Sai da pausa: redesenha HUD
        drawScoreHUD();
      }
    }
  } else {
    startPressed = false; // libera para detectar o próximo clique
  } 

  if (digitalRead(BTN_SELECT) == HIGH) {
    //do something
  }

  // A and B
  if (digitalRead(BTN_A) == HIGH) {
    //do something
  }
  if (digitalRead(BTN_B) == HIGH) {
    //do something
  }

}

//BUZZ
void buzz(){
  // Buzzes for one frame. It i'll keep dong it until buzzFrames is 0
  if(buzzFrames > 0){
    digitalWrite(BUZZER_PIN, HIGH);
    buzzFrames--;
  }else{
    digitalWrite(BUZZER_PIN, LOW);
  }
}

//DRAW HUD
void drawScoreHUD() {
  tft.fillRect(0, 0, tft.width(), Y_OFFSET, TFT_BLACK); // limpa barra de cima
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  String s = "Score: " + String(score);
  tft.drawString(s, tft.width() / 2, Y_OFFSET / 2); // centered in top bar
}
