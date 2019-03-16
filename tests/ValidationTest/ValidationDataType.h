#ifndef VALIDATION_DATA_TYPE_H
#define VALIDATION_DATA_TYPE_H

#include <AceTime.h>

using namespace ace_time;

/** The epochSecond and the expected UTC offset and dateTime components. */
struct ValidationItem {
  acetime_t const epochSeconds;
  int16_t const utcOffsetMinutes;
  int16_t const deltaOffsetMinutes;
  int8_t const yearTiny;
  uint8_t const month;
  uint8_t const day;
  uint8_t const hour;
  uint8_t const minute;
  uint8_t const second;
};

/**
 * Header that points to an array of all ValidationItems and the specific
 * zonedb::ZoneInfo that is being tested.
 */
struct ValidationData {
  const zonedb::ZoneInfo* const zoneInfo;
  uint16_t const numItems;
  const ValidationItem* const items;
};

#endif
