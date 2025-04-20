#include <neorv32.h>
#include "py/mpconfig.h"
#include "py/runtime.h"

/**********************************************************************//**
 * Receive single character from NEORV32_UART0, blocking until one is available.
 **************************************************************************/
int mp_hal_stdin_rx_chr(void) {
  for (;;) {
    if (neorv32_uart_char_received(NEORV32_UART0)) {
      return (int)neorv32_uart_char_received_get(NEORV32_UART0);
    }
    MICROPY_EVENT_POLL_HOOK
  }
}


/**********************************************************************//**
 * Send the string [str] of given length [len] via NEORV32_UART0.
 **************************************************************************/
void mp_hal_stdout_tx_strn(const char *str, size_t len) {
  char c = 0;
  while (len--) {
    if ((c = *str++)) {
      while (!neorv32_uart_tx_free(NEORV32_UART0)) {
        MICROPY_EVENT_POLL_HOOK
      }
      neorv32_uart_tx_put(NEORV32_UART0, c);
    }
    else {
      return;
    }
  }
}


/**********************************************************************//**
 * Wait for [ms] milliseconds.
 **************************************************************************/
void mp_hal_delay_ms(mp_uint_t ms) {
  uint64_t stop = neorv32_clint_time_get() + (ms * (neorv32_sysinfo_get_clk() / 1000));
  while (1) {
    if (neorv32_clint_time_get() >= stop) {
      break;
    }
    MICROPY_EVENT_POLL_HOOK
  }
}


/**********************************************************************//**
 * Wait for [us] microseconds.
 **************************************************************************/
void mp_hal_delay_us(mp_uint_t us) {
  uint64_t stop = neorv32_clint_time_get() + (us * (neorv32_sysinfo_get_clk() / 10000));
  while (1) {
    if (neorv32_clint_time_get() >= stop) {
      break;
    }
    MICROPY_EVENT_POLL_HOOK
  }
}


/**********************************************************************//**
 * Returns an increasing millisecond counter with an arbitrary reference point.
 **************************************************************************/
mp_uint_t mp_hal_ticks_ms(void) {
  return (mp_uint_t)(neorv32_clint_time_get() / (neorv32_sysinfo_get_clk() / 1000));
}


/**********************************************************************//**
 * Returns an increasing microseconds counter with an arbitrary reference point.
 **************************************************************************/
mp_uint_t mp_hal_ticks_us(void) {
  return (mp_uint_t)(neorv32_clint_time_get() / (neorv32_sysinfo_get_clk() / 1000000));
}


/**********************************************************************//**
 * Returns an increasing cycles counter with an arbitrary reference point.
 **************************************************************************/
mp_uint_t mp_hal_ticks_cpu(void) {
  return (mp_uint_t)neorv32_clint_time_get();
}
