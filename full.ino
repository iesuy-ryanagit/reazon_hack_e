#include <ESP32Servo.h>

Servo myServo;  // サーボのインスタンス
#include <WiFi.h>
#include <HTTPClient.h>
#include <cJSON.h>

// WiFi接続情報
//コメントアウト

#include "DHT.h"

#define SERVO_PIN 17  // 信号線の接続ピン
#define MOTOR_PIN1 18
#define DHTPIN 35
#define DHTTYPE DHT22
#define SMPIN 14

DHT dht(DHTPIN, DHTTYPE);

void setupWifi()
{

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_PIN1, OUTPUT);
  myServo.attach(SERVO_PIN);

  Serial.begin(115200);
  setupWifi();
  Serial.println(F("DHT test!"));
  dht.begin();
  delay(2000);
}

String sendTextRequest(const String& text_data) {
  String endpoint = "https://api.openai.com/v1/chat/completions"; // 新しいエンドポイント
  String model = "gpt-4";  // 使用するモデル
  String contentType = "application/json";

  // リクエストボディを作成
  String reqBody = "{"
                    "\"model\": \"" + model + "\","
                    "\"messages\": [{\"role\": \"user\", \"content\": \"" + text_data + "\"}]"
                    "}";

  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Type", contentType);
  http.addHeader("Authorization", "Bearer " + openai_spi_key);

  int httpCode = http.POST(reqBody); // POSTリクエストを送信
  String response = http.getString(); // レスポンスを受け取る
  http.end();

  if (httpCode != 200) {
    Serial.printf("HTTP error code: %d\n", httpCode);
    return "";
  }

  // レスポンスJSONを解析
  cJSON* root = cJSON_Parse(response.c_str());
  if (!root) {
    Serial.println("Failed to parse JSON");
    return "";
  }

  cJSON* choices = cJSON_GetObjectItem(root, "choices");
  cJSON* text = cJSON_GetArrayItem(choices, 0); // 最初の選択肢を取得
  String result = "";
  if (text) {
    cJSON* message = cJSON_GetObjectItem(text, "message");
    if (message) {
      cJSON* content = cJSON_GetObjectItem(message, "content");
      if (content && cJSON_IsString(content)) {
        result = content->valuestring;
      }
    }
  }

  cJSON_Delete(root);
  return result;
}

bool Mesure()
{
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  int s = analogRead(SMPIN);
  int p = s / 16;

  String text_data = "湿度は " + String(h) + "%, 摂氏温度は " + String(t) + "°C, 華氏温度は " + String(f) + "°F、土壌水分は、"+ String(p) +"% です。水やりが必要なら1を、必要ないなら2を返してください。";

  String result = sendTextRequest(text_data);
  Serial.println("質問内容");
  Serial.println(text_data);
  // 結果表示
  Serial.println("=== APIからの応答 ===");
  if (result == "1")
  {
    Serial.println("Need Water");
    return (true);
  }
  else if (result == "2")
  {
    Serial.println("No Need Water");
    return (false);
  }
  else
  {
    Serial.println("No Need Water");
    Judge()
  }
}

void Water()
{
  digitalWrite(MOTOR_PIN1, HIGH);
  Serial.println("digitalWrite END");
  delay(5000); // 5秒間動作
  // // ポンプを停止
  digitalWrite(MOTOR_PIN1, LOW);
}

void DoWater()
{
  bool r = Mesure();
  
  if (r)
    Water();
}




void loop() {

  int i = 0;
  while (i < 180)
  {
    myServo.write(i);
    delay(3000);
    DoWater();
    i += 30;
    delay(3000);
  }
  while (i > 0)
  {
    myServo.write(i);
    delay(3000);
    DoWater();
    i -= 30;
    delay(3000);
  }
  delay(30000);
}

