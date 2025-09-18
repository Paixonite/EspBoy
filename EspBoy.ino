// LIBRARIES
#include <TFT_eSPI.h>
#include <SPI.h>
#include "Game.h" 
#include "pins.h"
#include "audio_assets.h"
#include "SoundManager.h"
#include "SnakeGame.h"
// Futuramente: #include "TetrisGame.h"

 // These TFT colors are originally very bad
#define MY_ORANGE tft.color565(255, 165, 0)
#define MY_GREY tft.color565(50, 50, 50)

// --- ESTADOS DO SISTEMA ---

enum SystemState {
  STATE_SPLASH,
  STATE_MENU,
  STATE_IN_GAME
};
SystemState currentSystemState;


// --- OBJETOS GLOBAIS DO SISTEMA ---
TFT_eSPI tft = TFT_eSPI();
SoundManager soundManager(BUZZER_PIN);

// --- LÓGICA DA SPLASH SCREEN ---
const int SPLASH_DURATION = 3000; // Duração em milissegundos
unsigned long splashStartTime;

// --- LÓGICA DO MENU ---
const char* gameNames[] = {"Snake", "Tetris", "Pac-Man"};
const int totalGames = sizeof(gameNames) / sizeof(char*);
int selectedGameIndex = 0;


// --- GERENCIADOR DE JOGO ATUAL ---
Game* currentGame = nullptr;


// --- VARIÁVEIS PARA INPUT NÃO-BLOQUEANTE DO MENU ---
bool lastLeftState = LOW;
bool lastRightState = LOW;
bool lastAState = LOW;
bool lastBState = LOW;

// =================================================================
// FUNÇÕES DO MENU
// =================================================================

// Desenha a Splash Screen
void drawSplashScreen() {
  tft.fillScreen(TFT_BLACK);

  // --- DESENHA "EspBoy" COM LETRAS COLORIDAS ---
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(5); // Fonte grande

  String title = "EspBoy";
  uint16_t rainbowColors[] = {TFT_RED, MY_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_BLUE, TFT_PURPLE};
  
  // Calcula a largura total da palavra para centralizar
  int totalWidth = tft.textWidth(title);
  int cursorX = (tft.width() / 2) - (totalWidth / 2);
  int cursorY = tft.height() / 2 - 10;

  // Itera sobre cada letra para desenhar com uma cor diferente
  for (int i = 0; i < title.length(); i++) {
    String letter = String(title.charAt(i));
    tft.setTextColor(rainbowColors[i % 6]); // Usa o array de cores
    
    // Desenha a letra e avança o cursor
    // Usamos ML_DATUM para alinhar a letra à esquerda do ponto do cursor
    tft.setTextDatum(ML_DATUM);
    tft.drawString(letter, cursorX, cursorY);
    cursorX += tft.textWidth(letter);
  }

  // --- DESENHA "by Paixonite" EMBAIXO ---
  tft.setTextDatum(MC_DATUM); // Volta a centralizar
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2); // Fonte menor
  tft.drawString("by Paixonite", tft.width() / 2, tft.height() / 2 + 30);
}

// Desenha a tela de seleção de jogos
void drawMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("SELECT A GAME", tft.width() / 2, tft.height() / 4);

  // Desenha o nome do jogo selecionado com setas
  String gameText = "< " + String(gameNames[selectedGameIndex]) + " >";
  tft.setTextColor(TFT_WHITE);
  tft.drawString(gameText, tft.width() / 2, tft.height() / 2);
}

// Lida com os botões na tela de menu
void handleMenuInput() {
  bool currentLeftState = digitalRead(BTN_LEFT);
  bool currentRightState = digitalRead(BTN_RIGHT);
  bool currentAState = digitalRead(BTN_A);
  bool currentBState = digitalRead(BTN_B);

  // --- LÓGICA PARA ATIVAR/DESATIVAR O SOM ---
  // (Apertando Select + B no menu)
  if (digitalRead(BTN_SELECT) == HIGH && currentBState == HIGH) {
    // Para evitar que a ação dispare várias vezes, usamos a detecção de borda no botão B
    if (lastBState == LOW) { 
      soundManager.setMute(!soundManager.isMuted()); // Alterna o estado mudo

      // Feedback visual (opcional, mas recomendado)
      tft.setTextColor(TFT_RED);
      tft.setTextSize(1);
      tft.setTextDatum(BR_DATUM); // Alinha no canto inferior direito
      if (soundManager.isMuted()) {
        tft.drawString("Sound OFF", tft.width() - 5, tft.height() - 5);
        delay(500);
        drawMenu(); // Redesenha o menu para apagar a mensagem
      } else {
        tft.drawString("Sound ON", tft.width() - 5, tft.height() - 5);
        delay(500);
        drawMenu();
      }
    }
  }

  // --- NAVEGAÇÃO PARA A ESQUERDA ---
  if (lastLeftState == HIGH && currentLeftState == LOW) {
    selectedGameIndex--;
    if (selectedGameIndex < 0) {
      selectedGameIndex = totalGames - 1; // Volta para o final
    }
    soundManager.play(MELODY_MENU_NAVIGATE, MELODY_MENU_NAVIGATE_LENGTH);
    tft.fillScreen(MY_GREY); // Efeito visual de "flash"
    delay(40); // Duração do flash
    drawMenu();
  }

  // --- NAVEGAÇÃO PARA A DIREITA ---
  if (lastRightState == HIGH && currentRightState == LOW) {
    selectedGameIndex++;
    if (selectedGameIndex >= totalGames) {
      selectedGameIndex = 0; // Volta para o início
    }
    soundManager.play(MELODY_MENU_NAVIGATE, MELODY_MENU_NAVIGATE_LENGTH);
    tft.fillScreen(MY_GREY); // Efeito visual de "flash"
    delay(40); // Duração do flash
    drawMenu();
  }

  // --- SELEÇÃO COM BOTÃO 'A' ---
  if (lastAState == HIGH && currentAState == LOW) {
    switch (selectedGameIndex) {
      case 0: // Snake
        // Deleta o jogo antigo se existir, para liberar memória
        if (currentGame != nullptr) delete currentGame;
        // Cria e inicia o novo jogo
        currentGame = new SnakeGame(&tft, &soundManager);
        break;
        
      case 1: // Tetris (Placeholder)
      case 2: // Pac-Man (Placeholder)
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Not Implemented", tft.width() / 2, tft.height() / 2);
        delay(1500);
        drawMenu();
        break;
    }

    if (selectedGameIndex == 0) { // Ou outros jogos, futuramente
      currentGame->setup();
      currentSystemState = STATE_IN_GAME;
    }
  }

  // Atualiza o estado dos botões para o próximo ciclo
  lastLeftState = currentLeftState;
  lastRightState = currentRightState;
  lastAState = currentAState;
  lastBState = currentBState;
}


// =================================================================
// SETUP E LOOP PRINCIPAIS
// =================================================================

void setup() {
  // Inicialização do hardware
  tft.init();
  tft.setRotation(1);
  Serial.begin(115200);

  // Configuração dos pinos (boa prática manter aqui)
  pinMode(BTN_UP, INPUT_PULLDOWN);
  pinMode(BTN_DOWN, INPUT_PULLDOWN);
  pinMode(BTN_LEFT, INPUT_PULLDOWN);
  pinMode(BTN_RIGHT, INPUT_PULLDOWN);
  pinMode(BTN_START, INPUT_PULLDOWN);
  pinMode(BTN_SELECT, INPUT_PULLDOWN);
  pinMode(BTN_A, INPUT_PULLDOWN);
  pinMode(BTN_B, INPUT_PULLDOWN);
  pinMode(BUZZER_PIN, OUTPUT);

  // --- INICIALIZAÇÃO DO SISTEMA ---
  currentSystemState = STATE_SPLASH;
  drawSplashScreen();
  splashStartTime = millis();
}

void loop() {
  // O soundManager deve sempre ser atualizado, não importa o estado.
  soundManager.loop();

  // Roteia o controle com base no estado atual do sistema
  switch (currentSystemState) {
    case STATE_SPLASH:
      // Fica neste estado até o tempo acabar
      if (millis() - splashStartTime > SPLASH_DURATION) {
        currentSystemState = STATE_MENU; // Muda para o estado de Menu
        drawMenu(); // Desenha o menu pela primeira vez
      }
      break;

    case STATE_MENU:
      handleMenuInput();
      break;

    case STATE_IN_GAME:
      if (currentGame != nullptr) {
        currentGame->loop();

        if (currentGame->shouldExit()) {
          delete currentGame;
          currentGame = nullptr;
          currentSystemState = STATE_MENU;
          drawMenu();
        }
      }
      break;
  }
}