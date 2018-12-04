/**
 * @mainpage AceTime Library
 *
 * This is the Doxygen documentation for the
 * <a href="https://github.com/bxparks/AceTime">AceTime Library</a>.
 */

#ifndef ACE_TIME_ACE_TIME_H
#define ACE_TIME_ACE_TIME_H

#include "ace_time/zonedb/zone_policies.h"
#include "ace_time/zonedb/zone_infos.h"
#include "ace_time/common/DateStrings.h"
#include "ace_time/LocalDate.h"
#include "ace_time/LocalTime.h"
#include "ace_time/UtcOffset.h"
#include "ace_time/OffsetDateTime.h"
#include "ace_time/ManualTimeZone.h"
#include "ace_time/ZoneAgent.h"
#include "ace_time/AutoTimeZone.h"
#include "ace_time/DateTime.h"
#include "ace_time/TimePeriod.h"
#include "ace_time/provider/TimeProvider.h"
#include "ace_time/provider/TimeKeeper.h"
#include "ace_time/provider/NtpTimeProvider.h"
#include "ace_time/provider/DS3231TimeKeeper.h"
#include "ace_time/provider/SystemTimeKeeper.h"
#include "ace_time/provider/SystemTimeSyncLoop.h"
#include "ace_time/provider/SystemTimeHeartbeatLoop.h"

// activate only if <AceRoutine.h> is included before this header
#ifdef ACE_ROUTINE_VERSION
  #include "ace_time/provider/SystemTimeSyncCoroutine.h"
  #include "ace_time/provider/SystemTimeHeartbeatCoroutine.h"
#endif

// Version format: xxyyzz == "xx.yy.zz", 100 == 0.1.0
#define ACE_TIME_VERSION 100

#endif
