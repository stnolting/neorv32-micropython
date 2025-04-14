# NEORV32 MicroPython Port

A very simple out-of-tree port of [MicroPython](https://github.com/micropython/micropython)
for the [NEORV32 RISC-V Processor](https://github.com/stnolting/neorv32) based on the
official [porting guide](https://docs.micropython.org/en/latest/develop/porting.html).

Supported features and TODOs:

* [x] REPL (Python prompt) using UART0
* [x] Builtin modules: gc, array, sys
* [ ] GPIO interface
* [ ] Peripherals (TWI, SPI, 1-Wire, etc.)
* [ ] Time / timers and interrupts
* [ ] Custom hardware modules

> [!NOTE]
> This MicroPython port requires UART0 and at least 512kB of ROM (IMEM) and 256kB of RAM
(IMEM). The memory layout (base addresses and sizes) can be customized in the Makefile.

### How-To

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

4. Compile the NEORV32 MicroPython port:

```bash
neorv32-micropython$ make all
```

5. This will generate a single ELF file (`build/firmware.elf`) that can be loaded
using the processor's on-chip debugger:

```
(gdb) file build/firmware.elf
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Reading symbols from build/firmware.elf...
(gdb) load
Loading section .text, size 0x36e98 lma 0x0
Loading section .rodata, size 0x8144 lma 0x36e98
Loading section .data, size 0xbe4 lma 0x3efdc
Start address 0x00000000, load size 261056
Transfer rate: 212 KB/sec, 13739 bytes/write.
(gdb) c
Continuing.
```

6. The MicroPython REPL console is available via UART0 using a 19200-8-N-1 configuration:

```python
MicroPython v1.25.0-preview.541.g9ee2ef510 on 2025-04-14; neorv32-default with neorv32
>>> print("hello world!")
hello world!
>>>
```
