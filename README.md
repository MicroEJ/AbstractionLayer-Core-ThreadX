![ARCH](https://shields.microej.com/endpoint?url=https://repository.microej.com/packages/badges/arch_8.3.json)

# Abstraction Layer for the MicroEJ Core Engine over ThreadX

# Overview

Low Level MicroEJ Core Engine API implementation over ThreadX.

This component implements the `LLMJVM` Low Level API for MicroEJ Platforms connected to a Board Support Package based on [Eclipse ThreadX RTOS](https://github.com/eclipse-threadx/threadx).

See the MicroEJ documentation for a description of the `LLMJVM` functions:
- [LLMJVM: MicroEJ Core Engine](https://docs.microej.com/en/latest/PlatformDeveloperGuide/appendix/llapi.html#llmjvm-microej-core-engine)
- [MicroEJ Core Engine: Implementation](https://docs.microej.com/en/latest/PlatformDeveloperGuide/coreEngine.html#implementation)


# Usage

1. These sources can be included in the VEE Port with the method you prefer, by using this repository as a submodule or by doing a copy of the sources in the VEE Port repository.

2. Implement the MicroEJ time functions, as described in [microej_time.h](./inc/microej_time.h).

3. The `LLMJVM_IMPL_scheduleRequest` schedule request function in [LLMJVM_ThreadX.c](./src/LLMJVM_ThreadX.c) uses a software timer. In order to correctly schedule MicroEJ threads, check the following elements in the ThreadX configuration file:

   - `TX_TIMER_TICKS_PER_SECOND`: can depend on the application, if it needs a 1 ms precision then the tick rate would be 1000 Hz, the recommended value is between 100 Hz and 1000 Hz.

# Requirements

None.

# Validation

This Abstraction Layer implementation can be validated in the target Board Support Package using the [MicroEJ Core Validation](https://github.com/MicroEJ/PlatformQualificationTools/tree/master/tests/core/java/microej-core-validation) Platform Qualification Tools project.

Here is a non exhaustive list of tested environments:

- Hardware
  - STMicroelectronics STM32F746-DISCOVERY
  - STMicroelectronics STM32U5G9J-DK2
- Compilers / development environments:
  - IAR Embedded Workbench 8.50.6
  - IAR Embedded Workbench 9.30.1
  - IAR Embedded Workbench 9.60.4
- ThreadX RTOS versions:
  - 6.1
  - 6.2
  - 6.4.1

## MISRA Compliance

This Abstraction Layer implementation is MISRA-compliant (MISRA C:2012) with some noted exception.
It has been verified with Cppcheck v2.19. Here is the list of deviations from MISRA standard:

| Deviation | Category |                                                 Justification                                                 |
|:---------:|:--------:|:-------------------------------------------------------------------------------------------------------------:|
|  Rule 5.5 | Required | Intentional usage of macros to define native functions.                                                       |
|  Rule 7.4 | Required | ThreadX takes the object names as non-const `CHAR *`: the names are `CHAR` arrays initialized from a string literal, so no pointer to the literal exists. |
|  Rule 8.4 | Required | A compatible declaration is defined in headers provided by the VEE Port.                                      |
|  Rule 8.7 | Advisory | The `LLMJVM_IMPL_*` functions are the LLAPI of the Core Engine. Only the Core Engine calls them, and the analysis does not see it. |
| Rule 11.4 | Advisory | `LLMJVM_IMPL_getCurrentTaskID()` returns the address of the ThreadX thread control block as the `int32_t` task identifier of the LLAPI. |


# Dependencies

- MicroEJ Architecture ``7.x`` or higher.
- ThreadX RTOS ``6.1`` or higher.


# Source

N/A.

# Restrictions

None.

---

_Copyright 2021-2026 MicroEJ Corp. All rights reserved._
_Use of this source code is governed by a BSD-style license that can be found with this software._
_Build: 7E4D1F7C_
