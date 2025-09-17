#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <TFT_eSPI.h>
#include "pins.h" // Inclui nossas definições de pinos
#include "SoundManager.h" 

// O struct Point e os enums são usados pela classe, então os declaramos aqui.
typedef struct {
    int x, y;
} Point;

// O enum para controlar nossa máquina de estados
enum GameState {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_GAME_OVER
};
enum Cell { EMPTY, WALL, SNAKE, APPLE };
enum Direction { UP, DOWN, LEFT, RIGHT };


class SnakeGame {
public:
    // Construtor: é chamado quando criamos um objeto SnakeGame.
    // Ele recebe um ponteiro para o display (tft) que já foi inicializado.
    SnakeGame(TFT_eSPI* tft_display, SoundManager* sound_manager);

    // Funções públicas que o .ino principal irá chamar
    void setup(); // Prepara o jogo (substitui a antiga initGame)
    void loop();  // Executa um ciclo/frame do jogo

private:
    // --- VARIÁVEIS DE MEMBRO (antigas variáveis globais) ---
    TFT_eSPI* tft; // Ponteiro para o objeto tft principal
    SoundManager* sound; // Adiciona um ponteiro para o gerenciador de som

    // Constantes do jogo
    static const int DISPLAY_WIDTH = 320;
    static const int DISPLAY_HEIGHT = 170;
    static const int SPEED = 200;
    static const int CELL_SIZE = 8;
    static const int Y_OFFSET = CELL_SIZE;
    static const int GRID_WIDTH = DISPLAY_WIDTH / CELL_SIZE;
    static const int GRID_HEIGHT = (DISPLAY_HEIGHT - Y_OFFSET) / CELL_SIZE;

    // Estado do jogo
    int snakeLength;
    int grid[GRID_WIDTH][GRID_HEIGHT];
    Point snake[GRID_HEIGHT * GRID_WIDTH];
    bool gameOver;
    bool paused;
    bool lastStartState; 
    Direction currentDirection;
    Direction pendingDirection;
    unsigned long now;
    unsigned long lastUpdate;
    int score;

GameState currentState;
    unsigned long gameOverStartTime; // Para o timer não-bloqueante

    // --- FUNÇÕES PRIVADAS (antigas funções globais) ---
    // Estas são as funções internas do jogo, que não precisam ser chamadas de fora.
    void update();
    void drawGrid();
    void titleScreen();
    void gameOverScreen();
    void spawnApple();
    void readInput();
    void buzz();
    void drawScoreHUD();
};

#endif // SNAKEGAME_H