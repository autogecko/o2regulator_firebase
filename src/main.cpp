

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
#define WIFI_SSID "minux12"
#define WIFI_PASSWORD "vivaminux"

#define TRIGGER_PIN 26
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

int tDelay_FireBase = 2000;
int tDelay_Sensing= 1000;
int tNow_FireBase = 0;
int tNow_Sensing = 0;


//WiFiManager wm;

void publish_data(char *devicePath, float valPressure);

int WiFiStatus;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  delay(50);
  pinMode(pinWiFiSet, INPUT_PULLUP);

  // 센서 analogRead 설정
  ledcSetup(0,1E5,12);  // ESP32 톤전용 ledcSetup: ledcSetup(channel,1E5,12);
  ledcAttachPin(pinBuzzer,0);  // ledcAttachPin(uint8_t pin, uint8_t channel);



// WiFiManger Setup
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  wm.setHostname("MDNSEXAMPLE");
  wm.setConfigPortalTimeout(30); // auto close configportal after n seconds
  if(wm_nonblocking) wm.setConfigPortalBlocking(false);
  WiFiStatus = WiFi.status();
  wm.setAPCallback(configModeCallback);
  wm.autoConnect();
  Serial.setDebugOutput(true);
  //----------------------------------------

// TFT Setup
  tft.init(TFT_BLACK);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  spr.createSprite(SWIDTH, SHEIGHT);
  tft.fillScreen(TFT_BLACK);

//----------------------------------------
  set_mode(BOOT_MODE);
  update_lcd(CUR_MODE);


// WiFi Debugging
  Serial.print("WiFiStatus: ");
  Serial.println(String(WiFiStatus));
  Serial.println(WiFi.localIP());

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
//----------------------------------------


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
  warnLED.begin();
  warnLED.clear();

//delay(2000);
  set_mode(RUNNING_MODE);

}
void loop() {
    btnUp.loop();
    btnDn.loop();
    btnMenu.loop();
    doWiFiManager();
//    checkWarn();
    update_lcd(CUR_MODE);

    if(Firebase.ready() && signupOK && (millis() - tNow_FireBase) > tDelay_FireBase ){
        tNow_FireBase = millis();
        publish_data(devicePath,pressureValue );
        Serial.println("Value: "+ String(pressureValue));


    }
    if(millis() - tNow_Sensing > tDelay_Sensing){
        tNow_Sensing = millis();
        pressureValue = roundUpToDecimal(get_pressure());
//        if(pressureValue >= warnHighLevel || pressureValue <= warnLowLevel) set_mode(WARN_MODE);

        doWarn();
        Serial.println("ana: "+ String(analogRead(pinSenor)));
      //  Serial.println("pressure: "+ String(pressureValue));
        Serial.printf("pressure: %.1f \n",pressureValue);
    }
}

void publish_data(char *devicePath, float valPressure){
/* publish firebase */
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

