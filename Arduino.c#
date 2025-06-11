#include <SoftwareSerial.h>
SoftwareSerial abc(9, 10); // RX = 9, TX = 10

char plant[20]
void setup() {
  pinMode(13,OUTPUT);
  Serial.begin(115200);      
  abc.begin(115200);        
  Serial.println("Arduino Ready");
  

}
// --recive plant from ESP--//
void loop() {
  if (ABC.available()) {
    byte len = ABC.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
    buffer[len] = '\0'; 

    Serial.print("Received: ");
    Serial.println(buffer);
  }
}
    if (plant == "Mint"){
      int mlPerDay = 180;
      int WaterTime = (mlPerDay * 16);
      
    }
    if (plant == "Rosemary"){
      int mlPerDay = 180;
      int WaterTime = (mlPerDay * 16);
    }
    if (plant == "Lettuce"){
      int mlPerDay = 180;
      int WaterTime = (mlPerDay * 16);
    }
    if (plant == "Cherry Tomatoes"){
      int mlPerDay = 180;
      int WaterTime = (mlPerDay * 16);
    }
    if (plant == "Petunias"){
      int mlPerDay = 180;
      int WaterTime = (mlPerDay * 16);
    }
    ///////////////////////////
    
  }
}
