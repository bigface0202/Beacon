#include <Arduino.h>
#include <M5Stack.h>
#include <WiFi.h>

int xPlot = 20;
int points = 10;

// Network information
// char *ssid = "GlobalWalkers_Sol_2_4GHz";
// const char *password = "8gotanda8";

char *ssid_list[] = {"GlobalWalkers_Sol_2_4GHz", "SPWN_H37_66E8A6"};
char *password_list[] = {"8gotanda8", "n3b4n9atamq3ntn"};

void connectWiFi(char *ssid, char *password)
{
  while (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    M5.Lcd.print(".");
    delay(1000);
  }
}

int getStrength(int points)
{
  long rssi = 0;
  long averageRSSI = 0;

  for (int i = 0; i < points; i++)
  {
    rssi += WiFi.RSSI();
    delay(10);
  }
  averageRSSI = rssi / points;
  return averageRSSI;
}

void setup()
{
  M5.begin();
  M5.Lcd.setTextSize(2);

  delay(1000);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(20, 220);
  M5.Lcd.print("0");
  M5.Lcd.setCursor(110, 220);
  M5.Lcd.print("10.0");
  M5.Lcd.setCursor(210, 220);
  M5.Lcd.print("20.0");
  M5.Lcd.setCursor(290, 220);
  M5.Lcd.print("30.0");
  M5.Lcd.drawLine(20, 215, 320, 215, WHITE);
  M5.Lcd.drawLine(20, 215, 20, 120, WHITE);
  M5.Lcd.setCursor(0, 120);
  M5.Lcd.print("0");
  M5.Lcd.setCursor(0, 140);
  M5.Lcd.print("-20");
  M5.Lcd.setCursor(0, 160);
  M5.Lcd.print("-40");
  M5.Lcd.setCursor(0, 180);
  M5.Lcd.print("-60");
  M5.Lcd.setCursor(0, 200);
  M5.Lcd.print("-80");
  M5.Lcd.setTextSize(2);
}

void loop()
{
  int wifiStrength;
  M5.Lcd.fillRect(0, 0, 320, 100, BLACK);
  for (int i = 0; i < 2; i++)
  {
    char *ssid = ssid_list[i];
    char *password = password_list[i];
    connectWiFi(ssid, password);
    wifiStrength = getStrength(points);
    if (i == 0){
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.println("2.4GHz");
      M5.Lcd.fillCircle(xPlot, 121 + (-1 * wifiStrength), 2, RED);
    }else{
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.println("Wireless");
      M5.Lcd.fillCircle(xPlot, 121 + (-1 * wifiStrength), 2, GREEN);
    }
    M5.Lcd.print("Intensity:");
    M5.Lcd.println(wifiStrength);
    WiFi.disconnect(true);
  }
  xPlot++;
  if (xPlot > 320)
  {
    xPlot = 20;
    M5.Lcd.fillRect(21, 100, 320, 114, BLACK);
  }
}