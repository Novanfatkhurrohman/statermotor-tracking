#include "SIM900.h"
#include "sms.h"
#include "TinyGPS.h"
#include <Adafruit_Fingerprint.h> 
#include <SoftwareSerial.h>
SoftwareSerial SIM800L(4, 5);
SoftwareSerial mySerial(2, 3);
SoftwareSerial GPS(11, 12);
TinyGPS neo6m;
int relay = 6;
String response;
String _buffer;
int _timeout;
String txt;
String password;
String yourPassword = "Location";
int lastStringLength = response.length();
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
void setup()  
{
  Serial.begin(9600);
   _buffer.reserve(50);
  delay(100);
  SIM800L.begin(9600);
  delay(500);
  GPS.begin(9600);
   SIM800L.println("AT+CMGR=\"082298401004\"\r");
  delay(500);
  while (!Serial); 
  delay(100);
  pinMode(13,OUTPUT);
  pinMode(relay, OUTPUT);
  Serial.println("Arduino with SIM800L is ready");
  SIM800L.begin(9600);  
  SIM800L.println("AT+CMGF=1");
  Serial.println("SIM800L started at 9600");
  delay(1000);
  Serial.println("Setup Complete! SIM800L is Ready!");
  SIM800L.println("AT+CNMI=2,2,0,0,0");
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  Serial.println("Waiting for valid finger...");
}
void loop()                     
{ while (GPS.available())
  { if (neo6m.encode(GPS.read()));
    {
      float latitude, longitude;
      neo6m.f_get_position(&latitude, &longitude);
      float lat = latitude;
      float lon = longitude;
      float veloc;
      veloc = neo6m.f_speed_kmph();   //km/h
      String slat = String(lat);
      String slon = String(lon);
      String svel = String(veloc);
      String clat = " La ";
      String clon = " Lo ";
      String cvel = " Ve ";
      String txt = clat+slat+clon+slon+cvel+svel;
      Serial.println(txt);
  if (SIM800L.available()) 
  {  
    SIM800L.println("AT+CNMI=2,2,0,0,0");
    password = "";
    char c;
    while(c=SIM800L.read())
    { password += c; }
    Serial.println(password); 
    Serial.println("\nEND OF MESSAGE"); 
    SIM800L.flush(); 
    Serial.println("MESSAGE DELETED"); 
    if (password == yourPassword)
    {
      Serial.println("\nPASSWORD VALID"); 
      SIM800L.println("AT+CMGF=1");
      delay(1000);
      SIM800L.println("AT+CMGS=\"+082298401004\"\r");
      delay(1000);
      SIM800L.println(txt);
      delay(1000);
      SIM800L.println((char)26);
      delay(1000);
      SIM800L.println();
    }
    else Serial.println("\nPASSWORD NOT VALID"); 
  }
 }
} if (Serial.available()){
      SIM800L.write(Serial.read());      
    }
  getFingerprintID();
  delay(50);            

    if (SIM800L.available()>0){
      response = SIM800L.readStringUntil('\n');
    }
    if(lastStringLength != response.length()){
      Serial.println(response);
      Serial.println(response.indexOf("ON"));
      if(response.indexOf("ON") == 6){
        digitalWrite(relay,LOW);
        KirimHidup();
        Serial.println("MOTOR ON");
      }
      if(response.indexOf("OFF") == 6){
        digitalWrite(relay,HIGH);
        KirimMati();
        Serial.println("MOTOR OFF");
      }
      lastStringLength = response.length();
    }
}
void KirimHidup(){
  SIM800L.println("AT+CMGF=1");    
  delay(1000); 
  SIM800L.println("AT+CMGS=\"082298401004\"\r"); 
  delay(1000);
  SIM800L.println("Motor Hidup");
  delay(100);
  SIM800L.println((char)26);
  delay(1000);
}
void KirimMati(){
  SIM800L.println("AT+CMGF=1"); 
  delay(1000);  
  SIM800L.println("AT+CMGS=\"082298401004\"\r"); 
  delay(1000);
  SIM800L.println("Motor Mati");
  SIM800L.println((char)26);
  delay(1000);
} 
 
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println(".");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  // OK success!
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    delay(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  digitalWrite(13,HIGH);
  delay(3000);
  digitalWrite(13,LOW);
  return finger.fingerID;
}
