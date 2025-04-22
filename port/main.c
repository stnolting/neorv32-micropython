/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2022-2023 Damien P. George
 *
 * Ported to the NEORV32 RISC-V Processor.
 * Copyright (c) 2025 Stephan Nolting
 */

#include <neorv32.h>
#include <stdio.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stackctrl.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"

#include "mphalport.h"

// Allocate memory for the MicroPython GC heap.
#if MICROPY_ENABLE_GC
static char heap[32*1024];
#endif


/**********************************************************************//**
 * Systick interrupt (CLINT MTIMER)
 **************************************************************************/
static uint64_t __systick_inc;
mp_obj_t systick_callback_func = mp_const_none;

void systick_isr(void) {
  neorv32_clint_mtimecmp_set(neorv32_clint_mtimecmp_get() + __systick_inc);
  if (systick_callback_func != mp_const_none) {
    mp_sched_schedule(systick_callback_func, MP_OBJ_NULL);
  }
}


/**********************************************************************//**
 * Main function
 **************************************************************************/
int main(int argc, char **argv) {

  // setup NEORV32 runtime environment
  neorv32_rte_setup();

  // check if core peripherals are available
  if (!neorv32_uart0_available()) {
    return -1;
  }
  if (!neorv32_clint_available()) {
    neorv32_uart0_printf("[ERROR] CLINT not synthesized\n");
    return -1;
  }

  // setup peripherals
  neorv32_uart_setup(NEORV32_UART0, 19200, 0); // no interrupts
  if (neorv32_gpio_available()) {
    neorv32_gpio_port_set(0); // clear GPIO.output port if available
  }

  // setup time
  date_t date;
  date.year    = 2025; // absolute
  date.month   = 04;   // 1..12
  date.day     = 20;   // 1..31
  date.hours   = 19;   // 0..23
  date.minutes = 51;   // 0..59
  date.seconds = 00;   // 0..59
  neorv32_clint_unixtime_set(neorv32_aux_date2unixtime(&date));

  // setup system tick interrupt
  neorv32_rte_handler_install(RTE_TRAP_MTI, systick_isr);
  __systick_inc = neorv32_sysinfo_get_clk() / 100;
  neorv32_clint_mtimecmp_set(neorv32_clint_time_get() + __systick_inc);
  neorv32_cpu_csr_set(CSR_MIE, 1 << CSR_MIE_MTIE);

  // enable interrupts globally
  neorv32_cpu_csr_set(CSR_MSTATUS, 1 << CSR_MSTATUS_MIE);

  // initialize the MicroPython runtime
  mp_stack_ctrl_init();
  gc_init(heap, heap + sizeof(heap));
  mp_init();

  // start a normal REPL; will exit when ctrl-D is entered on a blank line
  pyexec_friendly_repl();
  neorv32_uart0_printf("Shutdown...\n");

  // de-initialize the runtime
  gc_sweep_all();
  mp_deinit();

  return 0;
}


/**********************************************************************//**
 * Handle uncaught exceptions (should never be reached in a correct C implementation)
 **************************************************************************/
void nlr_jump_fail(void *val) {
  neorv32_uart0_puts("\nERR nlr_jump_fail\n");
  while(1);
}


/**********************************************************************//**
 * Do a garbage collection cycle
 **************************************************************************/
void gc_collect(void) {
  gc_collect_start();
  gc_helper_collect_regs_and_stack();
  gc_collect_end();
}


/**********************************************************************//**
 * There is no filesystem so stat'ing returns nothing
 **************************************************************************/
mp_import_stat_t mp_import_stat(const char *path) {
  return MP_IMPORT_STAT_NO_EXIST;
}


/**********************************************************************//**
 * There is no filesystem so opening a file raises an exception
 **************************************************************************/
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
  mp_raise_OSError(MP_ENOENT);
}


/**********************************************************************//**
 * There is no filesystem so opening a file does nothing
 **************************************************************************/
mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
