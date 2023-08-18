

#include "o2Reg.h"

#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#include <FirebaseESP8266.h>
#endif

#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "minux2G"
#define WIFI_PASSWORD "0123456789han"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

#define API_KEY "AIzaSyDb5cR4OhypDR1IxbEtc9e2q_IAU3H0Sz0"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://ss-pjt001-default-rtdb.firebaseio.com/"
/* 2. Define the API Key */

/* 4. Define the user Email and password that alreadey registerd or added in your project */

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis;
unsigned long count ;
bool signupOK ;
char * devicePath = "device/oxy01/o2_val";

#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define pinBtnUp 25
#define pinBtnMenu 33
#define pinBtnDn 32
#define pinSenor 35
#define pinWiFiSet 26




bool WiFistatus = false;

void publish_data(char *devicePath, float valPressure);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  delay(50);
  pinMode(pinWiFiSet, INPUT_PULLUP);

//----------------------------------------

  bool res;
  res = wm.autoConnect("o2WIFI","CHANGEME"); // password protected ap
    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    }
    else {
        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
    }

//----------------------------------------
  
  config.api_key = API_KEY;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  // Assign the user sign in credentials
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;        // this is the anonymous sign in
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  // Buton Setup
  btnUp.begin(pinBtnUp);
  btnMenu.begin(pinBtnMenu);
  btnDn.begin(pinBtnDn);

  btnUp.setClickHandler(hndlr_btnUp);
  btnUp.setLongClickHandler(hndlr_btnUp);

  btnDn.setClickHandler(hndlr_btnDn);
  btnDn.setLongClickHandler(hndlr_btnDn);

  btnMenu.setClickHandler(hndlr_btnMenu);
  btnMenu.setLongClickHandler(hndlr_btnMenu);
  Serial.println("-- Button Setup");
  delay(500);

//----------------------------------------
tft.init();
tft.setRotation(1);
tft.fillScreen(TFT_BLACK);
tft.setTextColor(TFT_BLUE, TFT_WHITE);
tft.setTextSize(2);
tft.drawString("Welcome iO2",30, 120,4);
//----------------------------------------
  // Network Setup
  //
  Serial.println("-- Network Setup");
  delay(500);
  //Network Check
Serial.println("-- Network Check ");
//   CUR_MODE = RUNNING_MODE;
//  ESP_LOGW(TASK1_TAG,"hello loogoogogo from setup");
tft.fillScreen(TFT_BLACK);
}
void loop() {
    btnUp.loop();
    btnDn.loop();
    btnMenu.loop();
    update_lcd(CUR_MODE);

    /* if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 3000 || sendDataPrevMillis == 0)){ */
    /*     sendDataPrevMillis = millis(); */

    /*     publish_data(devicePath, random(0,10)/10+random(0,12)); */

}

void publish_data(char *devicePath, float valPressure){

    if (Firebase.RTDB.setInt(&fbdo, devicePath, valPressure)){
        Serial.println("PASSED");
        Serial.println("PATH : " + fbdo.dataPath());
        Serial.println("TYPE : " + fbdo.dataType());
    }
    else {
        Serial.println("FAILED");
        Serial.println("REASON : " + fbdo.errorReason());
    }
}
