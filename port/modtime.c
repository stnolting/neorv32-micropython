// This file is never compiled standalone, it's included directly from
// extmod/modtime.c via MICROPY_PY_TIME_INCLUDEFILE.

#include "py/obj.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include <neorv32.h>

/**********************************************************************//**
 * Return the local time as an 8-tuple.
 **************************************************************************/
void mp_time_localtime_get(timeutils_struct_time_t *tm) {

  date_t date;
  neorv32_aux_unixtime2date(neorv32_clint_unixtime_get(), &date);

  tm->tm_year = date.year;
  tm->tm_mon  = date.month;
  tm->tm_mday = date.day;
  tm->tm_hour = date.hours;
  tm->tm_min  = date.minutes;
  tm->tm_sec  = date.seconds;
  tm->tm_wday = date.weekday - 1;
  tm->tm_yday = timeutils_year_day(date.year, date.month, date.day);
}


/**********************************************************************//**
 * Returns the number of seconds since Epoch.
 **************************************************************************/
mp_obj_t mp_time_time_get(void) {

  date_t date;
  neorv32_aux_unixtime2date(neorv32_clint_unixtime_get(), &date);

  return mp_obj_new_int(timeutils_seconds_since_epoch(date.year, date.month, date.day, date.hours, date.minutes, date.seconds));
}
