
# Ultrawide band based localization system

First off, a comprehensive wiki on this sytem can be found at: [RSE-WIKI - 3D localization](http://rsewiki.elektro.dtu.dk/index.php/3D_localization)

The entire system is build to be easily ported to new hardware platforms, with minimum configuration.

The general folder structure is describe below, with the important hardware specific files highlighted and described.

| Folder              | Contains                                                               |
| ------------------- | ---------------------------------------------------------------------- |
| Drivers/            | Peripheral libraries, IMU drivers, DWM drivers, etc.                   |
| Inc/                | Include files (.h)                                                     |
| Inc/$(CPU)/         | Processor/device specific include files                                |
| Inc/hal.h           | Hardware abstraction configuration file (Add new devices here first)   |
| Linkers/            | Linker files for GCC (Processor specific)                              |
| MATLAB/             | MATLAB simulation scripts and design files                             |
| Middlewares/        | Middleware libraries (eg. USB middlewares)                             |
| PCB/                | DipTrace files for the PCB design                                      |
| Related papers/     | Papers I have found relevant for the project                           |
| Scripts/            | Various helper scripts (eg. Python scripts for DFU management)         |
| Src/                | All of the source files (.c/.cpp)                                      |
| Src/$(CPU)/         | Processor/device specific source files                                 |
| Src/periph/         | Peripheral setup source files (i2c, spi, tim, dma, etc.)               |
