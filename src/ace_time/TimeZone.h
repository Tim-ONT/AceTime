#ifndef ACE_TIME_TIME_ZONE_H
#define ACE_TIME_TIME_ZONE_H

#include <stdint.h>
#include "TimeOffset.h"
#include "ZoneSpecifier.h"
#include "ManualZoneSpecifier.h"

class Print;

namespace ace_time {

/**
 * Class that describes a time zone. There are 2 types:
 *
 *    * kTypeFixed: a time zone with a fixed UTC offset that cannot be changed.
 *      Very few actual time zones have a fixed UTC offset, but this type is
 *      useful for testing and parsing date/time strings with a fixed offset.
 *    * kTypeManual: a time zone using an underlyingManualZoneSpecifier
 *    * kTypeBasic: a time zone using an underlyingBasicZoneSpecifier
 *    * kTypeExtended: a time zone using an underlyingExtendedZoneSpecifier
 *
 * The TimeZone class really really wants to be a reference type. In other
 * words, it would be very convenient if the client code could create this
 * object on the heap, and pass it around using a pointer (or smart pointer) to
 * the ZonedDateTime class and shared among multiple ZonedDateTime objects.
 * This would also allow new TimeZones to be created, while allowing older
 * instances of ZonedDateTime to hold on to the previous versions of TimeZone.
 *
 * However, in a small memory embedded environment (like Arduino Nano or Micro
 * with only 2kB of RAM), I want to avoid any use of the heap (new operator or
 * malloc()) inside the AceTime library. I separated out the memory intensive
 * or mutable features of the TimeZone class into the separate ZoneSpecifier
 * class. The ZoneSpecifier object should be created once at initialization
 * time of the application (either statically allocated or potentially on the
 * heap early in the application start up).
 *
 * The TimeZone class becomes a thin wrapper around a ZoneSpecifier object
 * (essentially acting like a smart pointer in some sense). It should be
 * treated as a value type and passed around by value or by const reference.
 *
 * An alternative implementation would use an inheritance hierarchy for the
 * TimeZone, with 2 subclasses (ManualTimeZone and AutoTimeZone). However this
 * means that the TimeZone object can no longer be passed around by value, and
 * the ZonedDateTime is forced to hold on to the TimeZone object using a
 * pointer. It then becomes very difficult to change the offset and DST fields
 * of the ManualTimeZone. Using a single TimeZone class and implementing it as
 * a value type simplifies a lot of code.
 */
class TimeZone {
  public:
    static const uint8_t kTypeFixed = 0;
    static const uint8_t kTypeManual = ZoneSpecifier::kTypeManual;
    static const uint8_t kTypeBasic = ZoneSpecifier::kTypeBasic;
    static const uint8_t kTypeExtended = ZoneSpecifier::kTypeExtended;

    /**
     * Factory method to create from a fixed UTC offset.
     *
     * @param offset the fixed UTC offset, default 00:00 offset
     */
    static TimeZone forTimeOffset(TimeOffset offset = TimeOffset()) {
      return TimeZone(offset);
    }

    /**
     * Factory method to create from a ZoneSpecifier.
     *
     * @param zoneSpecifier an instance of ManualZoneSpecifier,
     * BasicZoneSpecifier, or ExtendedZoneSpecifier. Cannot be nullptr.
     */
    static TimeZone forZoneSpecifier(const ZoneSpecifier* zoneSpecifier) {
      return TimeZone(zoneSpecifier);
    }

    /** Default constructor. */
    TimeZone():
      mType(kTypeFixed),
      mOffset() {}

    /** Return the type of TimeZone. */
    uint8_t getType() const { return mType; }

    /** Return the total UTC offset at epochSeconds, including DST offset. */
    TimeOffset getUtcOffset(acetime_t epochSeconds) const {
      return (mType == kTypeFixed)
          ? mOffset
          : mZoneSpecifier->getUtcOffset(epochSeconds);
    }

    /**
     * Return the DST offset from standard UTC offset at epochSeconds. This is
     * an experimental method that has not been tested thoroughly. Use with
     * caution.
     */
    TimeOffset getDeltaOffset(acetime_t epochSeconds) const {
      return (mType == kTypeFixed)
          ? TimeOffset()
          : mZoneSpecifier->getDeltaOffset(epochSeconds);
    }

    /**
     * Return the best guess of the UTC offset at the given LocalDateTime for
     * the current TimeZone. Used by ZonedDateTime::forComponents(), so
     * intended to be used mostly for testing and debugging.
     */
    TimeOffset getUtcOffsetForDateTime(const LocalDateTime& ldt) const {
      return (mType == kTypeFixed)
          ? mOffset
          : mZoneSpecifier->getUtcOffsetForDateTime(ldt);
    }

    /**
     * Print the human readable representation of the time zone.
     *    * kTypeFixed at UTC is printed as "UTC"
     *    * kTypeFixed at another offset is printed as "+/-hh:mm"
     *    * kTypeManual is printed as "UTC+/-hh:mm (STD|DST)" (e.g. "-08:00
     *    (DST)")
     *    * kTypeBasic is printed as "{zonename}" (e.g. "America/Los_Angeles")
     *    * kTypeExtended is printed as "{zonename}" (e.g. "America/Los_Angeles")
     */
    void printTo(Print& printer) const;

    /**
     * Print the time zone abbreviation for the given epochSeconds.
     *    * kTypeFixed at UTC is printed as "UTC"
     *    * kTypeFixed at another offset is printed as "+/-hh:mm"
     *    * kTypeManual is printed as "{abbrev}" (e.g. "PDT")
     *    * kTypeBasic is printed as "{abbrev}" (e.g. "PDT")
     *    * kTypeExtended is printed as "{abbrev}" (e.g. "PDT")
     */
    void printAbbrevTo(Print& printer, acetime_t epochSeconds) const;

    /**
     * Return the isDst() value of the underlying ManualZoneSpecifier. This is
     * a convenience method that is valid only if the TimeZone was constructed
     * using a ManualZoneSpecifier. Returns false for all other type of
     * TimeZone. This is intended to be used by applications which allows the
     * user to set the UTC offset and DST flag manually (e.g.
     * examples/WorldClock.ino).
     */
    bool isDst() const {
      if (mType != kTypeManual) return false;
      return ((ManualZoneSpecifier*) mZoneSpecifier)->isDst();
    }

    /**
     * Sets the isDst() flag of the underlying ManualZoneSpecifier. Does
     * nothing for any other type of TimeZone. This is a convenience method for
     * applications that allow the user to set the DST flag manually (e.g.
     * examples/WorldClock).
     */
    void isDst(bool dst) {
      if (mType != kTypeManual) return;
      ((ManualZoneSpecifier*) mZoneSpecifier)->isDst(dst);
    }

    // Use default copy constructor and assignment operator.
    TimeZone(const TimeZone&) = default;
    TimeZone& operator=(const TimeZone&) = default;

  private:
    friend bool operator==(const TimeZone& a, const TimeZone& b);

    /**
     * Constructor for a fixed UTC offset.
     * @param offset the fix UTC offset
     */
    explicit TimeZone(TimeOffset offset):
      mType(kTypeFixed),
      mOffset(offset) {}

    /**
     * Constructor.
     *
     * It would be nice if zoneSpecifier could be a reference instead of a
     * pointer. But that makes TimeZone non-copyable which prevents it from
     * being used as a value-type. I don't want to use a wrapper like
     * std::reference_wrapper<T> because I want to avoid a dependency to the
     * C++ standard library for something targeted for the Arduino environment.
     *
     * @param zoneSpecifier an instance of ManualZoneSpecifier,
     * BasicZoneSpecifier, or ExtendedZoneSpecifier. Cannot be nullptr.
     */
    explicit TimeZone(const ZoneSpecifier* zoneSpecifier):
        mType(zoneSpecifier->getType()),
        mZoneSpecifier(zoneSpecifier) {}

    uint8_t mType;

    union {
      /** Used if mType == mTypeFixed. */
      TimeOffset mOffset;

      /** Used if mType == mTypeZoneSpecifier. */
      const ZoneSpecifier* mZoneSpecifier;
    };
};

inline bool operator==(const TimeZone& a, const TimeZone& b) {
  if (a.getType() != b.getType()) return false;
  if (a.getType() == TimeZone::kTypeFixed) {
    return a.mOffset == b.mOffset;
  } else {
    if (a.mZoneSpecifier == b.mZoneSpecifier) return true;
    return *a.mZoneSpecifier == *b.mZoneSpecifier;
  }
}

inline bool operator!=(const TimeZone& a, const TimeZone& b) {
  return ! (a == b);
}

}

#endif
