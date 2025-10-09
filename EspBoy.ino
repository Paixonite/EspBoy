// BIBLIOTECAS
#include <TFT_eSPI.h>
#include <SPI.h>
#include "Game.h"
#include "pins.h"
#include "audio_assets.h"
#include "SoundManager.h"

// Jogos
#include "SnakeGame.h"
#include "FlappyBirdGame.h"

// Definições de cores personalizadas, pois as cores padrão da biblioteca não são ideais.
#define MY_ORANGE tft.color565(255, 55, 0)
#define MY_GREY tft.color565(30, 30, 40)

// --- ESTADOS GLOBAIS DO SISTEMA ---
enum SystemState {
  STATE_SPLASH,
  STATE_MENU,
  STATE_IN_GAME
};
SystemState currentSystemState;

// --- OBJETOS GLOBAIS DO SISTEMA ---
TFT_eSPI tft = TFT_eSPI();
SoundManager soundManager(BUZZER_PIN);

// --- LÓGICA DA TELA DE SPLASH ---
const int SPLASH_DURATION_MS = 3500;
unsigned long splashStartTime;

// --- LÓGICA DO MENU ---
const char* gameNames[] = {"Snake", "Flappy Bird"};
const int gameCount = sizeof(gameNames) / sizeof(char*);
int selectedGameIndex = 0;
unsigned long lastBatteryUpdate = 0;

// Ponteiro para o objeto do jogo que está rodando.
Game* currentGame = nullptr;

// --- VARIÁVEIS DE ESTADO PARA O INPUT DO MENU ---
bool lastLeftState = LOW;
bool lastRightState = LOW;
bool lastConfirmState = LOW;
bool lastMuteActionState = LOW;

// =================================================================
// FUNÇÕES DE DESENHO E LÓGICA
// =================================================================

int calculateBatteryPercentage(int adcValue) {
    int percentage = 0;
    if (adcValue >= 3100) {
        percentage = 100;
    } else if (adcValue > 2950) {
        percentage = map(adcValue, 2950, 3100, 80, 100);
    } else if (adcValue > 2800) {
        percentage = map(adcValue, 2800, 2950, 50, 80);
    } else if (adcValue > 2600) {
        percentage = map(adcValue, 2600, 2800, 20, 50);
    } else if (adcValue > 2300) {
        percentage = map(adcValue, 2300, 2600, 0, 20);
    } else {
        percentage = 0;
    }
    return percentage;
}

bool isCharging() {
    static int lastAdcValue = 0;
    static unsigned long lastCheckTime = 0;
    static bool chargingState = false;
    
    unsigned long currentTime = millis();

    // Atualizamos o estado apenas a cada 1 segundo para estabilizar a leitura
    if (currentTime - lastCheckTime > 3000) {
        int currentAdcValue = analogRead(BATTERY_SENSE_PIN);
        
        if (currentAdcValue > lastAdcValue && lastAdcValue > 0) { // Se a voltagem subiu, está carregando
            chargingState = true;
        } else if (currentAdcValue < lastAdcValue) { // Se a voltagem caiu, está descarregando
            chargingState = false;
        }
        // Se a voltagem está estável, mantém o estado anterior.
        // Uma verificação extra para o caso de estar 100% carregado e conectado.
        if (currentAdcValue > 3080) {
            chargingState = true;
        }

        lastAdcValue = currentAdcValue;
        lastCheckTime = currentTime;
    }
    return chargingState;
}

String getBatteryStatus() {
    int currentAdcValue = analogRead(BATTERY_SENSE_PIN);
    int percentage = calculateBatteryPercentage(currentAdcValue);
    
    if (isCharging()) {
        return String(percentage - 31) + "%+"; // Adiciona '+' para indicar carga
    } else {
        return String(percentage) + "%";
    }
}

void updateBatteryDisplay() {
  // Esta função desenha APENAS a informação da bateria,
  // sem limpar a tela toda, para evitar pisca-pisca.
  tft.setTextDatum(TR_DATUM); // Alinha o texto no canto superior direito
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);

  // Desenha um pequeno retângulo preto atrás do texto para apagar o valor antigo.
  tft.fillRect(tft.width() - 60, 5, 60, 16, TFT_BLACK);

  String batteryText = getBatteryStatus();
  tft.drawString(batteryText, tft.width() - 5, 5);
}

void drawSplashScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM); // Alinha o texto no centro (Middle Center)
  tft.setTextSize(5);

  String title = "EspBoy";
  uint16_t rainbowColors[] = {TFT_RED, MY_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_BLUE, TFT_PURPLE};

  // Calcula a largura total da palavra para poder centralizá-la manualmente.
  int totalWidth = tft.textWidth(title);
  int cursorX = (tft.width() / 2) - (totalWidth / 2);
  int cursorY = tft.height() / 2 - 10;

  // Itera sobre cada letra para desenhá-la com uma cor diferente.
  for (int i = 0; i < title.length(); i++) {
    String letter = String(title.charAt(i));
    tft.setTextColor(rainbowColors[i % 6]);
    tft.setTextDatum(ML_DATUM); // Alinha a letra à esquerda do ponto do cursor (Middle Left)
    tft.drawString(letter, cursorX, cursorY);
    cursorX += tft.textWidth(letter); // Move o cursor para a posição da próxima letra.
  }

  // Desenha o texto "by Paixonite" embaixo.
  tft.setTextDatum(MC_DATUM); // Volta a alinhar pelo centro.
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.drawString("by Paixonite", tft.width() / 2, tft.height() / 2 + 30);
}

void drawMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("SELECT A GAME", tft.width() / 2, tft.height() / 4);

  // Desenha o nome do jogo selecionado com setas.
  String gameText = "< " + String(gameNames[selectedGameIndex]) + " >";
  tft.setTextColor(TFT_WHITE);
  tft.drawString(gameText, tft.width() / 2, tft.height() / 2);

  // Exibe a porcentagem da bateria.
  //updateBatteryDisplay();
}

void handleMenuInput() {
  // Lê o estado atual de todos os botões no início da função.
  bool currentLeftState = digitalRead(BTN_LEFT);
  bool currentRightState = digitalRead(BTN_RIGHT);
  bool currentConfirmState = digitalRead(BTN_A);
  bool currentMuteActionState = digitalRead(BTN_B);

  // Lógica para ativar/desativar o som (pressionando Select + B).
  if (digitalRead(BTN_SELECT) == HIGH && currentMuteActionState == HIGH) {
    // Usa a detecção de borda (ação ao soltar) para evitar disparos múltiplos.
    if (lastMuteActionState == LOW) {
      soundManager.setMute(!soundManager.isMuted());

      // Fornece um feedback visual para o usuário.
      tft.setTextColor(TFT_RED);
      tft.setTextSize(1);
      tft.setTextDatum(BR_DATUM); // Alinha no canto inferior direito
      if (soundManager.isMuted()) {
        tft.drawString("Sound OFF", tft.width() - 5, tft.height() - 5);
      } else {
        tft.drawString("Sound ON", tft.width() - 5, tft.height() - 5);
      }
      delay(500);
      drawMenu(); // Redesenha o menu para apagar a mensagem.
    }
  }

  // Navegação para a esquerda.
  if (lastLeftState == HIGH && currentLeftState == LOW) {
    selectedGameIndex--;
    if (selectedGameIndex < 0) {
      selectedGameIndex = gameCount - 1; // Volta para o final da lista.
    }
    soundManager.play(MELODY_MENU_NAVIGATE, MELODY_MENU_NAVIGATE_LENGTH);
    tft.fillScreen(MY_GREY); // Efeito visual de "flash".
    delay(40);
    drawMenu();
  }

  // Navegação para a direita.
  if (lastRightState == HIGH && currentRightState == LOW) {
    selectedGameIndex++;
    if (selectedGameIndex >= gameCount) {
      selectedGameIndex = 0; // Volta para o início da lista.
    }
    soundManager.play(MELODY_MENU_NAVIGATE, MELODY_MENU_NAVIGATE_LENGTH);
    tft.fillScreen(MY_GREY); // Efeito visual de "flash".
    delay(40);
    drawMenu();
  }

  // Seleção de jogo com o botão 'A'.
  if (lastConfirmState == HIGH && currentConfirmState == LOW) {
    switch (selectedGameIndex) {
      case 0: // Snake
        if (currentGame != nullptr) delete currentGame; // Libera memória do jogo anterior.
        currentGame = new SnakeGame(&tft, &soundManager); // Cria o novo jogo.
        break;
      
      case 1: // Flappy
        if (currentGame != nullptr) delete currentGame; // Libera memória do jogo anterior.
        currentGame = new FlappyBirdGame(&tft, &soundManager); // Cria o novo jogo.
        break;
    }

    currentGame->setup();
    currentSystemState = STATE_IN_GAME;
  }

  // Atualiza o estado anterior dos botões para o próximo ciclo do loop.
  lastLeftState = currentLeftState;
  lastRightState = currentRightState;
  lastConfirmState = currentConfirmState;
  lastMuteActionState = currentMuteActionState;
}

// =================================================================
// SETUP E LOOP PRINCIPAIS
// =================================================================

void setup() {
  // Inicialização de hardware.
  tft.init();
  tft.setRotation(1);
  Serial.begin(115200);

  // Configuração dos pinos dos botões.
  pinMode(BTN_UP, INPUT_PULLDOWN);
  pinMode(BTN_DOWN, INPUT_PULLDOWN);
  pinMode(BTN_LEFT, INPUT_PULLDOWN);
  pinMode(BTN_RIGHT, INPUT_PULLDOWN);
  pinMode(BTN_START, INPUT_PULLDOWN);
  pinMode(BTN_SELECT, INPUT_PULLDOWN);
  pinMode(BTN_A, INPUT_PULLDOWN);
  pinMode(BTN_B, INPUT_PULLDOWN);
  pinMode(BUZZER_PIN, OUTPUT);

  // Habilita o circuito de alimentação da bateria.
  pinMode(POWER_ENABLE_PIN, OUTPUT);
  digitalWrite(POWER_ENABLE_PIN, HIGH);

  // Configuração inicial do sistema.
  currentSystemState = STATE_SPLASH;
  drawSplashScreen();
  splashStartTime = millis();
  soundManager.play(MELODY_SPLASH, MELODY_SPLASH_LENGTH);
}

void loop() {
  // O gerenciador de som deve ser atualizado constantemente, em qualquer estado.
  soundManager.loop();

  // Roteia a lógica principal com base no estado atual do sistema.
  switch (currentSystemState) {
    case STATE_SPLASH:
      // Permanece neste estado até o tempo da splash screen acabar.
      if (millis() - splashStartTime > SPLASH_DURATION_MS) {
        currentSystemState = STATE_MENU;
        drawMenu();
      }
      break;

    case STATE_MENU:
      // Executa a lógica de interação com o menu.
      handleMenuInput();

      // Atualiza o display da bateria periodicamente.
      if (millis() - lastBatteryUpdate > 5000) { // Atualiza a cada 5 segundos
        lastBatteryUpdate = millis();
        //updateBatteryDisplay();
      }
      break;

    case STATE_IN_GAME:
      // Se um jogo estiver carregado, executa o loop dele.
      if (currentGame != nullptr) {
        currentGame->loop();

        // Verifica se o jogo solicitou o encerramento.
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