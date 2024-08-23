#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

#define WIFI_SSID "Pranav"
#define WIFI_PASSWORD "20pranav04"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBoedn83R_SicL-yTc3_NMPqT5uSvIClWs"

#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://linear-range-317307-default-rtdb.firebaseio.com" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;


unsigned long sendDataPrevMillis = 0;
bool signupOK = false;


#define IR_PIN 26


void setup() {
  pinMode(IR_PIN, INPUT);
  pinMode(2,OUTPUT);
  pinMode(27,OUTPUT);
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
    digitalWrite(2,1);
  }
  else{
    digitalWrite(2,0);
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
int infoValue=0;
void infor(){
  if (Firebase.RTDB.getInt(&fbdo, "/test/info")) {
    infoValue = fbdo.intData();
    if (fbdo.dataType() == "int") {
      infoValue = fbdo.intData();
      Serial.println(infoValue);
      if (infoValue==1){
        digitalWrite(27,LOW);
      }
      else{
        digitalWrite(27,HIGH);
      }
}
}
}
void irs(){
  if (infoValue == 0) {
      int irRead = digitalRead(IR_PIN);
      if (irRead == LOW) {
        int irsensor = 1;
        if (Firebase.RTDB.setInt(&fbdo, "/test/irsensor", irsensor)) {
        } else {
          Serial.println("Failed to set irsensor value.");
        }
      }
      else{
        int irsensor = 0;
        if (Firebase.RTDB.setInt(&fbdo, "/test/irsensor", irsensor)) {
        } else {
          Serial.println("Failed to set irsensor value.");
        }
      }
    }
  else {
    int irsensor = 0;
        if (Firebase.RTDB.setInt(&fbdo, "/test/irsensor", irsensor)) {
        } else {
          Serial.println("Failed to set irsensor value.");
        }
    Serial.println("Failed to read 'info' from Firebase.");
    Serial.println("------------------------------------");
  }
}
void loop() {
  infor();
  irs();
  delay(1000); // Delay for stability, adjust as needed
}
