#include <ESP32Servo.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// SG90 ServoMoter Setting
#define SERVO_MIN_US 500
#define SERVO_MAX_US 2400
#define SERVO_HERTZ 50
#define SERVO_PIN 23

#define BT_NAME "DoorMotor" // BlueTooth Name
#define ACT_TIMER 5000
#define LOOP_CYCLE 1000

BluetoothSerial SerialBT;
Servo servo;
int pos = 0;
int servoTimer = 0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin(BT_NAME);  
  servo.setPeriodHertz(SERVO_HERTZ);
  servo.attach(SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
}

void loop() {
  String readBuf="";
  
  if (SerialBT.available()) {
    
    // BlueToothからデータ読み込み
    readBuf = SerialBT.readString();

    // ドアオープン
    if ((readBuf.startsWith("op")) && (pos == 0)){
      setOpenPos();
    }

    // 初期位置に戻す
    if ((readBuf.startsWith("cl")) && (pos == 180)){
      setInitPos();
    }

    // アクションタイマー設定
    servoTimer = ACT_TIMER;

  } else {

    // 前回アクションから5秒経ったら初期位置に戻す
    if (servoTimer > 0){
      servoTimer -= LOOP_CYCLE;
    }
    if ((servoTimer == 0) && (pos == 180)){
      setInitPos();
    }

  }

  delay(LOOP_CYCLE);
}

void setOpenPos() {
  for (pos = 0; pos < 180; pos += 1) { // sweep from 0 degrees to 180 degrees
    servo.write(pos);
    delay(2); 
  }
}

void setInitPos() {
  for (pos = 180; pos > 0; pos -= 1) { // sweep from 180 degrees to 0 degrees
    servo.write(pos);
    delay(2);
  }
}
