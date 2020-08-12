/*
 * File:      BLEObserver.ino
 * Function:  BLEブロードキャスト通信のオブザーバーとして動作します。
 *            ブロードキャストしているデバイスを見つけて、それが目的のデバイスであれば、温湿度
 *            計測データを受信してシリアルモニターに詳細を表示し、OLEDディスプレイに温度と湿度
 *            を表示します（OLED表示はデバイス識別番号1のもののみ）。
 *            デバイスからの異常情報をキャッチするとLEDを点灯して知らせ、プッシュボタンを押すと
 *            消灯します。
 * Date:      2019/04/10
 * Author:    M.Ono
 * 
 * Hardware   MCU:  ESP32 (DOIT ESP32 DEVKIT V1 Board)
 *            ブレッドボードに上記開発ボードとOLEDディスプレイ、プッシュボタン、LEDを配線
 *            OLEDディスプレイはSSD1306  (0.96inch 128x64dot) 
 */
#include <Arduino.h>
#include <BLEDevice.h>
#include <Wire.h>                   // I2C interface
#include <M5Stack.h>


/* 基本属性定義  */
#define SPI_SPEED   115200          // SPI通信速度

/* スキャン制御用 */
#define DEVICE_NAME "ESP32"         // 対象デバイス名
#define MAX_DEVICES  8              // 最大デバイス数
#define ManufacturerId 0xffff       // 既定のManufacturer ID
const int scanning_time = 1;        // スキャン時間（秒）
BLEScan* pBLEScan;                  // Scanオブジェクトへのポインター
int prev_seq[MAX_DEVICES] = { -1, -1, -1, -1, -1, -1, -1, -1 };

/* 受信データ構造体 */
struct tmpData {
    int     device_number;          // デバイス識別番号
    bool    abnormal;               // デバイス異常
    int     seq_number;             // シーケンス番号
    float   temperature;            // 温度
    float   humidity;               // 湿度
};

/* LEDピン */
const int ledPin = 25;              // LEDの接続ピン

/* プッシュボタン */
const int buttonPin = 32;           // プッシュボタンの接続ピン


/*  初期化処理  */
void doInitialize() {
    Serial.begin(SPI_SPEED);
    M5.begin();
    pinMode(buttonPin, INPUT);        // GPIO設定：プッシュボタン        
    pinMode(ledPin, OUTPUT);          // GPIO設定：LED
    digitalWrite(ledPin, LOW);
    Serial.println("BLE Client start ...");
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.fillScreen(BLACK);
}


/*****************************************************************************
 *                          Predetermined Sequence                           *
 *****************************************************************************/
void setup() {
    doInitialize();                             // 初期化処理をして
    BLEDevice::init("");                        // BLEデバイスを作成する
    Serial.println("Client application start...");
    pBLEScan = BLEDevice::getScan();            // Scanオブジェクトを取得して、
    pBLEScan->setActiveScan(false);             // パッシブスキャンに設定する
}

void loop() {
    struct tmpData td;
    // 所定時間だけスキャンして、見つかったデバイス数を取得する
    BLEScanResults foundDevices = pBLEScan->start(scanning_time);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    int count = foundDevices.getCount();

    for (int i = 0; i < count; i++) {     // 受信したアドバタイズデータを調べて
        BLEAdvertisedDevice dev = foundDevices.getDevice(i);
        std::string device_name = dev.getName();
        if (device_name == DEVICE_NAME 
                && dev.haveManufacturerData()) {   // デバイス名が一致しManufacturer dataがあって
            std::string data = dev.getManufacturerData();
            int manu_code = data[1] << 8 | data[0];
            int device_number = data[2];
            int seq_number = data[4];
            int ble_rssi = 0;
            int power = 0;
            if(dev.haveRSSI()) ble_rssi = dev.getRSSI();
            if(dev.haveTXPower()) power = dev.getTXPower();
            
            M5.Lcd.print("Device num:");
            M5.Lcd.println(device_number);
            M5.Lcd.print("RSSI:");
            M5.Lcd.println(ble_rssi);
            M5.Lcd.print("Power:");
            M5.Lcd.println(power);

            // デバイス識別番号が有効かつシーケンス番号が更新されていたら
            if (device_number >= 1 && device_number <= MAX_DEVICES
                      && seq_number != prev_seq[device_number - 1]) {
                // 受信データを取り出す
                prev_seq[device_number - 1] = seq_number;
                td.device_number = device_number;
                td.abnormal = (bool)data[3];
                td.seq_number = seq_number;
                td.temperature = (float)(data[6] << 8 | data[5]) / 100.00;
                td.humidity = (float)(data[8] << 8 | data[7]) / 100.00;
            }
        }
    }
    // プッシュボタン押下でLEDを消灯してディスプレイ表示を消去する
    int buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {
        digitalWrite(ledPin, LOW);
    }
}
