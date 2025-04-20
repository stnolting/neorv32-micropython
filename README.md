# NEORV32 MicroPython Port

A simple out-of-tree port of [MicroPython](https://github.com/micropython/micropython)
for the [NEORV32 RISC-V Processor](https://github.com/stnolting/neorv32) loosely based on
the upstream [stm32 port](https://github.com/micropython/micropython/tree/master/ports).


It is highly recommended to keep the current folder structure as an upstream "official" port
in the MicroPython repository simply doesn't make sense (yet?). This port is still under development.
Help and contributions are highly welcomed! :wink:

TODOs:

* add ports for further hardware modules (SPI, I²C, 1-Wire, NeoPixel, etc.)
* add arbitrary number of _software_ timers
* add support for custom hardware modules
* optimize the port (e.g. usage of `MICROPY_EVENT_POLL_HOOK`)
* ...


## Setup

> [!IMPORTANT]
> This MicroPython port requires UART0, the CLINT and at least 512kB of ROM (IMEM) and 256kB
> of RAM (IMEM). The memory layout (base addresses and sizes) can be customized in the Makefile.
> Make sure to adjust your NEORV32 processor configuration accordingly.

1. Clone this repository:

```bash
$ git clone https://github.com/stnolting/neorv32-micropython.git
```

2. Fetch the `neorv32` and `micropython` submodules:

```bash
$ cd neorv32-micropython
neorv32-micropython$ git submodule init
neorv32-micropython$ git submodule update
```

3. Get a RISC-V GCC toolchain. For example you can use the pre-built toolchain provided
by [xPack](https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack). Download and
install the toolchain and add it to your system's `PATH` environment variable. The default
GCC prefix is `riscv-none-elf-`.

4. Compile the NEORV32 MicroPython port. This will generate a single ELF file
(`build/firmware.elf`):

```bash
neorv32-micropython$ make all
...
LINK build/firmware.elf
   text    data     bss     dec     hex filename
 247600    2972   33804  284376   456d8 build/firmware.elf
```

5. The ELF can be loaded via the processor's on-chip debugger (using openOCD + DGB):

```
(gdb) monitor reset halt
JTAG tap: neorv32.cpu tap/device found: 0x00000001 (mfg: 0x000 (<invalid>), part: 0x0000, ver: 0x0)
(gdb) file build/firmware.elQuit
(gdb) monitor reset halt
JTAG tap: neorv32.cpu tap/device found: 0x00000001 (mfg: 0x000 (<invalid>), part: 0x0000, ver: 0x0)
(gdb) file build/firmware.elf
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "build/firmware.elf"? (y or n) y
Reading symbols from build/firmware.elf...
(gdb) load
Loading section .text, size 0x34008 lma 0x0
Loading section .rodata, size 0x8740 lma 0x34008
Loading section .data, size 0xb9c lma 0x3c748
Start address 0x00000000, load size 250596
Transfer rate: 199 KB/sec, 13922 bytes/write.
(gdb) c
Continuing.
```

6. The MicroPython REPL console is available via UART0 using a 19200-8-N-1 configuration:

```python
MicroPython v1.25.0 on 2025-04-20; neorv32-default with neorv32
Type "help()" for more information.
>>>
```


## Standard Modules

> [!TIP]
> The MicroPython shell supports auto-completion. Type a module name and press TAB to
> see all methods of that module.

Several standard MicroPathon modules are enabled by default. Additionally, the `machine`
and `time` have been ported. Run `modules("help")` to see all available modules.

```python
>>> help("modules")
__main__          collections       machine           sys
array             gc                micropython       time
builtins          io                struct
Plus any modules on the filesystem
```

To check the NEORV32 processor configuration you can use the `machine.info()` method:

```python
>>> import machine
>>> machine.info()
NEORV32 version 1.11.2.9
Clock: 150000000 Hz
MISA:  0x40901105
MXISA: 0x66006cd3
SoC:   0x480ba97b
```

The CLINT's machine timer is used for wall clock time-keeping. It is initialized with a pre-defined
start time in `ports/main.c`. To print the current time use `time.localtime()`. This will return
a tuple with 8 entires showing the current date and time.

```python
>>> import time
>>> time.localtime()
(2025, 4, 20, 19, 52, 46, 6, 110)
```

> [!TIP]
> Additional standard MicroPython modules can be enabled using `#define`s in `mpconfigport.h`.


## Port-Specific Modules

A port-specific module called `neorv32` is part of the standard `builtins` module.

```python
>>> import builtins
>>> builtins.
ArithmeticError                 AssertionError  AttributeError
BaseException   EOFError        Ellipsis        Exception
GeneratorExit   ImportError     IndentationError
IndexError      KeyError        KeyboardInterrupt
LookupError     MemoryError     NameError
NotImplementedError             OSError         OverflowError
RuntimeError    StopIteration   SyntaxError     SystemExit
TypeError       ValueError      ZeroDivisionError
abs             all             any             bool
bytearray       bytes           callable        chr
classmethod     dict            dir             divmod
eval            exec            getattr         globals
hasattr         hash            id              int
isinstance      issubclass      iter            len
list            locals          map             next
object          open            ord             pow
print           range           repr            round
set             setattr         sorted          staticmethod
str             sum             super           tuple
type            zip             StopAsyncIteration
__build_class__                 __dict__        __import__
__repl_print__  bin             delattr         enumerate
filter          help            hex             max
min             neorv32         oct             property
reversed        slice
```

The `neorv32` module provides some basic hardware access function. A help menu is available that
shows all currently implemented functions. For example, the `gpio_pin_*` functions provide access
to the processor's GPIO output port.

```python
>>> builtins.neorv32.help()
neorv32 - helper functions:
  gpio_pin_set(pin, level)       - Set GPIO.output [pin] to [level]
  gpio_pin_toggle(pin)           - Toggle GPIO.output [pin]
  systick_set_callback(callback) - Call [callback] from SysTICK IRQ
  help()                         - Show this text
```

The NEORV32 port implements a SYSTICK timer interrupt based on the processor's CLINT machine-timer
interrupt. This interrupt triggers every 10ms. A single callback function can be called from this
interrupt to execute a periodic Python method. The `neorv32` module provides a method `systick_set_callback()`
to install this handler method.

The following example defines a new method called `systick_handler`. It uses a global variable `ticks`
to "pre-scale" the actual execution time of the core functionality. Every 10ms x 100 = 1s it toggles
output pin 1of the processor's GPIO output to provide a simple heart-beat LED:

```python
>>> ticks = 0
>>> def systick_handler(_):
...     global ticks
...     if ticks > 100:
...         ticks = 0
...         builtins.neorv32.gpio_pin_toggle(1)
...     else:
...         ticks = ticks + 1
...
>>> builtins.neorv32.systick_set_callback(systick_handler)
```


## References

Resources that were used for creating this port:

* https://docs.micropython.org/en/latest/develop/porting.html
* https://firmware.card10.badge.events.ccc.de/pycardium-guide.html
* https://micropython-usermod.readthedocs.io/en/latest/usermods_05.html
* https://docs.micropython.org/en/latest/develop/library.html
* https://docs.micropython.org/en/latest/develop/extendingmicropython.html
* ChatGPT, GitHub Copilot and a lot of coffee
