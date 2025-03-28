#define MOTOR_PIN1 18

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_PIN1, OUTPUT);
}

void loop() {
  // ポンプを正回転
  digitalWrite(MOTOR_PIN1, HIGH);
  Serial.println("digitalWrite END");
  delay(5000); // 5秒間動作

  // // ポンプを停止
  digitalWrite(MOTOR_PIN1, LOW);
  delay(2000); // 2秒間停止

  // 逆回転はできません (MOTOR_PIN2 が GND に固定されているため)
}