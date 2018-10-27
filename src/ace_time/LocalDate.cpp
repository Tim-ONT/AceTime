#include "common/Util.h"
#include "common/DateStrings.h"
#include "LocalDate.h"

namespace ace_time {

using common::printPad2;
using common::printPad3;
using common::DateStrings;

const uint8_t LocalDate::sDayOfWeek[12] = {
  5 /*1=Jan=31*/,
  1 /*2=Feb=28*/,
  0 /*3=Mar=31, start of "year"*/,
  3 /*4=Apr=30*/,
  5 /*5=May=31*/,
  1 /*6=Jun=30*/,
  3 /*7=Jul=31*/,
  6 /*8=Aug=31*/,
  2 /*9=Sep=30*/,
  4 /*10=Oct=31*/,
  0 /*11=Nov=30*/,
  2 /*12=Dec=31*/,
};

void LocalDate::printTo(Print& printer) const {
  if (isError()) {
    printer.print(F("<Invalid LocalDate>"));
    return;
  }

  // Date
  printer.print(F("2"));
  printPad3(printer, mYear);
  printer.print('-');
  printPad2(printer, mMonth);
  printer.print('-');
  printPad2(printer, mDay);
  printer.print(' ');

  // Week day
  DateStrings ds;
  printer.print(ds.weekDayLongString(dayOfWeek()));
}

LocalDate& LocalDate::initFromDateString(const char* ds) {
  if (strlen(ds) < kDateStringLength) {
    return setError();
  }

  // year
  uint8_t year = (*ds++ - '0') - 2; // subtract 2000
  year = 10 * year + (*ds++ - '0');
  year = 10 * year + (*ds++ - '0');
  year = 10 * year + (*ds++ - '0');
  mYear = year;

  // '-'
  ds++;

  // month
  uint8_t month = (*ds++ - '0');
  month = 10 * month + (*ds++ - '0');
  mMonth = month;

  // '-'
  ds++;

  // day
  uint8_t day = (*ds++ - '0');
  day = 10 * day + (*ds++ - '0');
  mDay = day;

  return *this;
}

}
