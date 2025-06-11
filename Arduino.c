#include <SoftwareSerial.h>
SoftwareSerial ABC(9, 10); // RX = 9, TX = 10

int inPin1 = 5;
int inPin2 = 6;
char plant[20];

void setup() {
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

// --recive plant from ESP--//
void loop() {
  if (ABC.available()) {
    byte len = ABC.readBytesUntil('\n', plant, sizeof(plant) - 1);
    plant[len] = '\0'; 
    trimNewline(plant);

    Serial.print("Received: ");
    Serial.println(plant);
  }
    if (strcmp(plant,"Mint") == 0){
      int mlPerDay = 180;
      int WaterTime = (mlPerDay * 16);
      digitalWrite(inPin1, HIGH);
      delay(WaterTime * 600); // how much time keeping the pump on
      digitalWrite(inPin1, LOW);
      
    }
    if (strcmp(plant,"Rosemary") == 0){
      int mlPerDay = 180;
      int WaterTime = (mlPerDay * 16); //how much time to keep the pump on in minents
      digitalWrite(inPin1, HIGH);
      delay(WaterTime * 600); // convert to milisecs
      digitalWrite(inPin1, LOW);
    }
    if (strcmp(plant,"Lettuce") == 0){
      int mlPerDay = 180;
      int WaterTime = (mlPerDay * 16);
      digitalWrite(inPin1, HIGH);
      delay(WaterTime * 600); 
      digitalWrite(inPin1, LOW);
    }
    if (strcmp(plant,"Cherry Tomatoes") == 0){
      int mlPerDay = 180;
      int WaterTime = (mlPerDay * 16);
      digitalWrite(inPin1, HIGH);``````````````````````````````````````````                                                     
      delay(WaterTime * 600); 
      digitalWrite(inPin1, LOW);
    }
    if (strcmp(plant,"Petunias") == 0){
      int mlPerDay = 180;
      int WaterTime = (mlPerDay * 16);
      digitalWrite(inPin1, HIGH);
      delay(WaterTime * 600); 
      digitalWrite(inPin1, LOW);
    }
    ///////////////////////////
    
  }
