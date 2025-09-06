#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"

// Configuração da câmera (modelo AI Thinker)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// 4 for flash led or 33 for normal led
#define LED_GPIO_NUM   4

void startCameraServer(); // se futuramente quiser webserver
void setupLedFlash();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  // --- Inicializar câmera ---
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Frame size (resolução)
  config.frame_size = FRAMESIZE_UXGA; // 1600x1200 (~2MP)
  config.jpeg_quality = 12; // menor = melhor qualidade
  config.fb_count = 1;

  Serial.println("=== INICIANDO CONFIGURAÇÃO ===");
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Falha ao iniciar a câmera");
    return;
  } else Serial.println("Câmera iniciada!");

  // --- Inicializar SD ---
  if (!SD_MMC.begin("/sdcard", true)) { // true = 1-bit mode (mais estável)
    Serial.println("Falha ao montar SD Card");
    return;
  } else Serial.println("Cartão SD montado!");

  Serial.println("Câmera e SD prontos!");
}

void loop() {
  Serial.println("Capturando imagem...");

  // Captura
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Falha na captura");
    delay(5000);
    return;
  }

  // Salvar no SD
  String path = "/foto_" + String(millis()) + ".jpg";
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Erro ao abrir arquivo no SD");
  } else {
    file.write(fb->buf, fb->len); // salva os bytes da foto
    Serial.printf("Imagem salva em: %s (%u bytes)\n", path.c_str(), fb->len);
    file.close();
  }

  // Liberar buffer
  esp_camera_fb_return(fb);

  delay(10000); // tira foto a cada 10s (ajuste para X minutos depois)
}
