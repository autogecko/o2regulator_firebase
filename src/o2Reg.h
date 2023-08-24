#ifndef O2REG_H_
#define O2REG_H_

#endif // O2REG_H_
#include "Arduino.h"
#include "Button2.h"

#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
//#include "secrets.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "pitches.h"
#include <Adafruit_NeoPixel.h>


// ----------------------------------------
                        //
// --------------------
#define TFT_BLACK       0x0000
#define TFT_NAVY        0x000F
#define TFT_DARKGREEN   0x03E0
#define TFT_DARKCYAN    0x03EF
#define TFT_MAROON      0x7800
#define TFT_PURPLE      0x780F
#define TFT_OLIVE       0x7BE0
#define TFT_LIGHTGREY   0xC618
#define TFT_DARKGREY    0x7BEF
#define TFT_BLUE        0x001F
#define TFT_GREEN       0x07E0
#define TFT_CYAN        0x07FF
#define TFT_RED         0xF800
#define TFT_MAGENTA     0xF81F
#define TFT_YELLOW      0xFFE0
#define TFT_WHITE       0xFFFF
#define TFT_ORANGE      0xFDA0
#define TFT_GREENYELLOW 0xB7E0

#define USER_EMAIL "minux4it@gmail.com"
#define USER_PASSWORD "USER_PASSWORD"
#define pinBtnUp   25
#define pinBtnMenu 33
#define pinBtnDn   32
#define pinSenor   35
#define pinWiFiSet 26
#define pinSpeaker 13
#define pinBuzzer   13 // ESP32 pin GPIO18 connected to piezo buzzer
#define NEO_PIN    22      // 네오픽셀 DI핀과 연결된 핀번호 설정
#define NUMPIXELS  24     // 네오픽셀 LED 갯수 설정
//----------------------------------------
enum Mode_Type {
  MENU_MODE = 0,
  BOOT_MODE,
  RUNNING_MODE,
  WARN_CHANGE_MODE,
  INFO_MODE ,
  SETTING_MODE,
  NET_SETTING_MODE,
  NET_CHECK_MODE,
  REBOOT_MODE ,
};

extern void (*functionPointers[])(void) ;

extern Mode_Type CUR_MODE ;

#define nMainMenu 3
#define nSubMenu 3
#define MIN_WARN_LEVEL 3
#define MAX_WARN_LEVEL 12
#define RESET_BUTTON  26

//----------------------------------------
// WiFiManager

#ifdef ESP8266
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <ESPmDNS.h>
#endif

// select which pin will trigger the configuration portal when set to LOW
#define TRIGGER_PIN 25

//----------------------------------------------------


extern int warnHighLevel;
extern int warnLowLevel ;

extern int nSelectedMainMenu ;
extern int nSelectedSubMenu ;

extern char MODE_ITEM[10][20];;
//char MODE_SUB_SETTING_ITEM[3][20] =


extern char mainMenuItem[nMainMenu][12];
extern char subMenuItem[nSubMenu][12] ;

static Button2 btnUp, btnMenu, btnDn ;

extern WiFiManager wm;
extern TFT_eSPI tft;
extern float pressureValue;

extern unsigned int timeout;
extern unsigned int startTime;
extern bool portalRunning;
extern bool startAP;
extern bool wm_nonblocking;


extern int warnLowLevel;
extern int warnHighLevel;



extern Adafruit_NeoPixel warnLED;

extern
void set_mode(enum Mode_Type _CUR_);
void hndlr_btnUp(Button2 &btn);
void hndlr_btnMenu(Button2 &btn);
void hndlr_btnDn(Button2 &btn);
void update_display();
void update_display_temp();

void update_lcd(enum Mode_Type);
void debug_out();
void test_fbase();

void DISPLAY_MENU_MODE();
void DISPLAY_BOOT_MODE();
void DISPLAY_RUNNING_MODE();
void DISPLAY_WARN_CHANGE_MODE();
void DISPLAY_INFO_MODE();
void DISPLAY_SETTING_MODE();
void DISPLAY_NET_SETTING_MODE();
void DISPLAY_NET_CHECK_MODE();
void DISPLAY_REBOOT_MODE();
void DISPLAY_WARN_HIGH_MODE();
void DISPLAY_WARN_LOW_MODE();


float get_pressure();
void doWiFiManager();
void configModeCallback(WiFiManager *myWiFiManager);
void change_running_mode(float);
void checkWarn();
void doWarn();
