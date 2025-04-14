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
#include "py/stackctrl.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"

#include "mphalport.h"

// Allocate memory for the MicroPython GC heap.
#if MICROPY_ENABLE_GC
static char heap[128*1024];
#endif

int main(int argc, char **argv) {
  // Setup the processor and UART0.
  neorv32_rte_setup();
  neorv32_uart_setup(NEORV32_UART0, 19200, 0);

  // Initialize the MicroPython runtime.
  mp_stack_ctrl_init();
  gc_init(heap, heap + sizeof(heap));
  mp_init();

  // Start a normal REPL; will exit when ctrl-D is entered on a blank line.
  pyexec_friendly_repl();

  // Deinitialise the runtime.
  gc_sweep_all();
  mp_deinit();
  return 0;
}

// Handle uncaught exceptions (should never be reached in a correct C implementation).
void nlr_jump_fail(void *val) {
  for (;;) {
  }
}

// Do a garbage collection cycle.
void gc_collect(void) {
  gc_collect_start();
  gc_helper_collect_regs_and_stack();
  gc_collect_end();
}

// There is no filesystem so stat'ing returns nothing.
mp_import_stat_t mp_import_stat(const char *path) {
  return MP_IMPORT_STAT_NO_EXIST;
}

// There is no filesystem so opening a file raises an exception.
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
  mp_raise_OSError(MP_ENOENT);
}

