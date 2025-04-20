// This file is never compiled standalone, it's included directly from
// extmod/modtime.c via MICROPY_PY_TIME_INCLUDEFILE.

#include "py/obj.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include <neorv32.h>

/**********************************************************************//**
 * Return the localtime as an 8-tuple.
 **************************************************************************/
mp_obj_t mp_time_localtime_get(void) {
  date_t date;
  neorv32_aux_unixtime2date(neorv32_clint_unixtime_get(), &date);
  mp_obj_t tuple[8] = {
    mp_obj_new_int(date.year),
    mp_obj_new_int(date.month),
    mp_obj_new_int(date.day),
    mp_obj_new_int(date.hours),
    mp_obj_new_int(date.minutes),
    mp_obj_new_int(date.seconds),
    mp_obj_new_int(date.weekday - 1),
    mp_obj_new_int(timeutils_year_day(date.year, date.month, date.day)),
  };
  return mp_obj_new_tuple(8, tuple);
}


/**********************************************************************//**
 * Returns the number of seconds since Epoch.
 **************************************************************************/
mp_obj_t mp_time_time_get(void) {
  date_t date;
  neorv32_aux_unixtime2date(neorv32_clint_unixtime_get(), &date);
  return mp_obj_new_int(timeutils_seconds_since_epoch(date.year, date.month, date.day, date.hours, date.minutes, date.seconds));
}
