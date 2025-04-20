#include <stdint.h>

// Python internal features
#define MICROPY_ENABLE_GC                       (1)
#define MICROPY_HELPER_REPL                     (1)
#define MICROPY_ERROR_REPORTING                 (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_ENABLE_SCHEDULER                (1)
#define MICROPY_SCHEDULER_STATIC_NODES          (1)

// Fine control over Python builtins, classes, modules, etc
#define MICROPY_PY_MACHINE_BOOTLOADER           (1)
#define MICROPY_PY_TIME                         (1)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_INCLUDEFILE             "../port/modtime.c"
#define MICROPY_PY_MACHINE                      (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE          "../port/modmachine.c"
#define MICROPY_PY_MACHINE_FREQ_NUM_ARGS_MAX    (4)
#define MICROPY_PY_MACHINE_BOOTLOADER           (1)
#define MICROPY_PY_SYS                          (1)
#define MICROPY_PY_BUILTINS_HELP                (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES        (1)

// Externals
extern const struct _mp_obj_module_t mp_module_time;

// Type definitions for the specific machine
typedef intptr_t  mp_int_t;
typedef uintptr_t mp_uint_t;
typedef long      mp_off_t;

// Execute pending (interrupt) callbacks
#define MICROPY_EVENT_POLL_HOOK \
  do { \
    extern void mp_handle_pending(bool); \
    mp_handle_pending(true); \
    asm volatile ("wfi"); \
  } while (0);

// Port-specific builtins to add to the global namespace
extern const struct _mp_obj_module_t neorv32_module;
#define MICROPY_PORT_BUILTINS \
  { MP_ROM_QSTR(MP_QSTR_neorv32), MP_ROM_PTR(&neorv32_module) }

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

// Define the port's name and hardware
#define MICROPY_HW_BOARD_NAME "neorv32-default"
#define MICROPY_HW_MCU_NAME   "neorv32"

#define MP_STATE_PORT MP_STATE_VM
