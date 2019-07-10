# AceTime

This library provides date and time classes for the Arduino platform with
support for time zones in the [IANA TZ
database](https://www.iana.org/time-zones). Date and time from one timezone can
be converted to another timezone. The library also provides a SystemClock that
can be synchronized from a more reliable external time source, such as an
[NTP](https://en.wikipedia.org/wiki/Network_Time_Protocol) server or a [DS3231
RTC](https://www.maximintegrated.com/en/products/analog/real-time-clocks/DS3231.html)
chip. This library is can be an alternative to the [Arduino
Time](https://github.com/PaulStoffregen/Time) and [Arduino
Timezone](https://github.com/JChristensen/Timezone) libraries.

The AceTime classes are organized into roughly 4 bundles, placed in different
C++ namespaces:

* date and time classes and types
    * `ace_time::acetime_t`
    * `ace_time::LocalTime`
    * `ace_time::LocalDate`
    * `ace_time::LocalDateTime`
    * `ace_time::TimeOffset`
    * `ace_time::OffsetDateTime`
    * `ace_time::ZoneSpecifier`
        * `ace_time::ManualZoneSpecifier`
        * `ace_time::BasicZoneSpecifier`
        * `ace_time::ExtendedZoneSpecifier`
    * `ace_time::TimeZone`
    * `ace_time::ZonedDateTime`
    * `ace_time::BasicZoneManager` [`*`]
    * `ace_time::ExtendedZoneManager` [`*`]
    * `ace_time::TimePeriod`
    * mutation helpers
        * `ace_time::local_date_mutation::`
        * `ace_time::time_offset_mutation::`
        * `ace_time::time_period_mutation::`
        * `ace_time::zoned_date_time_mutation::`
* TZ Database zone files
    * C++ files generated by a code-generator from the TZ Database zone files
    * `ace_time::zonedb` (270 zones and 182 links for `BasicZoneSpecifier`)
        * `zonedb::kZoneAfrica_Abidjan`
        * `zonedb::kZoneAfrica_Accra`
        * ...
        * `zonedb::kZonePacific_Wake`
        * `zonedb::kZonePacific_Wallis`
    * `ace_time::zonedbx` (387 zones and 205 links for `ExtendedZoneSpecifier`)
        * `zonedbx::kZoneAfrica_Abidjan`
        * `zonedbx::kZoneAfrica_Accra`
        * ...
        * `zonedbx::kZonePacific_Wake`
        * `zonedbx::kZonePacific_Wallis`
* system clock classes
    * `ace_time::clock::TimeProvider`
        * `ace_time::clock::TimeKeeper`
            * `ace_time::clock::DS3231TimeKeeper`
            * `ace_time::clock::SystemClock`
        * `ace_time::clock::NtpTimeProvider`
    * `ace_time::clock::SystemClockHeartbeatCoroutine`
    * `ace_time::clock::SystemClockHeartbeatLoop`
    * `ace_time::clock::SystemClockSyncCoroutine`
    * `ace_time::clock::SystemClockSyncLoop`
* helper and common classes
    * `ace_time::common::DateStrings`
    * `ace_time::common::ZoneContext`
    * `ace_time::basic::ZoneInfo`
    * `ace_time::basic::ZoneEra`
    * `ace_time::basic::ZonePolicy`
    * `ace_time::basic::ZoneRule`
    * `ace_time::extended::ZoneInfo`
    * `ace_time::extended::ZoneEra`
    * `ace_time::extended::ZonePolicy`
    * `ace_time::extended::ZoneRule`

[`*`] - Experimental

The "date and time" classes provide an abstraction layer to make it easier
to use and manipulate date and time fields. For example, each of the
`LocalDateTime`, `OffsetDateTime` and `ZonedDateTime` classes provide the
`toEpochSeconds()` which returns the number of seconds from a epoch date, the
`forEpochSeconds()` which constructs the date and time fields from the epoch
seconds, the `forComponents()` method which constructs the object from the
individual (year, month, day, hour, minute, second) components, and the
`dayOfWeek()` method which returns the day of the week of the given date.

The Epoch in AceTime is defined to be 2000-01-01T00:00:00Z, in contrast to the
Epoch in Unix which is 1970-01-01T00:00:00Z. Internally, the current time is
represented as "seconds from Epoch" stored as a 32-bit signed integer
(`int32_t`). Therefore, the maximum date that AceTime can handle is
2068-01-19T03:14:07Z (compared to the 32-bit Unix maximum of
2038-01-19T03:14:07Z.) The `ZonedDateTime` class works with the `TimeZone` class
to provide access to the TZ Database and allow conversions to other timezones
using the `ZonedDateTime::convertToTimeZone()` method.

The library provides 2 sets of zoneinfo files created from the IANA TZ Database:

* [zonedb/zone_infos.h](src/ace_time/zonedb/zone_infos.h) contains `kZone*`
  declarations for 270 zones and 182 links which have (relatively) simple time
  zone transition rules, and intended to be used by `BasicZoneSpecifier` class,
  and
* [zonedbx/zone_infos.h](src/ace_time/zonedbx/zone_infos.h) contains `kZone*`
  declarations for 387 zones and 205 links in the TZ Database (essentially
  the entire database) intended to be used by the `ExtendedZoneSpecifier` class.

These zoneinfo files have been validated to match the UTC offsets calculated
using the Python [pytz](https://pypi.org/project/pytz/) library from the year
2000 until 2037 (inclusive), and using the [Java 11
Time](https://docs.oracle.com/en/java/javase/11/docs/api/java.base/java/time/package-summary.html)
library from year 2000 to 2049 (inclusive). Custom datasets with smaller or
larger range of years may be generated by developers using scripts provided in
this library. The target application may be compiled against the custom dataset
instead of using `zonedb::` and `zonedbx::` zone files provided in this library.

It is expected that an application using AceTime will use only a small subset of
these timezones at the same time (1 to 3 zones have been tested). The C++
compiler will include only the subset of zoneinfo files needed to support those
timezones, instead of compiling in the entire TZ database.

The `ace_time::clock` classes collaborate together to implement the
SystemClock which can obtain its time from various sources, such as a DS3231 RTC
chip, or an Network Time Protocol (NTP) server. Retrieving the current time
from accurate clock sources can be expensive, so the `SystemClock` uses the
built-in `millis()` function to provide fast access to a reasonably accurate
clock, but synchronizes to more accurate clocks periodically.

This library does not perform dynamic allocation of memory, in other words,
it does not call the `new` operator nor the `malloc()` function, and it does not
use the Arduino `String` class. Everything it needs is allocated statically at
initialization time. On an 8-bit Arduino Nano (AVR), the AceTime library with
one timezone using the `BasicZoneSpecifier` and the `SystemClock` consumes about
8kB of flash and 350 bytes of RAM. On an ESP8266 processor (32-bit) the same
functionality consumes 11kB of flash and 850 bytes of RAM. Loading all 270
timezones supported by `BasicZoneSpecifier` takes about 14kB of flash on an
8-bit processor and about 21kB of flash on a 32-bit processor. The fully
functioning [WorldClock](examples/WorldClock) with 3 OLED displays, 3 timezones,
a SystemClock synchronized to a DS3231 chip, and 2 buttons can fit inside the
30kB flash size of an Arduino Pro Micro controller.

Conversion from date-time components (year, month, day, etc) to epochSeconds
(`ZonedDateTime::toEpochSeconds()`) takes about:
* 90 microseconds on an 8-bit AVR processor,
* 7 microseconds on an ESP8266,
* 1.4 microseconds on an ESP32,
* 0.5 microseconds on a Teensy 3.2.

Conversion from an epochSeconds to date-time components including timezone
(`ZonedDateTime::forEpochSeconds()`) takes:
* 600 microseconds on an 8-bit AVR,
* 25 microseconds on an ESP8266,
* 2.5 microseconds on an ESP32,
* 6 microseconds on a Teensy 3.2.

if we get hits of the internal transitions cache.

**Version**: 0.4 (2019-07-09, TZ DB version 2019a, beta)

**Status**: Fully functional with an API that is expected to be stable for all
classes except for the `ZoneManager` classes.

## HelloDateTime

Here is a simple program (see [examples/HelloDateTime](examples/HelloDateTime))
which demonstrates how to create and manipulate date and times in different time
zones:

```C++
#include <AceTime.h>

using namespace ace_time;

// ZoneSpecifier instances should be created statically at initialization time.
static BasicZoneSpecifier pacificSpec(&zonedb::kZoneAmerica_Los_Angeles);
static BasicZoneSpecifier londonSpec(&zonedb::kZoneEurope_London);

void setup() {
  delay(1000);
  Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
  while (!Serial); // Wait until Serial is ready - Leonardo/Micro

  auto pacificTz = TimeZone::forZoneSpecifier(&pacificSpec);
  auto londonTz = TimeZone::forZoneSpecifier(&londonSpec);

  // Create from components. 2019-03-10T03:00:00 is just after DST change in
  // Los Angeles (2am goes to 3am).
  auto startTime = ZonedDateTime::forComponents(
      2019, 3, 10, 3, 0, 0, pacificTz);

  Serial.print(F("Epoch Seconds: "));
  acetime_t epochSeconds = startTime.toEpochSeconds();
  Serial.println(epochSeconds);

  Serial.print(F("Unix Seconds: "));
  acetime_t unixSeconds = startTime.toUnixSeconds();
  Serial.println(unixSeconds);

  Serial.println(F("=== Los_Angeles"));
  auto pacificTime = ZonedDateTime::forEpochSeconds(epochSeconds, pacificTz);
  Serial.print(F("Time: "));
  pacificTime.printTo(Serial);
  Serial.println();

  Serial.print(F("Day of Week: "));
  Serial.println(
      common::DateStrings().dayOfWeekLongString(pacificTime.dayOfWeek()));

  // Print info about UTC offset
  TimeOffset offset = pacificTime.timeOffset();
  Serial.print(F("Total UTC Offset: "));
  offset.printTo(Serial);
  Serial.println();

  // Print info about the current time zone
  Serial.print(F("Zone: "));
  pacificTz.printTo(Serial);
  Serial.println();

  // Print the current time zone abbreviation, e.g. "PST" or "PDT"
  Serial.print(F("Abbreviation: "));
  pacificTz.printAbbrevTo(Serial, epochSeconds);
  Serial.println();

  // Create from epoch seconds. London is still on standard time.
  auto londonTime = ZonedDateTime::forEpochSeconds(epochSeconds, londonTz);

  Serial.println(F("=== London"));
  Serial.print(F("Time: "));
  londonTime.printTo(Serial);
  Serial.println();

  // Print info about the current time zone
  Serial.print(F("Zone: "));
  londonTz.printTo(Serial);
  Serial.println();

  // Print the current time zone abbreviation, e.g. "PST" or "PDT"
  Serial.print(F("Abbreviation: "));
  londonTz.printAbbrevTo(Serial, epochSeconds);
  Serial.println();

  Serial.println(F("=== Compare ZonedDateTime"));
  Serial.print(F("pacificTime.compareTo(londonTime): "));
  Serial.println(pacificTime.compareTo(londonTime));
  Serial.print(F("pacificTime == londonTime: "));
  Serial.println((pacificTime == londonTime) ? "true" : "false");
}

void loop() {
}
```

Running this should produce the following on the Serial port:
```
Epoch Seconds: 605527200
Unix Seconds: 1552212000
=== Los Angeles
Time: 2019-03-10T03:00:00-07:00[America/Los_Angeles]
Day of Week: Sunday
Total UTC Offset: -07:00
Zone: America/Los_Angeles
Abbreviation: PDT
=== London
Time: 2019-03-10T10:00:00+00:00[Europe/London]
Zone: Europe/London
Abbreviation: GMT
=== Compare ZonedDateTime
pacificTime.compareTo(londonTime): 0
pacificTime == londonTime: false
```

## HelloSystemClock

This is the example code for using the `SystemClock` taken from
[examples/HelloSystemClock](examples/HelloSystemClock).

```C++
#include <AceTime.h>

using namespace ace_time;
using namespace ace_time::clock;

// ZoneSpecifier instances should be created statically at initialization time.
static BasicZoneSpecifier pacificSpec(&zonedb::kZoneAmerica_Los_Angeles);

SystemClock systemClock(nullptr /*sync*/, nullptr /*backup*/);
SystemClockHeartbeatLoop systemClockHeartbeat(systemClock);

//------------------------------------------------------------------

void setup() {
  delay(1000);
  Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
  while (!Serial); // Wait until Serial is ready - Leonardo/Micro

  systemClock.setup();

  // Creating timezones is cheap, so we can create them on the fly as needed.
  auto pacificTz = TimeZone::forZoneSpecifier(&pacificSpec);

  // Set the SystemClock using these components.
  auto pacificTime = ZonedDateTime::forComponents(
      2019, 6, 17, 19, 50, 0, pacificTz);
  systemClock.setNow(pacificTime.toEpochSeconds());
}

//------------------------------------------------------------------

void printCurrentTime() {
  acetime_t now = systemClock.getNow();

  // Create a time
  auto pacificTz = TimeZone::forZoneSpecifier(&pacificSpec);
  auto pacificTime = ZonedDateTime::forEpochSeconds(now, pacificTz);
  pacificTime.printTo(Serial);
  Serial.println();
}

void loop() {
  printCurrentTime();
  systemClockHeartbeat.loop(); // should be called every 65.535s or less
  delay(2000);
}
```

This will start by setting the SystemClock to 2019-06-17T19:50:00-07:00,
then printing the system time every 2 seconds:
```
2019-06-17T19:50:00-07:00[America/Los_Angeles]
2019-06-17T19:50:02-07:00[America/Los_Angeles]
2019-06-17T19:50:04-07:00[America/Los_Angeles]
```

## Example: WorldClock

Here is a photo of the [WorldClock](examples/WorldClock) that supports 3
OLED displays with 3 timezones, and automatically adjusts the DST transitions
for all 3 zones:

![WorldClock](examples/WorldClock/WorldClock.jpg)

## User Guide

See the [AceTime User Guide](USER_GUIDE.md) for information on:
* Installation
* Date and Time classes
* Mutations
* System Clock classes
* Error Handling
* Benchmarks
* Comparison to Other Libraries
* Bugs and Limitations

## System Requirements

### Tool Chain

This library was developed and tested using:

* [Arduino IDE 1.8.9](https://www.arduino.cc/en/Main/Software)
* [AVR Core 1.6.23](https://github.com/arduino/ArduinoCore-avr)
* [ESP8266 Arduino Core 2.5.2](https://github.com/esp8266/Arduino)
* [ESP32 Arduino Core 1.0.2](https://github.com/espressif/arduino-esp32)
* [Teensydino 1.46](https://www.pjrc.com/teensy/td_download.html)

It should work with [PlatformIO](https://platformio.org/) but I have
not tested it.

### Operating System

I use Ubuntu 18.04 for the vast majority of my development. I expect that the
library will work fine under MacOS and Windows, but I have not tested them.

### Hardware

The library is tested on the following hardware before each release:

* Arduino Nano clone (16 MHz ATmega328P)
* Arduino Pro Micro clone (16 MHz ATmega32U4)
* WeMos D1 Mini clone (ESP-12E module, 80 MHz ESP8266)
* ESP32 dev board (ESP-WROOM-32 module, 240 MHz dual core Tensilica LX6)

I will occasionally test on the following hardware as a sanity check:

* Teensy 3.2 (72 MHz ARM Cortex-M4)

## Changelog

See [CHANGELOG.md](CHANGELOG.md).

## License

[MIT License](https://opensource.org/licenses/MIT)

## Feedback and Support

If you have any questions, comments, bug reports, or feature requests, please
file a GitHub ticket instead of emailing me unless the content is sensitive.
(The problem with email is that I cannot reference the email conversation when
other people ask similar questions later.) I'd love to hear about how this
software and its documentation can be improved. I can't promise that I will
incorporate everything, but I will give your ideas serious consideration.

## Authors

Created by Brian T. Park (brian@xparks.net).
