#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include<SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


SoftwareSerial ABC(4, 5);  //D2, D1


#define trigPin D5
#define echoPin D6

const char* ssid = "Gungor";
const char* password = "0544731045";

const char* host = "whimsical-pegasus-639ddc.netlify.app";
const int httpsPort = 443;

String lastCommand = "";

void setup() {
//--LCD--//
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Counter:");

  Serial.begin(115200);
  ABC.begin(9600);
  delay(100);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
  if (duration == 0) {
    Serial.println("No echo received");
    return -1;
  }
  float distance = (duration * 0.0343) / 2.0;
  return distance;
}

bool postDistance(float dist) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected for POST");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();  // skip SSL cert validation (not recommended for production)

  HTTPClient https;
  String url = String("https://") + host + "/.netlify/functions/latest";

  https.begin(client, url);
  https.addHeader("Content-Type", "application/json");

  String json = "{\"sensor\":\"" + String(dist) + "\"}";

  int httpCode = https.POST(json);

  if (httpCode > 0) {
    String payload = https.getString();
    Serial.println("POST response code: " + String(httpCode));
    Serial.println("POST response payload: " + payload);
  } else {
    Serial.println("POST failed, error: " + https.errorToString(httpCode));
    https.end();
    return false;
  }

  https.end();
  return true;
}

String fetchCommand() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected for GET");
    return "";
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  String url = String("https://") + host + "/.netlify/functions/set-command";

  https.begin(client, url);
  int httpCode = https.GET();

  String command = "";

  if (httpCode > 0) {
    String payload = https.getString();
    Serial.println("GET response code: " + String(httpCode));
    Serial.println("GET response payload: " + payload);

    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      command = doc["command"].as<String>();
    } else {
      Serial.println("JSON parse error: " + String(error.c_str()));
    }
  } else {
    Serial.println("GET failed, error: " + https.errorToString(httpCode));
  }

  https.end();
  return command;
}

void loop() {
  float distance = measureDistance();
  if (distance > 0) {
    Serial.println("Measured Distance: " + String(distance) + " cm");
    if (postDistance(distance)) {
      Serial.println("Distance posted successfully");
    } else {
      Serial.println("Distance POST failed");
    }
  } else {
    Serial.println("Invalid distance measurement");
  }
  // -- LCD -- //
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Value: ");
  lcd.print(distance);
  lcd.print(" cm");
 //------------//
  lastCommand = fetchCommand();
  if (lastCommand != "") {
    Serial.println("Latest Command: " + lastCommand);

    // Act on commands
    if (lastCommand == "start") {
      ABC.println("Petunias");
    } 
    else if (lastCommand == "stop") {
      ABC.println("Cherry Tomatoes");
    } 
    else if (lastCommand == "reset") {
      ABC.println("Lettuce");
    }
    else if (lastCommand == "calibrate") {
      ABC.println("Rosemary");
    }
    else if (lastCommand == "debug") {
      ABC.println("Mint");  
    }
    else {
      Serial.println("Unknown command");
    }
  }

  delay(3000); // Adjust frequency as needed
}
