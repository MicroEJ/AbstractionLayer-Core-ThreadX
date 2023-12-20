# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.1] - 2023-12-20

### Fixed

- Fix typo in README.

## [1.2.0] - 2023-12-19

### Changed

- Update license terms to MicroEJ Corp. BSD-style license 1.2.
- Update MISRA compliance to MISRA-2012 with latest static analysis process

## [1.1.0] - 2022-06-30

### Changed

- Change license terms to MicroEJ Corp. BSD-style license.

### Fixed

- Fix application time that was not set properly in `LLMJVM_IMPL_setApplicationTime()`.
- Fix invalid creation of ThreadX timer and semaphore in `LLMJVM_IMPL_initialize()`.
- Fix ThreadX timer that was configured as a periodic timer instead of one-shot timer in `LLMJVM_IMPL_scheduleRequest()`.
- Fix MicroEJ Time component that was not initialized.
- Fix semaphore's initialization that caused the first call to `LLMJVM_IMPL_idleVM()` to be non-blocking.

## [1.0.0] - 2021-06-03

### Added

  - Initial publication version.

---

_Copyright 2021-2023 MicroEJ Corp. All rights reserved._
_Use of this source code is governed by a BSD-style license that can be found with this software._

