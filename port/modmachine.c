// This file is never compiled standalone, it's included directly from
// extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE.

#include <neorv32.h>
#include "modmachine.h"
#include "mpconfigport.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#define MICROPY_PY_MACHINE_INFO_ENTRY \
  { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&machine_info_obj) },

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
  { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&machine_info_obj) },


/**********************************************************************//**
 * Print hardware information.
 **************************************************************************/
static mp_obj_t machine_info(uint n_args, const mp_obj_t *args) {
  neorv32_uart0_printf("NEORV32 version ");
  neorv32_aux_print_hw_version(neorv32_cpu_csr_read(CSR_MIMPID));
  neorv32_uart0_printf("\n");
  neorv32_uart0_printf("Clock: %u Hz\n", neorv32_sysinfo_get_clk());
  neorv32_uart0_printf("MISA:  0x%x\n", neorv32_cpu_csr_read(CSR_MISA));
  neorv32_uart0_printf("MXISA: 0x%x\n", neorv32_cpu_csr_read(CSR_MXISA));
  neorv32_uart0_printf("SoC:   0x%x\n", NEORV32_SYSINFO->SOC);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);


/**********************************************************************//**
 * Jump back to the bootloader (if available).
 **************************************************************************/
NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
  if (NEORV32_SYSINFO->SOC & (1 << SYSINFO_SOC_BOOTLOADER)) {
    // jump back to NEORV32 bootloader
    uint32_t bootloader_addr = NEORV32_BOOTROM_BASE;
    asm volatile ("jalr x0, 0(%[dst])" : : [dst] "r" (bootloader_addr));
    while(1);
  }
  else {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("NEORV32 Bootloader not synthesized"));
  }
}


/**********************************************************************//**
 * Get or set the MCU frequency.
 **************************************************************************/
static mp_obj_t mp_machine_get_freq(void) {
  mp_obj_t tuple[] = {
    mp_obj_new_int(neorv32_sysinfo_get_clk())
  };
  return mp_obj_new_tuple(MP_ARRAY_SIZE(tuple), tuple);
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_machine_get_freq_obj, mp_machine_get_freq);


/**********************************************************************//**
 * Idle state
 **************************************************************************/
static void mp_machine_idle(void) {
  MICROPY_EVENT_POLL_HOOK
}
