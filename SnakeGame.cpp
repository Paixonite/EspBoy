#include "SnakeGame.h" // Inclui a definição da nossa classe
#include "audio_assets.h"

// Construtor: Apenas inicializa as variáveis de membro.
// A sintaxe ": tft(tft_display)" é uma "initializer list", mais eficiente em C++.
SnakeGame::SnakeGame(TFT_eSPI* tft_display, SoundManager* sound_manager) 
    : tft(tft_display), sound(sound_manager) {
    // O construtor pode ficar vazio se tudo for inicializado no setup.
}

// Renomeamos initGame para setup() para manter o padrão.
void SnakeGame::setup() {
    lastStartState = LOW; 
    score = 0;
    gameOver = false;
    paused = false;
    snakeLength = 4;
    currentDirection = RIGHT;
    pendingDirection = currentDirection;
    lastUpdate = 0;

    for (int x = 0; x < GRID_WIDTH; x++)
        for (int y = 0; y < GRID_HEIGHT; y++)
            grid[x][y] = EMPTY;

    for (int i = 0; i < snakeLength; i++) {
        snake[i].x = (GRID_WIDTH / 2) - i;
        snake[i].y = (GRID_HEIGHT / 2);
        grid[snake[i].x][snake[i].y] = SNAKE;
    }

    for (int y = 0; y < GRID_HEIGHT; y++) {
        grid[0][y] = WALL;
        grid[GRID_WIDTH - 1][y] = WALL;
    }
    for (int x = 0; x < GRID_WIDTH; x++) {
        grid[x][0] = WALL;
        grid[x][GRID_HEIGHT - 1] = WALL;
    }

    spawnApple();
    // Define o estado inicial do jogo
    currentState = STATE_TITLE;
    titleScreen();
}

// A antiga função loop() global agora é o loop da classe.
void SnakeGame::loop() {
 // O loop agora é um roteador que executa código baseado no estado atual
    switch (currentState) {
        
        case STATE_TITLE:
            // No estado de título, apenas esperamos o botão START ser pressionado
            if (digitalRead(BTN_START) == HIGH) { //quebrado?
                // Ao pressionar, mudamos para o estado de jogo
                currentState = STATE_PLAYING;
                paused = true; // Gambiarra pro jogo não começar pausado
                drawGrid(); // Desenha o grid inicial do jogo
            }
            break;

        case STATE_PLAYING:
            // Este é o loop de jogo normal
            readInput();

            now = millis();
            if (!paused && now - lastUpdate >= SPEED) {
                update();
                lastUpdate = now;
            }
            break;

        case STATE_GAME_OVER:
            // No estado de game over, esperamos 3 segundos sem bloquear
            if (millis() - gameOverStartTime > 3000) {
                // Após 3 segundos, reinicia o jogo chamando setup()
                setup(); // setup() já define o estado para STATE_TITLE
            }
            break;
    }
}

// --- A PARTIR DAQUI, APENAS COPIE E COLE SUAS FUNÇÕES ORIGINAIS ---
// A única mudança é adicionar "SnakeGame::" na frente de cada uma
// e trocar "tft." por "tft->".

void SnakeGame::update() {
    grid[snake[snakeLength - 1].x][snake[snakeLength - 1].y] = EMPTY;

    tft->fillRect(snake[snakeLength - 1].x * CELL_SIZE, snake[snakeLength - 1].y * CELL_SIZE + Y_OFFSET, CELL_SIZE, CELL_SIZE, TFT_BLACK);

    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    int dx = 0, dy = 0;
    currentDirection = pendingDirection;
    switch (currentDirection) {
        case UP: dy = -1; break;
        case DOWN: dy = 1; break;
        case LEFT: dx = -1; break;
        case RIGHT: dx = 1; break;
    }

    int newX = snake[0].x + dx;
    int newY = snake[0].y + dy;

    int collided = grid[newX][newY];
    if (collided == SNAKE || collided == WALL) {
        // ---- MUDANÇA IMPORTANTE ----
        // Em vez de 'gameOver = true;', mudamos o estado do jogo
        currentState = STATE_GAME_OVER;
        gameOverStartTime = millis(); // Inicia o timer do game over
        sound->play(MELODY_GAME_OVER, MELODY_GAME_OVER_LENGTH); // Toca a música
        gameOverScreen(); // Desenha a tela de game over
        return; // Sai da função update
    }

    if (collided == APPLE) {
        snakeLength++;
        snake[snakeLength - 1] = snake[snakeLength - 2];
        spawnApple();
        score++;
        sound->play(MELODY_APPLE, MELODY_APPLE_LENGTH);
    }

    snake[0].x = newX;
    snake[0].y = newY;
    grid[newX][newY] = SNAKE;

    tft->fillRect(snake[0].x * CELL_SIZE, snake[0].y * CELL_SIZE + Y_OFFSET, CELL_SIZE, CELL_SIZE, TFT_GREEN);

    drawScoreHUD();
}

void SnakeGame::drawGrid() {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            uint16_t color;
            if (grid[x][y] == EMPTY) color = TFT_BLACK;
            else if (grid[x][y] == SNAKE) color = TFT_GREEN;
            else if (grid[x][y] == APPLE) color = TFT_RED;
            else if (grid[x][y] == WALL) color = TFT_WHITE;
            tft->fillRect(x * CELL_SIZE, y * CELL_SIZE + Y_OFFSET, CELL_SIZE, CELL_SIZE, color);
        }
    }
    tft->fillRect(0, 0, tft->width(), Y_OFFSET, TFT_BLACK);
    drawScoreHUD();
}

void SnakeGame::titleScreen() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_YELLOW);
    tft->setTextSize(2);
    tft->setTextDatum(MC_DATUM);
    tft->drawString("Press START", tft->width() / 2, tft->height() / 2);
    // while (digitalRead(BTN_START) == LOW) {
    //     delay(10);
    // }

    // // NOVO: Espera o botão START ser solto para evitar a pausa imediata
    // while (digitalRead(BTN_START) == HIGH) {
    //     delay(10);
    // }
}

void SnakeGame::gameOverScreen() {
    sound->play(MELODY_GAME_OVER, MELODY_GAME_OVER_LENGTH);
    tft->fillScreen(TFT_RED);
    tft->setTextColor(TFT_WHITE);
    tft->setTextSize(2);
    tft->setTextDatum(MC_DATUM);
    tft->drawString("GAME OVER", tft->width() / 2, tft->height() / 2 - 12);
    String s = "Score: " + String(score);
    tft->drawString(s, tft->width() / 2, tft->height() / 2 + 12);
}

void SnakeGame::spawnApple() {
    Point apple;
    do {
        apple.x = random(1, GRID_WIDTH - 1);
        apple.y = random(1, GRID_HEIGHT - 1);
    } while (grid[apple.x][apple.y] != EMPTY);
    grid[apple.x][apple.y] = APPLE;
    tft->fillRect(apple.x * CELL_SIZE, apple.y * CELL_SIZE + Y_OFFSET, CELL_SIZE, CELL_SIZE, TFT_RED);
}

void SnakeGame::readInput() {
    if (digitalRead(BTN_UP) == HIGH && currentDirection != DOWN) pendingDirection = UP;
    if (digitalRead(BTN_DOWN) == HIGH && currentDirection != UP) pendingDirection = DOWN;
    if (digitalRead(BTN_LEFT) == HIGH && currentDirection != RIGHT) pendingDirection = LEFT;
    if (digitalRead(BTN_RIGHT) == HIGH && currentDirection != LEFT) pendingDirection = RIGHT;

    // 1. Lê o estado atual do botão
    bool currentStartState = digitalRead(BTN_START);

    // 2. Verifica se a borda de descida ocorreu (estava pressionado E agora está solto)
    if (lastStartState == HIGH && currentStartState == LOW) {
        // Ação de Pausa/Despausa
        paused = !paused;

        // Lógica para desenhar/limpar a tela de pausa
        if (paused && !gameOver) {
            tft->fillRect(0, 0, tft->width(), Y_OFFSET, TFT_BLACK);
            tft->setTextColor(TFT_YELLOW, TFT_BLACK);
            tft->setTextSize(1);
            tft->setTextDatum(MC_DATUM);
            String s = "Pausado - Score: " + String(score);
            tft->drawString(s, tft->width() / 2, Y_OFFSET / 2);
        } else if (!gameOver) {
            drawScoreHUD();
        }
    }

    // 3. Atualiza o estado anterior para o próximo ciclo do loop
    lastStartState = currentStartState;
}

void SnakeGame::drawScoreHUD() {
    tft->fillRect(0, 0, tft->width(), Y_OFFSET, TFT_BLACK);
    tft->setTextColor(TFT_YELLOW, TFT_BLACK);
    tft->setTextSize(1);
    tft->setTextDatum(MC_DATUM);
    String s = "Score: " + String(score);
    tft->drawString(s, tft->width() / 2, Y_OFFSET / 2);
}