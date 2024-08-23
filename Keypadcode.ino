#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

#define Password_Lenght 7
String abc="123456";
char Data[Password_Lenght]; // 6 is the number of chars it can hold + the null char = 7
char Master[Password_Lenght] = "123456";     //Change PASSWORD here
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

#include <LiquidCrystal.h>
const int rs = 25, en = 13, d4 = 5, d5 = 4, d6 = 15, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int count = 0;
char card_no[12];
int info=0;

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Pranav"
#define WIFI_PASSWORD "20pranav04"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBoedn83R_SicL-yTc3_NMPqT5uSvIClWs"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://linear-range-317307-default-rtdb.firebaseio.com" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS]= {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
bool door = true;
byte rowPins[ROWS] = {18, 19, 14, 27}; 
byte colPins[COLS] = {12, 33, 32, 22};   

Keypad customKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS );

String usern1="";
String usern2="";
String usern3="";

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup(){
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  pinMode(2,OUTPUT);
  Serial.begin(9600);
  lcd.print("Place the card:");
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
  /* Assign the api key (required) */
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    digitalWrite(2,1);
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
    digitalWrite(2,0);
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if(Serial.available())
   {
      count = 0;
      while(Serial.available() && count < 12)
      {
         card_no[count] = Serial.read();
         count++; 
         delay(5);
      }


      
      opens();

   }
   if (door == 0)
  {
    customKey = customKeypad.getKey();

    if (customKey == '#')

    {
      
    lcd.clear();
      lcd.setCursor (3,0);
        lcd.print("LOCKING");
        delay (700);
      lcd.setCursor (10,0);
        lcd.print(".");
        delay (800);
      lcd.setCursor (11,0);  
        lcd.print(".");
        delay (800);
      lcd.setCursor (12,0);
        lcd.print(".");
        delay (500);
        lcd.clear();
        delay (500);
      lcd.setCursor (5,0);
      lcd.print("LOCKED");
      delay (1700);
      
      door = 1;
    }
  }
  else Open();

 
}
String user1 = "";
String user2 = "";
String user3 = "";
int users = 0;
void trimEnd(char* inputArray, int numCharsToRemove) {
    int length = strlen(inputArray);

    if (length >= numCharsToRemove) {
        inputArray[length - numCharsToRemove] = '\0';  // Set the new end of the string
    } else {
    }
}
void dispuser(){
  if(Firebase.RTDB.getString(&fbdo, "/test/usern1")){
        if (fbdo.dataType() == "string") {
           usern1 = fbdo.stringData();
          }
        }
  if(Firebase.RTDB.getString(&fbdo, "/test/usern2")){
        if (fbdo.dataType() == "string") {
           usern2 = fbdo.stringData();
          }
        }
  if(Firebase.RTDB.getString(&fbdo, "/test/usern3")){
        if (fbdo.dataType() == "string") {
           usern3 = fbdo.stringData();
          }
        }
 }
 
void opens(){
  trimEnd(card_no, 1);
  dispuser();
  if (Firebase.RTDB.getString(&fbdo, "/test/USER1")) {
    if (fbdo.dataType() == "string") {
      user1 = fbdo.stringData();
      Serial.print(card_no);
      if (user1 == card_no) {
        users=1;
        clearData();
        pass();
      }
    }
  } else {
    Serial.println(fbdo.errorReason());
  } 

  if (Firebase.RTDB.getString(&fbdo, "/test/user2")) {
    if (fbdo.dataType() == "string") {
      user2 = fbdo.stringData();
      if (user2 == card_no) {
        users=2;
        pass();
      }
    }
  }

  if (Firebase.RTDB.getString(&fbdo, "/test/user3")) {
    if (fbdo.dataType() == "string") {
      user3 = fbdo.stringData();
      if (user3 == card_no) {
        users=3;
        pass();
      }
    }
  }
}


void clearData()
{
  while (data_count != 0)
  { // This can be used for any array size,
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}
int l=0;
void pass(){
  if (users==1){
    if(Firebase.RTDB.getString(&fbdo, "/test/pass1")){
        if (fbdo.dataType() == "string") {
           abc = fbdo.stringData();
           abc.replace("\"", ""); 
           abc.replace("\\", "");
           Serial.print(abc);
           abc.toCharArray(Master,Password_Lenght);
          }
        }
      }
   else if (users==2){
    if(Firebase.RTDB.getString(&fbdo, "/test/pass2")){
        if (fbdo.dataType() == "string") {
           abc = fbdo.stringData();
           abc.replace("\"", ""); 
           abc.replace("\\", "");
           Serial.print(abc);
           abc.toCharArray(Master,Password_Lenght);
          }
        }
      }
  if (users==3){
    if(Firebase.RTDB.getString(&fbdo, "/test/pass3")){
        if (fbdo.dataType() == "string") {
           abc = fbdo.stringData();
           abc.replace("\"", ""); 
           abc.replace("\\", "");
           Serial.print(abc);
           abc.toCharArray(Master,Password_Lenght);
          }
        }
      }
  }
 
void Open()
{
  info=0;
  if (Firebase.RTDB.setFloat(&fbdo, "/test/info",info))
  lcd.clear();
  if (users == 1){
    lcd.setCursor(1, 0);
    lcd.print(usern1);
  }
  else if(users == 2){
    lcd.setCursor(1, 0);
    lcd.print(usern2);
  }
  else if(users == 3){
    lcd.setCursor(1, 0);
    lcd.print(usern3);
  }
  lcd.setCursor(11, 0);
  lcd.print("PASS:");

  customKey = customKeypad.getKey();
  if (customKey) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
  {
    lcd.setCursor(0, 1); 
    lcd.print("******");     //To hide your PASSWORD, make sure its the same lenght as your password
    Data[data_count] = customKey; // store char into data array
    lcd.setCursor(data_count, 1); // move cursor to show each new char
    lcd.print(Data[data_count]); // print char at said cursor
    data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
  }

  if (data_count == Password_Lenght - 1) // if the array index is equal to the number of expected chars, compare data to master
  {
    if (!strcmp(Data, Master)) // equal to (strcmp(Data, Master) == 0)
    {
      lcd.clear();
      lcd.print(" ACCESS GRANTED");
      lcd.setCursor(0,1);
      info=1;
      if (Firebase.RTDB.setFloat(&fbdo, "/test/info",info))
      lcd.print("press # to close");
      door = 0 ;
    }
    else
    {
      lcd.clear();

     lcd.setCursor(1,0); 
      lcd.print("ACCESS DENIED!");
      info=0;
      if (Firebase.RTDB.setFloat(&fbdo, "/test/info",info))
      delay(1500);
      door = 1;
    }
    clearData();
  }
} 
