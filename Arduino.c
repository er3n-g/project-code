#include <SoftwareSerial.h>

SoftwareSerial ABC(11, 12); // RX = 11, TX = 12

#define ENA 10
#define inPin1 8
#define inPin2 9
#define inPin3 6
#define inPin4 7
#define ENB 5
#define MoistPin A4

char incoming[30];
char selectedPlant[30] = "";  // Store selected plant globally
int PumpHaveActed = 0;
int DaysPassed = 0;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(MoistPin, INPUT);
  Serial.begin(115200);
  ABC.begin(9600);
  pinMode(ENA, OUTPUT);
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

void ActWaterPump(int WmlPerDay) {
  int WwaterTime = WmlPerDay * 16;
  analogWrite(ENA, 50);
  digitalWrite(inPin1, HIGH);
  delay(WwaterTime * 600);
  digitalWrite(inPin1, LOW);
}

void ActFertPump(int FmlPerDay) {
  int FwaterTime = FmlPerDay * 16;
  analogWrite(ENB, 50);
  digitalWrite(inPin3, HIGH);
  delay(FwaterTime * 600);
  digitalWrite(inPin3, LOW);
}


void loop() {
  if (ABC.available()) {
    byte len = ABC.readBytesUntil('\n', incoming, sizeof(incoming) - 1);
    incoming[len] = '\0';
    trimNewline(incoming);

    Serial.print("Received: ");
    Serial.println(incoming);

    if (strncmp(incoming, "TIME:", 5) == 0) {
      char* timeStr = incoming + 5;
      Serial.print("Parsed time: ");
      Serial.println(timeStr);

      int hour = atoi(strtok(timeStr, ":"));
      int minute = atoi(strtok(NULL, ":"));
      int SoilMoist = analogRead(MoistPin);

      /////////// -- activate fertelizer pumpy every week -- ////////
        if (DaysPassed >= 7){
          if (strcmp(selectedPlant, "Mint") == 0) {
            ActFertPump(1);
            DaysPassed = 0;
          }
          else if (strcmp(selectedPlant, "Rosemary") == 0){
            ActFertPump(2);
            DaysPassed = 0;
          }
          else if (strcmp(selectedPlant, "Lettuce") == 0) {
            ActFertPump(3);
            DaysPassed = 0;
          }
          else if (strcmp(selectedPlant, "Cherry Tomatoes") == 0) {
            ActFertPump(3);
            DaysPassed = 0;
          }
          else if (strcmp(selectedPlant, "Petunias") == 0) {
            ActFertPump(5);
            DaysPassed = 0;
          }
        }
      /////////////////////////////////////////////////////////////

        
      ////////// -- activate water pump every day -- ///////
     // if (SoilMoist < 500) {
        if (hour == 22 && minute == 30 && PumpHaveActed != 1) {
          if (strcmp(selectedPlant, "Mint") == 0) {
            ActWaterPump(1);
            PumpHaveActed = 1;
            DaysPassed = DaysPassed +1;
          } else if (strcmp(selectedPlant, "Rosemary") == 0) {
            ActWaterPump(2);
            PumpHaveActed = 1;
            DaysPassed = DaysPassed +1;
          } else if (strcmp(selectedPlant, "Lettuce") == 0) {
            ActWaterPump(3);
            PumpHaveActed = 1;
            DaysPassed = DaysPassed +1;
          } else if (strcmp(selectedPlant, "Cherry Tomatoes") == 0) {
            ActWaterPump(4);
            PumpHaveActed = 1;
            DaysPassed = DaysPassed +1;
          } else if (strcmp(selectedPlant, "Petunias") == 0) {
            ActWaterPump(5);
            PumpHaveActed = 1;
            DaysPassed = DaysPassed +1;
          } 
        }
        if (hour == 22 && minute == 31){
          PumpHaveActed = 0;
        }
      //}
      //////////////////////////////////////////////////////////
    } else {
      // Store selected plant
      strncpy(selectedPlant, incoming, sizeof(selectedPlant) - 1);
      selectedPlant[sizeof(selectedPlant) - 1] = '\0';
      Serial.print("Selected Plant Set To: ");
      Serial.println(selectedPlant);
    }
  }

  delay(100);
}
