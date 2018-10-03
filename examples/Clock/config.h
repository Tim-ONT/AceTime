#ifndef CLOCK_CONFIG_H
#define CLOCK_CONFIG_H

//------------------------------------------------------------------
// Configuration parameters.
//------------------------------------------------------------------

#define ENABLE_SERIAL 0

#define EEPROM_SIZE 32

#define DISPLAY_TYPE_LED 0
#define DISPLAY_TYPE_OLED 1
#define DISPLAY_TYPE_FULL_OLED 2

#ifndef AUNITER
  #define AUNITER_MICRO_MINDER
  #warning Using default AUNITER_MICRO_MINDER
#endif

#if defined(AUNITER_MICRO_MINDER)
  #define MODE_BUTTON_PIN 8
  #define CHANGE_BUTTON_PIN 9
  #define USE_DS3231
  #define DISPLAY_TYPE DISPLAY_TYPE_OLED
  #define OLED_REMAP false
#elif defined(AUNITER_NANO)
  #define MODE_BUTTON_PIN 2
  #define CHANGE_BUTTON_PIN 3
  #define USE_DS3231
  #define DISPLAY_TYPE DISPLAY_TYPE_FULL_OLED
  #define OLED_REMAP false
#elif defined(AUNITER_MINI_MINDER)
  #define MODE_BUTTON_PIN 2
  #define CHANGE_BUTTON_PIN 3
  #define USE_DS3231
  #define DISPLAY_TYPE DISPLAY_TYPE_FULL_OLED
  #define OLED_REMAP false
#elif defined(AUNITER_ESP_MINDER)
  #define MODE_BUTTON_PIN D4
  #define CHANGE_BUTTON_PIN D3
  #define USE_NTP
  #define DISPLAY_TYPE DISPLAY_TYPE_FULL_OLED
  #define OLED_REMAP false
#elif defined(AUNITER_ESP_MINDER2)
  #define MODE_BUTTON_PIN D4
  #define CHANGE_BUTTON_PIN D3
  #define USE_NTP
  #define DISPLAY_TYPE DISPLAY_TYPE_FULL_OLED
  #define OLED_REMAP true
#else
  #error Unknown AUNITER environment
#endif

#endif
