#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// LCD on I2C, 16×2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ultrasonic pins
#define trigPin D5
#define echoPin D6

// SoftwareSerial for commands
SoftwareSerial ABC(4 /*D2=RX*/, 5 /*D1=TX*/);

// Wi‑Fi creds & endpoint
const char* ssid     = "Gungor";
const char* password = "0544731045";
const char* host     = "whimsical-pegasus-639ddc.netlify.app";

// Timing
unsigned long lastNetwork = 0;
const unsigned long NETWORK_INTERVAL = 15000; // 15 s

String lastCommand = "";

// ——— Setup —————————————————————————————————
void setup() {
  Serial.begin(115200);
  ABC.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // I2C + LCD init
  Wire.begin(D3, D4);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD Ready");
  delay(500);

  // Wi‑Fi with timeout
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) {
    Serial.print(".");
    delay(200);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    lcd.print("Cmd:(none)     ");
  } else {
    Serial.println("\nWiFi FAILED");
    lcd.print("Cmd:(no net)   ");
  }
}

// ——— Measure Distance —————————————————————————
float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long dur = pulseIn(echoPin, HIGH, 30000);
  return (dur == 0) ? -1 : (dur * 0.0343) / 2.0;
}

// ——— POST distance (2 s timeout) ——————————————————
bool postDistance(float d) {
  if (WiFi.status() != WL_CONNECTED) return false;
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  https.setTimeout(2000);
  String url = String("https://") + host + "/.netlify/functions/latest";
  https.begin(client, url);
  https.addHeader("Content-Type","application/json");
  int code = https.POST(String("{\"sensor\":\"") + d + "\"}");
  https.end();
  return code > 0;
}

// ——— GET command (2 s timeout) ———————————————————
String fetchCommand() {
  if (WiFi.status() != WL_CONNECTED) return "";
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  https.setTimeout(2000);
  String url = String("https://") + host + "/.netlify/functions/set-command";
  https.begin(client, url);
  int code = https.GET();
  String cmd = "";
  if (code > 0) {
    String resp = https.getString();
    DynamicJsonDocument doc(256);
    if (!deserializeJson(doc, resp)) {
      cmd = doc["command"].as<String>();
    }
  }
  https.end();
  return cmd;
}

// ——— NEW: GET time (2 s timeout) ———————————————————
String fetchTime() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to WiFi");
    return "";
  }

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  https.setTimeout(2000);

  String url = String("https://") + host + "/.netlify/functions/get-time";
  
  if (!https.begin(client, url)) {
    Serial.println("HTTPS begin failed");
    return "";
  }

  int code = https.GET();
  //Serial.print("HTTP code: "); //"HTTPS code:"
  //Serial.println(code);

  if (code <= 0) {
    Serial.println("HTTP GET failed");
    https.end();
    return "";
  }
  
  String resp = https.getString();
  //Serial.println("Raw response: " + resp); //raw time response:

  DynamicJsonDocument doc(256);
  DeserializationError err = deserializeJson(doc, resp);

  if (err) {
    Serial.print("JSON parse error: ");
    Serial.println(err.c_str());
    https.end();
    return "";
  }
  
  String timeStr = doc["time"].as<String>();
  //Serial.println("Parsed time: " + timeStr);  //parse time:
  https.end();
  return timeStr;
}


// ——— Main Loop —————————————————————————————
void loop() {
  unsigned long now = millis();

  // 1) LCD + sensor update every 500ms
  static unsigned long lastLCD = 0;
  if (now - lastLCD >= 500) {
    lastLCD = now;
    float dist = measureDistance();

    lcd.setCursor(0, 1);
    String line = (dist >= 0)
      ? "Water:" + String(dist, 1) + "cm"
      : "Water: No Echo";
    while (line.length() < 16) line += ' ';
    lcd.print(line);
  }

  // 2) Network work every 15s
  if (now - lastNetwork >= NETWORK_INTERVAL) {
    lastNetwork = now;
    float dist = measureDistance();
    postDistance(dist);

    // Get and show command
    lastCommand = fetchCommand();
    Serial.println("Cmd: " + lastCommand);
    lcd.setCursor(0, 0);
    String cmdLine = "Cmd:" + (lastCommand.length() ? lastCommand : "(none)");
    while (cmdLine.length() < 16) cmdLine += ' ';
    lcd.print(cmdLine);

    // Send command to Arduino
    if      (lastCommand == "Petunias"){              ABC.println("Petunias");  Serial.println("sent:Petunias");  }
    else if (lastCommand == "stopCherry Tomatoes"){   ABC.println("Cherry Tomatoes");Serial.println("sent:Cherry Tomatoes"); }
    else if (lastCommand == "Lettuce"){               ABC.println("Lettuce");Serial.println("sent:Lettuce"); }
    else if (lastCommand == "Rosemary"){              ABC.println("Rosemary");Serial.println("sent:Rosemary");  }
    else if (lastCommand == "Mint"){                  ABC.println("Mint");Serial.println("sent:Mint");  }    

    // Get and send time to Arduino
    String timeNow = fetchTime();
    if (timeNow.length()) {
      Serial.println("Time: " + timeNow);
      ABC.println("TIME:" + timeNow);  // ← Arduino will parse this
    }
  }

  delay(10);  // allow watchdog
}
