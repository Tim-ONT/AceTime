#ifndef ACE_TIME_ZONE_INFO_H
#define ACE_TIME_ZONE_INFO_H

#include <stdint.h>
#include "ZoneRule.h"

namespace ace_time {

/**
 * An entry in ZoneInfo which describe which rule the zone followed during
 * a particular time period, starting with fromYear.
 */
struct ZoneInfoEntry {

  /** UTC offset in 15 min increments */
  int8_t const offsetCode;

  /** Zone policy. NonNull. */
  const ZonePolicy* const zonePolicy;

  /** Zone abbreviations (e.g. PST, EST, etc) */
  const char* const format;

  /** Entry valid until year. 0 = 2000. */
  // TODO: Petersburg has month/day, do we need that?
  uint8_t const untilYear;
};

/** Data structure that represents a given time zone. */
struct ZoneInfo {
  /** Name of zone. */
  const char* const name; // name of zone

  /** ZoneInfoEntry records in increasing order of untilYear. */
  const ZoneInfoEntry* const entries;

  /** Number of ZoneInfoEntry records. */
  uint8_t const numEntries;
};

}

#endif
