// LIBRARIES
#include <TFT_eSPI.h>
#include <SPI.h>
#include "pins.h"            // Nossas definições de pinos
#include "SoundManager.h" 
#include "SnakeGame.h" // Nossa nova biblioteca de jogo

// OBJETOS GLOBAIS DO SISTEMA
TFT_eSPI tft = TFT_eSPI();
SoundManager soundManager(BUZZER_PIN);

// Ponteiro para o jogo que está rodando atualmente.
// Por enquanto, será sempre o SnakeGame.
SnakeGame* currentGame;

// SETUP
void setup() {
  // Inicialização do hardware
  tft.init();
  tft.setRotation(1);
  Serial.begin(115200);

  // Configuração dos pinos
  pinMode(BTN_UP, INPUT_PULLDOWN);
  pinMode(BTN_DOWN, INPUT_PULLDOWN);
  pinMode(BTN_LEFT, INPUT_PULLDOWN);
  pinMode(BTN_RIGHT, INPUT_PULLDOWN);
  pinMode(BTN_START, INPUT_PULLDOWN);
  pinMode(BTN_SELECT, INPUT_PULLDOWN);
  pinMode(BTN_A, INPUT_PULLDOWN);
  pinMode(BTN_B, INPUT_PULLDOWN);
  pinMode(BUZZER_PIN, OUTPUT);

  // --- GERENCIAMENTO DE JOGO ---
  // 1. Cria uma nova instância do jogo Snake, passando o controle do display para ele.
  currentGame = new SnakeGame(&tft, &soundManager);
  
  // 2. Chama a função de setup do jogo para que ele se inicialize.
  currentGame->setup();
}

// LOOP
void loop() {
  soundManager.loop(); // IMPORTANTE: Atualiza o som a cada frame
  // 3. Apenas executa o loop do jogo atual.
  // Toda a lógica complexa agora está dentro de currentGame->loop().
  if (currentGame != nullptr) {
    currentGame->loop();
  }
}