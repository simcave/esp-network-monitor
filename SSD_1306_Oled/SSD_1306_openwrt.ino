#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include "struct.h"
#include "parse.h"
#include "icon.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels 
#define SCREEN_HEIGHT 64 // OLED display height, in pixels 
#define OLED_RESET -1 // Reset pin 
#define SCREEN_ADDRESS 0x3C
time_t unixTime;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "Sim-Cave"; // SSID WIFI
const char* password = "12345678@@"; // PASSWORD WIFI (if no password, leave it empty)
String iface = "wwan0"; // Interface to be monitored

void setup() {
    Serial.begin(115200);
    
    // Initialize the OLED object
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }

    // Display connecting message
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 8);
    display.print("Connecting...");
    display.display();
    delay(3000);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
    
    // Display connected message
    booting();
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        parseNetwork(iface);
        netspeed();
        parseSysinfo(); // Ensure this populates sysinfo.uptime
        unixTime = sysinfo.uptime; // Set unixTime to sysinfo.uptime
        parseTemp();
        systeminfo();
        
        parseVnstat(iface);
        datausage();
        parseBoard();
        boardinfo();
        delay(5000); // Delay to avoid rapid loop execution
    }
}

void booting() {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 17);
    display.print("Connected to :");
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20, 29);
    display.print(String(ssid));
    display.drawBitmap(0, 28, ssidcon, 16, 16, WHITE);

    display.display();
    delay(3000);
}

void netspeed() {
    display.clearDisplay();
    display.fillRoundRect(0, 0, 128, 16, 6, WHITE);
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(10, 1);
    display.print("Net Speed");
    
    display.drawBitmap(0, 20, upicon, 16, 16, WHITE);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20, 21);
    display.print(String(netdata.UPLOAD / 1000 * -1));
    display.setTextSize(1);
    display.print(" Mbps");
    
    display.drawBitmap(0, 40, downicon, 16, 16, WHITE);
    display.setTextSize(2);
    display.setCursor(20, 41);
    display.print(String(netdata.DOWNLOAD / 1000));
    display.setTextSize(1);
    display.print(" Mbps");
    
    display.display();
    delay(4000);
}

void systeminfo() {
    display.clearDisplay();
    display.fillRoundRect(0, 0, 128, 16, 6, WHITE);
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(16, 1);
    display.print("Sys Info");
    
    display.drawBitmap(0, 17, cpuicon, 16, 16, WHITE);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20, 18);
    display.print(String(sysinfo.cpu));
    display.print(" %");

    display.drawBitmap(0, 33, tempicon, 16, 16, WHITE);
    display.setTextSize(2);
    display.setCursor(20, 34);
    display.print(String(sysinfo.temp/1000));
    display.print((char)247);
    display.print("C");

    display.drawBitmap(0, 49, ramicon, 16, 16, WHITE);
    display.setTextSize(2);
    display.setCursor(20, 50);
    display.print(String(sysinfo.memPercent*100));
    display.print(" %");

    display.display();
    delay(4000);
}

void datausage() {
    display.clearDisplay();
    display.fillRoundRect(0, 0, 128, 16, 6, WHITE);
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(4, 1);
    display.print("Data Usage");

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 29);
    display.print("Today");
    display.setTextSize(2);
    display.setCursor(32, 25);
    display.print(":"+String(vnstat.todayTotal/1000000));
    display.setTextSize(1);
    display.setCursor(110, 32);
    display.print("GB");

    display.setTextSize(1);
    display.setCursor(0, 52);
    display.print("Month");
    display.setTextSize(2);
    display.setCursor(32, 48);
    display.print(":"+String(vnstat.monthTotal/1000000));
    display.setTextSize(1);
    display.setCursor(110, 55);
    display.print("GB");

    display.display();
    delay(4000);
}

void boardinfo() {
    // Convert sysinfo.uptime to a readable format
    time_t uptime = sysinfo.uptime;
    int days = uptime / 86400;
    uptime %= 86400;
    int hours = uptime / 3600;
    uptime %= 3600;
    int minutes = uptime / 60;
    int seconds = uptime % 60;

    display.clearDisplay();
    display.fillRoundRect(0, 0, 128, 16, 6, WHITE);
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(4, 1);
    display.print("Board Info");

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.print("Host:"+String(board.hostName));
    display.setCursor(0, 30); 
    display.print("Version:"+String(board.version));
    display.setCursor(0, 40);
    display.print("Kernel:"+String(board.kernel));
    display.setCursor(0, 50);   
    display.print("Uptime: " + String(days) + "d " + String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s");

    display.display();
    delay(4000);
}
