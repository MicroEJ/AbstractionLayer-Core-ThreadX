# Overview

Low Level MicroEJ Core Engine API implementation over ThreadX.

This component implements the `LLMJVM` Low Level API for MicroEJ Platforms connected to a Board Support Package based on [Azure RTOS ThreadX](https://azure.microsoft.com/en-us/services/rtos/).

See the MicroEJ documentation for a description of the `LLMJVM` functions:
- [LLMJVM: MicroEJ Core Engine](https://docs.microej.com/en/latest/PlatformDeveloperGuide/appendix/llapi.html#llmjvm-microej-core-engine)
- [MicroEJ Core Engine: Implementation](https://docs.microej.com/en/latest/PlatformDeveloperGuide/coreEngine.html#implementation)


# Usage

1. Install ``src`` and ``inc`` directories in your Board Support Package. They can be automatically downloaded using the following command lines:
   ```sh
    svn export --force https://github.com/MicroEJ/AbstractionLayer-Core-ThreadX/trunk/inc [path_to_bsp_directory]    
    svn export --force https://github.com/MicroEJ/AbstractionLayer-Core-ThreadX/trunk/src [path_to_bsp_directory]
   ```

2. Implement the MicroEJ time functions, as described in [microej_time.h](./inc/microej_time.h).

3. The `LLMJVM_IMPL_scheduleRequest` schedule request function in [LLMJVM_ThreadX.c](./src/LLMJVM_ThreadX.c) uses a software timer. In order to correctly schedule MicroEJ threads, check the following elements in the ThreadX configuration file:

   - `TX_TIMER_TICKS_PER_SECOND`: can depend on the application, if it needs a 1 ms precision then the tick rate would be 1000 Hz, the recommended value is between 100 Hz and 1000 Hz

# Requirements

None.

# Validation

This Abstraction Layer implementation can be validated in the target Board Support Package using the [MicroEJ Core Validation](https://github.com/MicroEJ/PlatformQualificationTools/tree/master/tests/core/java/microej-core-validation) Platform Qualification Tools project.

Here is a non exhaustive list of tested environments:
- Hardware
  - STMicroelectronics STM32F746-DISCOVERY
- Compilers / development environments:
  - IAR Embedded Workbench 8.50.6
- ThreadX / Azure RTOS versions:
  - 6.1

## MISRA Compliance

The implementation is MISRA-compliant (MISRA C 2004) with the following observed deviations:
| Deviation | Category |                                                 Justification                                                 |
|:---------:|:--------:|:-------------------------------------------------------------------------------------------------------------:|
| Rule 6.3  | Advisory |             A deviation from this rule is applied so as to use the bool type from `<stdbool.h>`.              |
| Rule 11.3 | Advisory | A deviation from this rule is unavoidable as ThreadX's handles (timers, queues, tasks) are pointers to void. | 
  

# Dependencies

- MicroEJ Architecture ``7.x`` or higher.
- Azure RTOS ThreadX ``6.1`` or higher.


# Source

N/A.

# Restrictions

None.

---

_Copyright 2021 MicroEJ Corp. All rights reserved._  
_This library is provided in source code for use, modification and test, subject to license terms._    
_Any modification of the source code will break MicroEJ Corp. warranties on the whole library._ 
