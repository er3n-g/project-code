#include <RTClib.h>
#include <SoftwareSerial.h>
#include <Wire.h>
 


RTC_DS3231 rtc;
int lastExecutedDay = -1;

SoftwareSerial ABC(9, 10); // RX = 9, TX = 10

int inPin1 = 5;
int inPin2 = 6;
char plant[20];

void setup() {
  // -- clock -- //
    rtc.begin();
    if (rtc.lostPower()) {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to compile time
  }
  // ----------- //
  pinMode(13,OUTPUT);
  Serial.begin(115200);      
  ABC.begin(9600);
  pinMode(inPin1, OUTPUT);
  pinMode(inPin2, OUTPUT); 
  digitalWrite(inPin2, LOW);
  Serial.println("Arduino Ready");
}

void trimNewline(char* str) {
  int len = strlen(str);
  while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || str[len - 1] == ' ')) {
    str[--len] = '\0';
  }
}


  // -- pump-- //
void ActPump(int mlPerDay) {
  int WaterTime = (mlPerDay * 16);
  digitalWrite(inPin1, HIGH);
  delay(WaterTime * 600); 
  digitalWrite(inPin1, LOW);
}
  //-----------//

void loop() {
  // -- clock -- //
    DateTime now = rtc.now();

    if (now.hour() == 12 && lastExecutedDay != now.day()) {
      Plant(); // Your 24-hour task
      lastExecutedDay = now.day();
  }
    delay(60000);
  //-------------//

}
  void Plant(){
  // --recive plant from ESP--//
  if (ABC.available()) {
    byte len = ABC.readBytesUntil('\n', plant, sizeof(plant) - 1);
    plant[len] = '\0'; 
    trimNewline(plant);

    Serial.print("Received: ");
    Serial.println(plant);
  }
    if (strcmp(plant,"Mint") == 0){
      ActPump(300);
      
    }
    if (strcmp(plant,"Rosemary") == 0){
      ActPump(180);
    }
    if (strcmp(plant,"Lettuce") == 0){
      ActPump(20);
    }
    if (strcmp(plant,"Cherry Tomatoes") == 0){
      ActPump(5);
    }
    if (strcmp(plant,"Petunias") == 0){
      ActPump(80);
    }
    ///////////////////////////
    
  }

