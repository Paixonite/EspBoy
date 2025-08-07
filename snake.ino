//LIBRARIES
  #include <TFT_eSPI.h>  // Display library
  #include <SPI.h>

//CONSTANTS

  // Time in miliseconds between frames
  #define SPEED 75

  // This should be calculated from the display size
  #define TILE_SIZE 8
  #define GRID_WIDTH (tft.width() / TILE_SIZE)
  #define GRID_HEIGHT (tft.height() / TILE_SIZE)

  #define SNAKE_HEAD_CHAR 'Q' 
  #define SNAKE_BODY_CHAR 'o' // Code 1
  #define BOARD_CHAR ' ' // Code 0
  #define APPLE_CHAR '@' // Code 2
  #define WALL_CHAR '#'  // Code -1

//STRUCTS

  struct snakeSegment{
    int x, y;
  };

  struct sApple{
    int x, y;
  };

//GLOBAL VARIABLES  

  int board [BOARD_HEIGHT][BOARD_WIDTH];

//PROTOTYPES

  //...

// SETUP

  TFT_eSPI tft = TFT_eSPI();  // Usa o setup ativado

  void setup() {
    tft.init();
    tft.setRotation(1);           // Rotação horizontal

    titleScreen();

    Serial.begin(115200);
    Serial.println("Hello World no Serial Monitor!");
  }

// LOOP

  void loop() {
    //Nada
  }

// FUNCTION

  void titleScreen(){
    int16_t x, y;
    uint16_t w, h;
    String msg;

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);


    // Get text bounds (calculates width and height)
    tft.setTextDatum(MC_DATUM); // MC = Middle Center

    msg = "Get ready!";
    tft.drawString(msg, tft.width()/2, tft.height()/2);
    delay(1000);
    tft.fillScreen(TFT_BLACK);

    msg = "3...";
    tft.drawString(msg, tft.width()/2, tft.height()/2);
    delay(1000);
    tft.fillScreen(TFT_BLACK);

    msg = "2...";
    tft.drawString(msg, tft.width()/2, tft.height()/2);
    delay(1000);
    tft.fillScreen(TFT_BLACK);

    msg = "1...";
    tft.drawString(msg, tft.width()/2, tft.height()/2);
    delay(1000);
    tft.fillScreen(TFT_BLACK);

    msg = "GO!";
    tft.drawString(msg, tft.width()/2, tft.height()/2);
    delay(500);
    //tft.fillScreen(TFT_BLACK);
  }
