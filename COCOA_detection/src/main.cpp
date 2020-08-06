#include <Arduino.h>
#include <M5Stack.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 4; //BLEタイムアウト
BLEScan* pBLEScan;

// 接触確認アプリのUUID
const char* uuid = "0000fd6f-0000-1000-8000-00805f9b34fb";

int deviceNum = 0;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
       if(advertisedDevice.haveServiceUUID()){
            if(strncmp(advertisedDevice.getServiceUUID().toString().c_str(),uuid, 36) == 0){
                int rssi = advertisedDevice.getRSSI();
                M5.Lcd.setTextSize(2);
                M5.Lcd.print("ADDR: ");
                M5.Lcd.println(advertisedDevice.getAddress().toString().c_str());
                M5.Lcd.print("RSSI");
                M5.Lcd.println(rssi);
                deviceNum++;
            }
        }

    }
};

void drawScreen() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.print("COCOA Counter:");
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.printf(" %2d\n", deviceNum);
    M5.Lcd.setTextSize(1);
}
void Task1(void *pvParameters) {
  // loop()に書くとBLEスキャン中M5.update()が実行されなくてボタンが取れないのでマルチスレッド化している
  while(1) {
    deviceNum = 0;
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(deviceNum);
    Serial.println("Scan done!");
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
    drawScreen();
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
  pBLEScan->setInterval(5000); // スキャン間隔5秒
  pBLEScan->setWindow(4999);  // less or equal setInterval value
  xTaskCreatePinnedToCore(Task1,"Task1", 4096, NULL, 3, NULL, 1);
}

void loop() {
  M5.update();
    if ( M5.BtnA.wasReleased() ) {
      // M5.Axp.PowerOff();
  }
}