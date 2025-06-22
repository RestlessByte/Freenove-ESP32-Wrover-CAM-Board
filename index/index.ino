#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_camera.h>
#include <Base64.h> // Библиотека для кодирования изображения в Base64

// ----------------------------
// Настройка пинов для камеры OV2640 
// (для Freenove ESP32-Wrover Cam)
// ----------------------------
#define PWDN_GPIO_NUM    -1    // Если модуль не использует PWDN, оставляем -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

// ----------------------------
// Функция отладки: вывод сообщений в Serial Monitor
// ----------------------------
void debug(String msg) {
  Serial.println(msg);
}

// ----------------------------
// Функция обработки ошибок: вывод сообщения и перезагрузка устройства
// ----------------------------
void errorHandler(String msg) {
  debug("[ERROR] " + msg);
  ESP.restart();
}

// ----------------------------
// Функция инициализации камеры с заданной конфигурацией
// ----------------------------
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_1;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;   // Разрешение 640x480
  config.jpeg_quality = 12;            // Чем меньше число – выше качество
  config.fb_count = 2;                 // Используем два буфера для кадров

  debug("Initializing camera...");
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    debug("Camera init failed: " + String(esp_err_to_name(err)));
    return false;
  }
  return true;
}

// ----------------------------
// Функция подключения к WiFi (использует данные из config.h)
// ----------------------------
bool connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  debug("Connecting to WiFi...");
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > 20000) {
      debug("WiFi connection failed!");
      return false;
    }
    delay(500);
  }
  debug("WiFi connected! IP: " + WiFi.localIP().toString());
  return true;
}

// ----------------------------
// Функция отправки изображения на сервер в формате JSON
// JSON имеет вид: { "img" : "<base64 изображение>" }
// Отправка данных на сервер (HTTP POST) пока закомментирована для последующей реализации шифрования
// ----------------------------
  
// ----------------------------
// Функция отправки изображения в Telegram с использованием multipart/form-data
// Добавляется подпись (caption), передаваемая в качестве параметра
// ----------------------------
void sendToTelegram(String caption) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    errorHandler("Failed to capture image");
    return;
  }
  
  String boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
  String partChatId = "--" + boundary + "\r\n" +
                        "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n" +
                        String(TELEGRAM_CHAT_ID) + "\r\n";
  String partCaption = "--" + boundary + "\r\n" +
                        "Content-Disposition: form-data; name=\"caption\"\r\n\r\n" +
                        caption + "\r\n";
  String partPhotoHeader = "--" + boundary + "\r\n" +
                           "Content-Disposition: form-data; name=\"photo\"; filename=\"image.jpg\"\r\n" +
                           "Content-Type: image/jpeg\r\n\r\n";
  String endBoundary = "\r\n--" + boundary + "--\r\n";
  
  int totalLen = partChatId.length() + partCaption.length() +
                 partPhotoHeader.length() + fb->len + endBoundary.length();
  
  uint8_t *postData = (uint8_t *)malloc(totalLen);
  if (!postData) {
    esp_camera_fb_return(fb);
    errorHandler("Failed to allocate memory for POST data");
    return;
  }
  
  int offset = 0;
  memcpy(postData + offset, partChatId.c_str(), partChatId.length());
  offset += partChatId.length();
  memcpy(postData + offset, partCaption.c_str(), partCaption.length());
  offset += partCaption.length();
  memcpy(postData + offset, partPhotoHeader.c_str(), partPhotoHeader.length());
  offset += partPhotoHeader.length();
  memcpy(postData + offset, fb->buf, fb->len);
  offset += fb->len;
  memcpy(postData + offset, endBoundary.c_str(), endBoundary.length());
  offset += endBoundary.length();
  
  esp_camera_fb_return(fb);
  
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(TELEGRAM_BOT_TOKEN) + "/sendPhoto";
  http.begin(url);
  http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
  
  int httpCode = http.POST(postData, totalLen);
  String response = http.getString();
  if (httpCode == 200) {
    debug("Photo sent to Telegram");
  } else {
    debug("Telegram error: " + String(httpCode));
    debug("Response: " + response);
  }
  http.end();
  free(postData);
}

// ----------------------------
// Функция setup() - выполняется один раз при запуске
// ----------------------------
void setup() {
  Serial.begin(115200);
  delay(2000);

  // Настройка пина для датчика движения (PIR) - значение берется из config.h
  pinMode(PIR_PIN, INPUT_PULLUP);
  
  // Настройка питания камеры (если используется PWDN)
  if(PWDN_GPIO_NUM != -1) {
    pinMode(PWDN_GPIO_NUM, OUTPUT);
    digitalWrite(PWDN_GPIO_NUM, LOW);
  }
  delay(1000);

  if (!initCamera()) errorHandler("Camera init failed");
  
  // Корректировка ориентации камеры:
  // Если изображение получается перевёрнутым, измените параметры set_vflip и set_hmirror
  sensor_t * s = esp_camera_sensor_get();
  if (s) {
    s->set_vflip(s, 1);    // Включаем вертикальный переворот (при необходимости поменяйте на 0)
    s->set_hmirror(s, 0);  // Отключаем зеркальноc:\Users\air51\OneDrive\Документы\Arduino\index\config.hе отображение (при необходимости установите 1)
  }
  
  if (!connectWiFi()) errorHandler("WiFi connection failed");
  
  // Инициализация генератора случайных чисел (для демонстрационных данных)
  randomSeed(esp_random());
}

// ----------------------------
// Функция loop() - основной цикл программы
// При срабатывании датчика движения отправляем изображение на сервер и в Telegram
// ----------------------------
void loop() {
  static uint32_t lastTrigger = 0;
  if (digitalRead(PIR_PIN) && (millis() - lastTrigger > 2000)) {
    // Генерация случайных координат для демонстрационного сообщения
    String message = "Обнаружено какое то движение, посмотрите на скриншот с камеры";
    debug(message);
    
    // Отправка изображения на сервер (в виде JSON)
    
    // Отправка изображения в Telegram с подписью (caption)
    sendToTelegram(message);
    
    lastTrigger = millis();
  }
  delay(50);
}
