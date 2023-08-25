#include "o2Reg.h"

// const int nMainMenu = 3;
// const int nSubMenu = 3;

int warnLowLevel = 3;
int warnHighLevel = 12;

WiFiManager wm;


//----------------------------------------
char MODE_ITEM[][20] = {"MEMU_MODE","BOOT_MODE","RUNNING_MODE","WARN_CHANGE_MODE","INFO_MODE","SETTING_MODE","NET_SETTING_MODE","NET_CHECK_MODE","REBOOT_MODE"};
//char MODE_SUB_SETTING_ITEM[3][20] =

char mainMenuItem[nMainMenu][12] = {"Inforamtion","Setting", "Return" };
char subMenuItem[nSubMenu][12] = {"Networks","Warn Level","Return" };
String deviceID ;

int nSelectedMainMenu = 0;
int nSelectedSubMenu = 0;

float pressureValue = 0.0;
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
TFT_eSprite img = TFT_eSprite(&tft);  // Declare Sprite object "spr" with pointer to "tft" object

typedef struct {
    double sensor;
    double liter;
} LinearMapping;

LinearMapping linearMappings[] = {
    {98, 0},
    {100, 1},
    {107, 2},
    {113, 3},
    {125, 4},
    {139, 5},
    {158, 6},
    {181, 7},
    {204, 8},
    {238, 9},
    {274, 10},
    {326, 11},
    {385, 12},
    {454, 13},
    {536, 14},
    {600, 15}
};

Mode_Type CUR_MODE = BOOT_MODE;

// ------------------------------------------------------------------

unsigned int  timeout   = 120; // seconds to run for
unsigned int  startTime = millis();
bool portalRunning      = false;
bool startAP            = false; // start AP and webserver if true, else start only webserver
bool wm_nonblocking = false;
// const int MIN_WARN_LEVEL = 3;  // default 최저 경고레벨
//
// const int MAX_WARN_LEVEL = 12; // default 최고 경고레벨
//
Adafruit_NeoPixel warnLED(NUMPIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);



 void (*functionPointers[])(void) = {
 DISPLAY_MENU_MODE ,
 DISPLAY_BOOT_MODE,
 DISPLAY_RUNNING_MODE,
 DISPLAY_WARN_CHANGE_MODE,
 DISPLAY_INFO_MODE ,
 DISPLAY_SETTING_MODE,
 DISPLAY_NET_SETTING_MODE,
 DISPLAY_NET_CHECK_MODE,
 DISPLAY_REBOOT_MODE,
};

void set_mode(Mode_Type _CUR_){
  CUR_MODE = _CUR_;
  Serial.printf("### CURRENT SET MODE from setmode(%s) \n", MODE_ITEM[CUR_MODE]);
  tft.fillScreen(TFT_BLACK);
  if(CUR_MODE == RUNNING_MODE){
    //tft.fillScreen(TFT_BLACK);
    img.fillScreen(TFT_BLACK);
  }
  else if(CUR_MODE == REBOOT_MODE){

    for(int i=0; i<3; i++){
      delay(500);
      Serial.printf("%d sec to reboot\n", i);
    }
  }
  else if(CUR_MODE == BOOT_MODE) {
  tft.fillScreen(TFT_BLACK);
   deviceID = ESP.getChipModel();
   Serial.println(String(deviceID));
  }
  else if(CUR_MODE == NET_SETTING_MODE) {
    update_lcd(CUR_MODE);

    if(!portalRunning) {

    Serial.println("NetPressed!");
      if(startAP){
        Serial.println("Button Pressed, Starting Config Portal");
        wm.setConfigPortalBlocking(false);
        wm.startConfigPortal();
      }
      else{
        Serial.println("Button Pressed, Starting Config Portal");

        wm.setConfigPortalBlocking(false);
        wm.startConfigPortal();
      }
      portalRunning = true;
      startTime = millis();
    }

}
}

// 25 pin
// #33 pin
void hndlr_btnMenu(Button2 &btn) {

  switch (btn.getType()) {
    case single_click:
      if (CUR_MODE == MENU_MODE && nSelectedMainMenu == 0) set_mode(INFO_MODE);
      else if (CUR_MODE == MENU_MODE && nSelectedMainMenu == 1) set_mode(SETTING_MODE);
      else if (CUR_MODE == MENU_MODE && nSelectedMainMenu == 2) set_mode(RUNNING_MODE);

      else if (CUR_MODE == SETTING_MODE && nSelectedSubMenu == 0) set_mode(NET_SETTING_MODE);
      else if (CUR_MODE == SETTING_MODE && nSelectedSubMenu == 1) set_mode(WARN_CHANGE_MODE);
      else if (CUR_MODE == SETTING_MODE && nSelectedSubMenu == 2) set_mode(RUNNING_MODE);

      else if (CUR_MODE == RUNNING_MODE) set_mode(MENU_MODE);
      else if (CUR_MODE == WARN_CHANGE_MODE ) set_mode(MENU_MODE);
      else if (CUR_MODE == INFO_MODE ) set_mode(MENU_MODE);
      else if (CUR_MODE == NET_SETTING_MODE) set_mode(REBOOT_MODE);
//      else if (CUR_MODE == WARN_CONFIRM_MODE) set_mode(RUNNING_MODE);
      break;

    case long_click:{
      if (CUR_MODE == WARN_CHANGE_MODE) set_mode(RUNNING_MODE);
      break;
    }
  }

  //update_display();//debug
  debug_out();//debug
}

void hndlr_btnUp(Button2 &btn) {
  switch (btn.getType()) {
    case single_click:
      if (CUR_MODE == MENU_MODE){
      nSelectedMainMenu = nSelectedMainMenu == 0 ? 0 : nSelectedMainMenu - 1;
      Serial.printf("### Selected MainMenu item: %d", nSelectedMainMenu);//debug
      }

      else if (CUR_MODE == WARN_CHANGE_MODE)
        warnLowLevel = warnLowLevel == MAX_WARN_LEVEL ? MAX_WARN_LEVEL : warnLowLevel + 1;

      else if (CUR_MODE == SETTING_MODE){

      nSelectedSubMenu = nSelectedSubMenu == 0 ? 0 : nSelectedSubMenu - 1;
        Serial.printf("### Selected Submenu item: %d", nSelectedSubMenu);//debug
      }
    case long_click:
      break;
  }

  Serial.printf("Selecte #: %d\n", nSelectedMainMenu);
  //update_display();//debug
  debug_out();//debug
}

void hndlr_btnDn(Button2 &btn) {
  switch (btn.getType()) {
  case single_click:
    if (CUR_MODE == MENU_MODE)
      nSelectedMainMenu = nSelectedMainMenu == nMainMenu - 1 ? nMainMenu - 1  : nSelectedMainMenu + 1;
    else if (CUR_MODE == SETTING_MODE)
nSelectedSubMenu = nSelectedSubMenu == nSubMenu - 1 ? nSubMenu - 1 : nSelectedSubMenu + 1;
    else if (CUR_MODE == WARN_CHANGE_MODE)
      warnLowLevel = warnLowLevel == MIN_WARN_LEVEL ? warnLowLevel : warnLowLevel - 1;

      Serial.printf("### Selected Sub Menu item: %d", nSelectedSubMenu);//debug
  case long_click:
    break;
  }

  //update_display();//debug
  debug_out(); //debug

}


// optional function to subscribe to MQTT topics

// optional function to process MQTT subscribed to topics coming in

bool initWiFi() {
  int cnt = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin("minux2G", "0123456789han");
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
    cnt++;
    if(cnt >= 10) break;
    else return false;
  }
  return true;

}

void update_display() {

  if (CUR_MODE == BOOT_MODE){

  }

  else if (CUR_MODE == MENU_MODE) {

    tft.fillScreen(TFT_BLACK);
    for (int i = 0; i < nMainMenu; i++) {
      if (i == nSelectedMainMenu) {
        tft.setTextSize(1.8);
        tft.setTextColor(TFT_WHITE, TFT_DARKGREEN);
        tft.drawString(mainMenuItem[i], 30, 60 * (i + 1),4);
      } else {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(1.5);
        tft.drawString(mainMenuItem[i], 30, 60 * (i + 1),4);
      }
    }
  }
  else if (CUR_MODE == SETTING_MODE) {
    Serial.printf("** CUR_MODE: %s \n", MODE_ITEM[CUR_MODE]);
    for (int i = 0; i < nSubMenu; i++) {
      if (i == nSelectedSubMenu){
        tft.setTextSize(1.8);
        tft.setTextColor(TFT_WHITE, TFT_DARKGREEN);
        tft.drawString(subMenuItem[i], 30, 60 * (i + 1),4);
      } else {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(1.5);
        tft.drawString(subMenuItem[i], 30, 60 * (i + 1),4);
      }
   }
  }


  else if (CUR_MODE == WARN_CHANGE_MODE) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(1.8);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString("Warn Level: " + String(warnLowLevel), 30, 50, 4);
        tft.drawString("Hole [M] to Set", 30, 80, 4);
  }

  // else if (CUR_MODE == WARN_CONFIRM_MODE) {
  //  Serial.printf(
  //      "*** SET WARN_LEVEL : %d *** \n Return to RUNNING_MODE in 3 sec\n",
  //      warnLowLevel);

  //       tft.fillScreen(TFT_BLACK);
  //       tft.setTextSize(1.8);
  //       tft.setTextColor(TFT_WHITE, TFT_BLACK);
  //       tft.drawString("SET WARN LVL: "+String(warnLowLevel), 30, 50, 4);
  //       delay(3000);

  //       set_mode(RUNNING_MODE);
  // }

  else if (CUR_MODE == RUNNING_MODE) {

        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(1.8);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(String(pressureValue)+" L/min ["+ String(warnLowLevel), 30, 50, 4);
  }

  else if (CUR_MODE == INFO_MODE) {
   Serial.printf(
       "INFORMATION\n -ID:xxxx1234\n -GasService:30L\n [M] to Return");
  }

  else if (CUR_MODE == NET_CHECK_MODE){
    Serial.printf("CHECKING NETWORK \n");
  }

  else if (CUR_MODE == REBOOT_MODE){
    Serial.printf("REBOOT 5,4,3,2,1");
}

}

void debug_out(){
  Serial.printf("------- %s -------\n", MODE_ITEM[CUR_MODE]);
  if (CUR_MODE == BOOT_MODE){
   Serial.printf("Welcome IO2 \n [M] to Set \n");
  }

  else if (CUR_MODE == MENU_MODE) {
    Serial.printf("** CUR_MODE: %s \n", MODE_ITEM[CUR_MODE]);
    for (int i = 0; i < nMainMenu; i++) {
      if (i == nSelectedMainMenu)
        Serial.printf("* %s\n", mainMenuItem[i]);
      else
        Serial.printf("%s\n", mainMenuItem[i]);
    }
  }

  else if (CUR_MODE == SETTING_MODE) {
    Serial.printf("** CUR_MODE: %s \n", MODE_ITEM[CUR_MODE]);
    for (int i = 0; i < nSubMenu; i++) {
      if (i == nSelectedSubMenu)
        Serial.printf("* %s\n", subMenuItem[i]);
      else
        Serial.printf("%s\n", subMenuItem[i]);
   }
  }



  else if (CUR_MODE == RUNNING_MODE) {
   Serial.printf(" %d L/min [%d]\n", pressureValue, warnLowLevel);
  }

  else if (CUR_MODE == INFO_MODE) {
   Serial.printf(
       "INFORMATION\n -ID:xxxx1234\n -GasService:30L\n [M] to Return");
  }

  else if (CUR_MODE == NET_CHECK_MODE){
    Serial.printf("CHECKING NETWORK \n");
  }

}

void DISPLAY_MENU_MODE(){

        tft.setTextDatum(TL_DATUM);
    for (int i = 0; i < nMainMenu; i++) {
      if (i == nSelectedMainMenu) {
        tft.setTextSize(1.8);
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        tft.drawString(mainMenuItem[i], 30, 60 * (i + 1),4);
      } else {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(1.5);
        tft.drawString(mainMenuItem[i], 30, 60 * (i + 1),4);
      }
    }
}
void DISPLAY_BOOT_MODE(){
        tft.setTextSize(2);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Welcome",120, 80,4);

        tft.setTextSize(2.6);
        tft.setTextColor(TFT_BLUE, TFT_BLACK);

        tft.drawString("iO2",120, 160,4);
}
void DISPLAY_RUNNING_MODE(){
        tft.setTextSize(2.5);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
       img.fillRect(0, 70, 240, 100 , TFT_BLACK);
       tft.drawFloat(pressureValue,1 ,120, 120, 7);

//        img.fillRect(0,80, 100, 240, TFT_BLACK);
        // img.setTextDatum(MC_DATUM);
        // img.drawString(String(pressureValue), 60, 60, 7);
        // img.pushSprite(120,120);
  // tft.setTextColor(TFT_WHITE,TFT_BLACK);

  // tft.setCursor(0, 60, 7);
  // tft.setTextSize(2.5);
  // tft.printf("%02.1f", pressureValue);

       tft.setTextSize(1.8);
       tft.setTextDatum(TC_DATUM);
       tft.setTextColor(TFT_WHITE, TFT_BLACK);
       tft.drawString(" L/min", 120, 200, 4);
       tft.setTextColor(TFT_ORANGE, TFT_BLACK);
       tft.drawString("[" + String(warnLowLevel) + "]", 170, 200, 4);
}

void DISPLAY_WARN_CHANGE_MODE(){
        tft.setTextSize(1.8);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString("Warn Level: " + String(warnLowLevel), 30, 50, 4);
        tft.drawString("Hole [M] to Set", 30, 80, 4);

}
void DISPLAY_INFO_MODE(){

        tft.setTextSize(1.4);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);

        tft.drawString("ID: " + String(ESP.getChipModel()), 30,50, 4);
        tft.drawString("SSID: "+ WiFi.SSID(), 30,90 , 4);
        tft.drawString("GAS:  40 L", 30, 130, 4);
}
void DISPLAY_SETTING_MODE(){
    for (int i = 0; i < nSubMenu; i++) {
      if (i == nSelectedSubMenu){
        tft.setTextSize(1.8);
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        tft.drawString(subMenuItem[i], 30, 60 * (i + 1),4);
      } else {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(1.5);
        tft.drawString(subMenuItem[i], 30, 60 * (i + 1),4);
      }
   }
}
void DISPLAY_NET_SETTING_MODE(){


        // tft.fillScreen(TFT_BLACK); //
        // tft.setTextSize(1.4);
        // tft.setTextColor(TFT_WHITE, TFT_BLACK);
        // tft.setTextDatum(MC_DATUM);
        // tft.drawString("SSID: ",120,60,4);
        // tft.drawString(String(ESP.getChipModel()), 120, 100, 4);
        // tft.setTextSize(1.2);
        // tft.drawString("Reboot After Setting",120, 140, 4);
}

void DISPLAY_NET_CHECK_MODE(){
        tft.setTextSize(1.8);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString("NETWORK CHECKING", 30, 50, 4);
}
void DISPLAY_REBOOT_MODE(){

        tft.setTextSize(1.8);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString("REBOOT", 30, 50, 4);
}
void DISPLAY_WARN_LOW_MODE(){

        tft.setTextSize(2.5);
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.setTextDatum(MC_DATUM);
        tft.drawFloat(pressureValue,1 ,120, 120, 7);
        tft.setTextSize(1.5);
        tft.setTextDatum(TC_DATUM);
        tft.drawString(" L/min [" + String(warnLowLevel) + "]", 120, 200, 4);
}

void DISPLAY_WARN_HIGH_MODE(){

        tft.setTextSize(2.5);
        tft.setTextColor(TFT_WHITE, TFT_ORANGE);
        tft.setTextDatum(MC_DATUM);
        tft.drawFloat(pressureValue,1 ,120, 120, 7);
        tft.setTextSize(1.5);
        tft.setTextDatum(TC_DATUM);
        tft.drawString(" L/min [" + String(warnLowLevel) + "]", 120, 200, 4);
}


void update_lcd(enum Mode_Type funcName){
    //  if (funcName >= MENU_MODE && funcName <= REBOOT_MODE) {
    //     functionPointers[funcName]();
    // } else {
    //     tft.drawString("Invalid function name.\n",100, 100, 6);
    // }
         functionPointers[funcName]();
}

void configModeCallback(WiFiManager *myWiFiManager){

        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(1.4);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("SSID: ",120,60,4);
        tft.drawString(String(ESP.getChipModel()), 120, 100, 4);
        tft.setTextSize(1.2);
        tft.drawString("Reboot After Setting",120, 140, 4);
  Serial.println("CallBack: Entering Config Mode");
}

double calculateLinearInterpolation(double x1, double y1, double x2, double y2, int x) {
    return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
}

float get_pressure() {
    int sensorValue;
    sensorValue = analogRead(pinSenor);
    float literValue = 0.0;

    for (int i = 0; i < sizeof(linearMappings) / sizeof(linearMappings[0]) - 1; i++) {
        if (sensorValue >= linearMappings[i].sensor && sensorValue <= linearMappings[i + 1].sensor) {
            literValue = calculateLinearInterpolation(
                linearMappings[i].sensor, linearMappings[i].liter,
                linearMappings[i + 1].sensor, linearMappings[i + 1].liter,
                sensorValue
            );
            break;
        }
    }

      return literValue;


}
void doWiFiManager(){
 if(portalRunning){
    wm.process(); // do processing

    // check for timeout
    if((millis()-startTime) > (timeout*1000)){
      Serial.println("portaltimeout");
      portalRunning = false;
      if(startAP){
        wm.stopConfigPortal();
      }
      else{
        wm.stopWebPortal();
      }
   }
  }

  // is configuration portal requested?
}




void checkWarn(){
  if(CUR_MODE == RUNNING_MODE) set_mode(RUNNING_MODE);
}


void doWarn(){
  warnLED.setBrightness(10);
  if(pressureValue >= warnHighLevel) {
    warnLED.fill(warnLED.Color(255,94,14), 0, NUMPIXELS);
    ledcWriteTone(0,NOTE_A4);
   // tone(pinBuzzer, NOTE_A4);
//    for (int i = 0; i < NUMPIXELS; i++)warnLED.setPixelColor(i, warnLED.Color(255, 94,14));
  }
  else if (pressureValue <= warnLowLevel){
    warnLED.fill(warnLED.Color(255,0, 0), 0, NUMPIXELS);

    ledcWriteTone(0,NOTE_A4);
    //tone(pinBuzzer, NOTE_A4);
 //   for (int i = 0; i < NUMPIXELS; i++)warnLED.setPixelColor(i, warnLED.Color(255, 0, 0));
      //낮을 때 할일
    }

  else {

    warnLED.fill(warnLED.Color(0 ,150, 0), 0, NUMPIXELS);

    ledcWrite(0,0);
    //noTone(pinBuzzer);
  //  for (int i = 0; i < NUMPIXELS; i++)warnLED.setPixelColor(i, warnLED.Color(0, 150, 0));
    //정상 일 때 할일
  }

  warnLED.show();

}
