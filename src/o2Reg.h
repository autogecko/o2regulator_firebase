#ifndef O2REG_H_
#define O2REG_H_

#endif // O2REG_H_
#include "Arduino.h"
#include "Button2.h"

#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
//#include "secrets.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

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
#define pinBtnUp 25
#define pinBtnMenu 33
#define pinBtnDn 32
#define pinSenor 35
#define pinWiFiSet 26

enum Mode_Type {
  MENU_MODE = 0,
  BOOT_MODE,
  RUNNING_MODE,
  WARN_CHANGE_MODE,
  INFO_MODE ,
  SETTING_MODE,
  NET_SETTING_MODE,
  NET_CHECK_MODE,
  REBOOT_MODE = 8,
//  WARN_CONFIRM_MODE
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


extern int warnLevel ;
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
// extern "C" void set_mode(int _CUR_);
// extern "C" void hndlr_btnUp(Button2 &btn) ;
// extern "C" void hndlr_btnMenu(Button2 &btn);
// extern "C" void hndlr_btnDn(Button2 &btn) ;
// extern "C" void update_display();

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
// void DISPLAY_WARN_CONFIRM_MODE();
float get_pressure();
void doWiFiManager();
