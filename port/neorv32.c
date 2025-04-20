#include <neorv32.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mperrno.h"

/**********************************************************************//**
 * Set GPIO.output [pin] to [level]
 **************************************************************************/
static mp_obj_t mp_neorv32_gpio_pin_set(mp_obj_t pin, mp_obj_t level) {
  if (neorv32_gpio_available()) {
    int pin_tmp = mp_obj_get_int(pin);
    int level_tmp = mp_obj_get_int(level);
    neorv32_gpio_pin_set(pin_tmp, level_tmp);
  }
  else {
    neorv32_uart_puts(NEORV32_UART0, "[WARNING] NEORV32 GPIO module not available!\n");
  }
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(neorv32_gpio_pin_set_obj, mp_neorv32_gpio_pin_set);


/**********************************************************************//**
 * Toggle GPIO.output [pin]
 **************************************************************************/
static mp_obj_t mp_neorv32_gpio_pin_toggle(mp_obj_t pin) {
  if (neorv32_gpio_available()) {
    int pin_tmp = mp_obj_get_int(pin);
    neorv32_gpio_pin_toggle(pin_tmp);
  }
  else {
    neorv32_uart_puts(NEORV32_UART0, "[WARNING] NEORV32 GPIO module not available!\n");
  }
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(neorv32_gpio_pin_toggle_obj, mp_neorv32_gpio_pin_toggle);


/**********************************************************************//**
 * Program SYSTICK interrupt to call [callback]
 **************************************************************************/
extern mp_obj_t systick_callback_func;

static mp_obj_t mp_neorv32_systick_set_callback(mp_obj_t callback) {
  if (callback == mp_const_none) {
    systick_callback_func = mp_const_none;
  }
  else if (mp_obj_is_callable(callback)) {
    systick_callback_func = callback;
  }
  else {
    mp_raise_ValueError(MP_ERROR_TEXT("callback must be None or a callable object"));
  }
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(neorv32_systick_set_callback_obj, mp_neorv32_systick_set_callback);


/**********************************************************************//**
 * Help text.
 **************************************************************************/
static mp_obj_t mp_neorv32_help(void) {
  neorv32_uart_printf(NEORV32_UART0,
    "neorv32 - helper functions:\n"
    "  gpio_pin_set(pin, level)       - Set GPIO.output [pin] to [level]\n"
    "  gpio_pin_toggle(pin)           - Toggle GPIO.output [pin]\n"
    "  systick_set_callback(callback) - Call [callback] from SysTICK IRQ\n"
    "  help()                         - Show this text\n"
  );
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(neorv32_help_obj, mp_neorv32_help);


/**********************************************************************//**
 * The globals table for this module.
 **************************************************************************/
static const mp_rom_map_elem_t neorv32_module_globals_table[] = {
  {MP_ROM_QSTR(MP_QSTR___name__),             MP_ROM_QSTR(MP_QSTR_neorv32)},
  {MP_ROM_QSTR(MP_QSTR_gpio_pin_set),         MP_ROM_PTR(&neorv32_gpio_pin_set_obj)},
  {MP_ROM_QSTR(MP_QSTR_gpio_pin_toggle),      MP_ROM_PTR(&neorv32_gpio_pin_toggle_obj)},
  {MP_ROM_QSTR(MP_QSTR_systick_set_callback), MP_ROM_PTR(&neorv32_systick_set_callback_obj)},
  {MP_ROM_QSTR(MP_QSTR_help),                 MP_ROM_PTR(&neorv32_help_obj)},
};
static MP_DEFINE_CONST_DICT(neorv32_module_globals, neorv32_module_globals_table);

const mp_obj_module_t neorv32_module = {
  .base    = {&mp_type_module},
  .globals = (mp_obj_dict_t*)&neorv32_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_neorv32, neorv32_module);
