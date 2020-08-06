#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 4; //BLEタイムアウト
BLEScan* pBLEScan;

// 接触確認アプリのUUID
const char* uuid = "5f146110-bd74-4521-a364-83844968d366";

int deviceNum = 0;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
       if(advertisedDevice.haveServiceUUID()){
            if(strncmp(advertisedDevice.getServiceUUID().toString().c_str(),uuid, 36) == 0){
                int rssi = advertisedDevice.getRSSI();
                Serial.print("ADDR: ");
                Serial.println(advertisedDevice.getAddress().toString().c_str());
                Serial.print("RSSI");
                Serial.println(rssi);
                deviceNum++;
            }
        }else{
          Serial.println("No device!");
        }

    }
};

void Task1(void *pvParameters) {
  // loop()に書くとBLEスキャン中M5.update()が実行されなくてボタンが取れないのでマルチスレッド化している
  while(1) {
    deviceNum = 0;
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(deviceNum);
    Serial.println("Scan done!");
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  }
}

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
  pBLEScan->setInterval(5000); // スキャン間隔5秒
  pBLEScan->setWindow(4999);  // less or equal setInterval value
  xTaskCreatePinnedToCore(Task1,"Task1", 4096, NULL, 3, NULL, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("Scaninng...");
}