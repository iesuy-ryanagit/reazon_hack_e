// ライブラリーからESP32Servo by Kebinをインストール
#include <ESP32Servo.h>

Servo myServo;  // サーボのインスタンス

#define SERVO_PIN 18  // 信号線の接続ピン

void setup() {
  myServo.attach(SERVO_PIN);  // サーボをGPIO 18に接続
}

void loop() {
  int i = 0;
  while (i < 3)
  {
    myServo.write(0);    // 0度に移動
    delay(1000);
    myServo.write(180);  // 180度に移動
   delay(1000);
   i++;
  }
  myServo.write(90);
  
  // 次の命令を待機
  // while (1) {
  //   // 何もせず待機
  //   delay(5000);  // 省エネのため少し待つ
  // }
}
