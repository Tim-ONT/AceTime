/*
A simple digital clock using:
  * a DS3231 RTC chip
  * a 7-segment LED display, OR an SSD1306 OLED display
  * 2 push buttons

Supported boards are:
  * Arduino Nano
  * Arduino Pro Mini
  * Arduino Leonardo (Pro Micro clone)
*/

#include <Wire.h>
#include <AceSegment.h>
#include <AceButton.h>
#include <AceRoutine.h>
#include <AceTime.h>
#include <ace_time/hw/CrcEeprom.h>
#include <SSD1306AsciiWire.h>
#include "config.h"
#include "LedDisplay.h"
#include "OledClock.h"
#include "FullOledClock.h"
#include "LedClock.h"

using namespace ace_segment;
using namespace ace_button;
using namespace ace_routine;
using namespace ace_time;

//------------------------------------------------------------------
// Configure CrcEeprom.
//------------------------------------------------------------------

hw::CrcEeprom crcEeprom;

//------------------------------------------------------------------
// Configure various TimeKeepers and TimeProviders.
//------------------------------------------------------------------

#if TIME_SOURCE_TYPE == TIME_SOURCE_TYPE_DS3231
  DS3231TimeKeeper dsTimeKeeper;
  SystemTimeKeeper systemTimeKeeper(&dsTimeKeeper, &dsTimeKeeper);
#elif TIME_SOURCE_TYPE == TIME_SOURCE_TYPE_NTP
  NtpTimeProvider ntpTimeProvider;
  SystemTimeKeeper systemTimeKeeper(&ntpTimeProvider, nullptr);
#elif TIME_SOURCE_TYPE == TIME_SOURCE_TYPE_BOTH
  DS3231TimeKeeper dsTimeKeeper;
  NtpTimeProvider ntpTimeProvider;
  SystemTimeKeeper systemTimeKeeper(&ntpTimeProvider, &dsTimeKeeper);
#else
  SystemTimeKeeper systemTimeKeeper(nullptr /*sync*/, nullptr /*backup*/);
#endif

SystemTimeSyncCoroutine systemTimeSync(systemTimeKeeper);
SystemTimeHeartbeatCoroutine systemTimeHeartbeat(systemTimeKeeper);

//------------------------------------------------------------------
// Configure OLED display using SSD1306Ascii.
//------------------------------------------------------------------

#if DISPLAY_TYPE == DISPLAY_TYPE_OLED || DISPLAY_TYPE == DISPLAY_TYPE_FULL_OLED

// OLED address: 0X3C+SA0 - 0x3C or 0x3D
#define OLED_I2C_ADDRESS 0x3C

SSD1306AsciiWire oled;

void setupOled() {
#if DISPLAY_TYPE == DISPLAY_TYPE_FULL_OLED
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
#else
  oled.begin(&Adafruit128x32, OLED_I2C_ADDRESS);
#endif
  oled.displayRemap(OLED_REMAP);
  oled.clear();
}

#endif

//------------------------------------------------------------------
// Configure LED display using AceSegment.
//------------------------------------------------------------------

// Use polling or interrupt for AceSegment
#define USE_INTERRUPT 0

#if DISPLAY_TYPE == DISPLAY_TYPE_LED

const uint8_t DIGIT_PINS[LedDisplay::NUM_DIGITS] = {4, 5, 6, 7};

#if defined(AUNITER_MINI_MINDER)
  #if LED_MODULE_TYPE == LED_MODULE_DIRECT
    static const uint8_t SEGMENT_PINS[LedDisplay::NUM_SEGMENTS] =
        {8, 9, 10, 11, 12, 14, 15, 13};
    static const uint8_t DRIVER_TYPE = LedDisplay::DRIVER_TYPE_DIRECT;
  #elif LED_MODULE_TYPE == LED_MODULE_SERIAL
    static const uint8_t LATCH_PIN = SS; // ST_CP on 74HC595
    static const uint8_t DATA_PIN = MOSI; // DS on 74HC595
    static const uint8_t CLOCK_PIN = SCK; // SH_CP on 74HC595
    static const uint8_t SEGMENT_PINS[3] = { LATCH_PIN, DATA_PIN, CLOCK_PIN };
    static const uint8_t DRIVER_TYPE = LedDisplay::DRIVER_TYPE_SERIAL;
  #else
    #error Unsupported LED_MODULE_TYPE
  #endif
#elif defined(AUNITER_MICRO_MINDER)
  #if LED_MODULE_TYPE == LED_MODULE_SERIAL
    static const uint8_t LATCH_PIN = 10; // ST_CP on 74HC595
    static const uint8_t DATA_PIN = MOSI; // DS on 74HC595
    static const uint8_t CLOCK_PIN = SCK; // SH_CP on 74HC595
    static const uint8_t SEGMENT_PINS[3] = { LATCH_PIN, DATA_PIN, CLOCK_PIN };
    static const uint8_t DRIVER_TYPE = LedDisplay::DRIVER_TYPE_SERIAL;
  #else
    #error Unsupported LED_MODULE_TYPE
  #endif
#else
  #error Unsupported BOARD
#endif

LedDisplay ledDisplay(LedDisplay::DRIVER_TYPE_SERIAL, DIGIT_PINS, SEGMENT_PINS);

#if USE_INTERRUPT == 1
  // interrupt handler for timer 2
  ISR(TIMER2_COMPA_vect) {
    ledDisplay.renderField();
  }
#else
  COROUTINE(renderLed) {
    COROUTINE_LOOP() {
      ledDisplay.renderFieldWhenReady();
      COROUTINE_YIELD();
    }
  }
#endif

void setupLed() {
#if USE_INTERRUPT == 1
  // set up Timer 2
  uint8_t timerCompareValue =
      (long) F_CPU / 1024 / ledDisplay->getFieldsPerSecond() - 1;
  #if ENABLE_SERIAL == 1
    Serial.print(F("Timer 2, Compare A: "));
    Serial.println(timerCompareValue);
  #endif

  noInterrupts();
  TCNT2  = 0;	// Initialize counter value to 0
  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2A |= bit(WGM21); // CTC
  TCCR2B |= bit(CS22) | bit(CS21) | bit(CS20); // prescale 1024
  TIMSK2 |= bit(OCIE2A); // interrupt on Compare A Match
  OCR2A =  timerCompareValue;
  interrupts();
#endif
}

#endif

//------------------------------------------------------------------
// Create an appropriate controller/presenter pair depending on
// the DISPLAY_TYPE.
//------------------------------------------------------------------

#if DISPLAY_TYPE == DISPLAY_TYPE_OLED
  OledPresenter presenter(oled);
  OledClock clock(systemTimeKeeper, crcEeprom, presenter);
#elif DISPLAY_TYPE == DISPLAY_TYPE_FULL_OLED
  FullOledPresenter presenter(oled);
  FullOledClock clock(systemTimeKeeper, crcEeprom, presenter);
#elif DISPLAY_TYPE == DISPLAY_TYPE_LED
  LedPresenter presenter(ledDisplay);
  LedClock clock(systemTimeKeeper, crcEeprom, presenter);
#else
  #error Invalid DISPLAY_TYPE
#endif

//------------------------------------------------------------------
// Render the Clock periodically.
//------------------------------------------------------------------

// The RTC has a resolution of only 1s, so we need to poll it fast enough to
// make it appear that the display is tracking it correctly. The benchmarking
// code says that clock.display() runs as fast as or faster than 1ms for all
// DISPLAY_TYPEs. So we can set this to 100ms without worrying about too
// much overhead.
COROUTINE(displayClock) {
  COROUTINE_LOOP() {
    clock.update();
    COROUTINE_DELAY(100);
  }
}

//------------------------------------------------------------------
// Configure AceButton.
//------------------------------------------------------------------

AdjustableButtonConfig modeButtonConfig;
AceButton modeButton(&modeButtonConfig);

AdjustableButtonConfig changeButtonConfig;
AceButton changeButton(&changeButtonConfig);

void handleModeButton(AceButton* /* button */, uint8_t eventType,
    uint8_t /* buttonState */) {
  switch (eventType) {
    case AceButton::kEventReleased:
      clock.modeButtonPress();
      break;
    case AceButton::kEventLongPressed:
      clock.modeButtonLongPress();
      break;
  }
}

void handleChangeButton(AceButton* /* button */, uint8_t eventType,
    uint8_t /* buttonState */) {
  switch (eventType) {
    case AceButton::kEventPressed:
      clock.changeButtonPress();
      break;
    case AceButton::kEventReleased:
      clock.changeButtonRelease();
      break;
    case AceButton::kEventRepeatPressed:
      clock.changeButtonRepeatPress();
      break;
  }
}

void setupAceButton() {
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(CHANGE_BUTTON_PIN, INPUT_PULLUP);

  modeButton.init(MODE_BUTTON_PIN);
  changeButton.init(CHANGE_BUTTON_PIN);

  modeButtonConfig.setEventHandler(handleModeButton);
  modeButtonConfig.setFeature(ButtonConfig::kFeatureLongPress);
  modeButtonConfig.setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);

  changeButtonConfig.setEventHandler(handleChangeButton);
  changeButtonConfig.setFeature(ButtonConfig::kFeatureLongPress);
  changeButtonConfig.setFeature(ButtonConfig::kFeatureRepeatPress);
  changeButtonConfig.setRepeatPressInterval(150);
}

COROUTINE(checkButton) {
  COROUTINE_LOOP() {
    modeButton.check();
    changeButton.check();
    COROUTINE_DELAY(10); // check button 100/sec
  }
}

//------------------------------------------------------------------
// Main setup and loop
//------------------------------------------------------------------

void setup() {
  // Wait for stability on some boards.
  // 1000ms needed for Serial.
  // 1500ms needed for Wire, I2C or SSD1306 (don't know which one).
  delay(2000);

  // Turn off the RX and TX LEDs on Leonardos
#if defined(ARDUINO_AVR_LEONARDO)
  RXLED0; // LED off
  TXLED0; // LED off
#endif

#if ENABLE_SERIAL == 1
  Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
  while (!Serial); // Wait until Serial is ready - Leonardo/Micro
  Serial.println(F("setup(): begin"));
#endif

  Wire.begin();
  Wire.setClock(400000L);
  crcEeprom.begin(EEPROM_SIZE);

  setupAceButton();

#if DISPLAY_TYPE == DISPLAY_TYPE_LED
  setupLed();
#else
  setupOled();
#endif

#if TIME_SOURCE_TYPE == TIME_SOURCE_TYPE_DS3231
  dsTimeKeeper.setup();
#elif TIME_SOURCE_TYPE == TIME_SOURCE_TYPE_NTP
  ntpTimeProvider.setup(AUNITER_SSID, AUNITER_PASSWORD);
#elif TIME_SOURCE_TYPE == TIME_SOURCE_TYPE_BOTH
  dsTimeKeeper.setup();
  ntpTimeProvider.setup(AUNITER_SSID, AUNITER_PASSWORD);
#endif
  systemTimeKeeper.setup();
  clock.setup();

  systemTimeSync.setupCoroutine(F("systemTimeSync"));
  systemTimeHeartbeat.setupCoroutine(F("systemTimeHeartbeat"));
  CoroutineScheduler::setup();

#if ENABLE_SERIAL == 1
  Serial.println(F("setup(): end"));
#endif
}

void loop() {
  CoroutineScheduler::loop();
}
