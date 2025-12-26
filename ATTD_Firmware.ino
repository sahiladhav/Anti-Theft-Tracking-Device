#include <SoftwareSerial.h>
#include <TinyGPS.h>

// RX, TX pins for the GSM/GPS module communication
SoftwareSerial myGSM(3, 2); 
TinyGPS gps;

String textMessage;
float slat, slon;

void setup() {
  Serial.begin(9600);
  myGSM.begin(9600);
  
  Serial.print("System Initializing...");
  
  // Set GSM to Text Mode
  myGSM.println("AT+CMGF=1\r"); 
  delay(100);
  
  // Configure module to send SMS data to serial immediately
  myGSM.println("AT+CNMI=2,2,0,0,0\r"); 
  delay(100);
}

void loop() {
  // Read GPS data from Serial
  if (Serial.available() > 0) {
    int c = Serial.read();
    if (gps.encode(c)) {
      gps.f_get_position(&slat, &slon);
    }
  }

  // Check for incoming GSM messages
  if (myGSM.available() > 0) {
    textMessage = myGSM.readString();
    Serial.print("Received: " + textMessage);
    delay(10);
  }

  // Trigger: If "LOCATION" is received via SMS, reply with coordinates
  if (textMessage.indexOf("LOCATION") >= 0) {
    // Note: Replace with the target mobile number used in your project
    myGSM.println("AT+CMGS=\"+91XXXXXXXXXX\"\r"); 
    delay(100);
    
    // Construct Google Maps URL
    myGSM.print("Device Location: http://www.google.com/maps/place/");
    myGSM.print(slat, 6);
    myGSM.print(",");
    myGSM.print(slon, 6);
    
    // ASCII code 26 is the CTRL+Z command to send the SMS
    myGSM.write(26); 
    
    // Reset message string to avoid infinite loops
    textMessage = ""; 
  }
}
