/*
 * C
 *
 * Copyright 2021-2026 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 *
 * Build: 7E4D1F7C
 */

/**
 * @file
 * @brief LLMJVM implementation over ThreadX.
 * @author MicroEJ Developer Team
 * @version 1.2.4
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <assert.h>

#include "LLMJVM_impl.h"
#include "microej_time.h"
#include "tx_api.h"
#include "tx_semaphore.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

#define TIMER_INITIAL_TICKS_U (0xFFFFFFFFU)

#define IDLE_SEMAPHORE_CEILING (1U)

// -----------------------------------------------------------------------------
// Private global variables
// -----------------------------------------------------------------------------

static TX_SEMAPHORE idle_semaphore;

static TX_TIMER schedule_timer;
static int64_t current_schedule_time = INT64_MAX;
static void schedule_request_callback(ULONG param);

// -----------------------------------------------------------------------------
// Private functions
// -----------------------------------------------------------------------------

/**
 * @brief The callback for the schedule request timer.
 *
 * @param param Parameter passed during timer creation.
 */
static void schedule_request_callback(ULONG param) {
	(void)param;
	int32_t ret = LLMJVM_schedule();
	assert(LLMJVM_OK == ret);
}

// -----------------------------------------------------------------------------
// Public function definition
// -----------------------------------------------------------------------------

// See the header file for the function documentation
int32_t LLMJVM_IMPL_initialize(void) {
	int32_t result = LLMJVM_OK;
	UINT tx_result = (UINT)0;
	/* ThreadX takes the names as non-const CHAR *. */
	static CHAR timer_name[] = "schedule timer";
	static CHAR semaphore_name[] = "idle semaphore";

	/* Create schedule timer */
	tx_result = tx_timer_create(&schedule_timer, timer_name,
	                            schedule_request_callback, (ULONG)0U, (ULONG)TIMER_INITIAL_TICKS_U, (ULONG)0U,
	                            TX_NO_ACTIVATE);
	if (TX_SUCCESS != tx_result) {
		result = LLMJVM_ERROR;
	}

	/* Create schedule semaphore */
	tx_result = tx_semaphore_create(&idle_semaphore, semaphore_name, (ULONG)0);
	if (TX_SUCCESS != tx_result) {
		result = LLMJVM_ERROR;
	}

	microej_time_init();

	return result;
}

// See the header file for the function documentation
int32_t LLMJVM_IMPL_vmTaskStarted(void) {
	return LLMJVM_OK;
}

// See the header file for the function documentation
int32_t LLMJVM_IMPL_scheduleRequest(int64_t absoluteTime) {
	int64_t relative_time_milliseconds = 0;
	int32_t result = LLMJVM_OK;
	UINT tx_result = (UINT)0;

	/* Check if the requested time is lower than the current schedule time */
	if (absoluteTime < current_schedule_time) {
		/* Save the new alarm schedule time */
		current_schedule_time = absoluteTime;

		/* Stop the schedule timer */
		tx_result = tx_timer_deactivate(&schedule_timer);
		if (TX_SUCCESS != tx_result) {
			result = LLMJVM_ERROR;
		}

		if (LLMJVM_ERROR != result) {
			relative_time_milliseconds = absoluteTime - microej_time_get_current_time((uint8_t)1);

			if (0 < relative_time_milliseconds) {
				/* Update the timer and launch it*/
				tx_result = tx_timer_change(&schedule_timer,
				                            (ULONG)microej_time_time_to_tick(relative_time_milliseconds),
				                            (ULONG)0);
				if (TX_SUCCESS == tx_result) {
					tx_result = tx_timer_activate(&schedule_timer);
				}
				if (TX_SUCCESS != tx_result) {
					result = LLMJVM_ERROR;
				}
			} else {
				/* Absolute time has already been reached */
				result = LLMJVM_schedule();
			}
		}
	}

	return result;
}

// See the header file for the function documentation
int32_t LLMJVM_IMPL_idleVM(void) {
	int32_t result = LLMJVM_OK;
	UINT tx_result = (UINT)0;

	/* Try to take the idle semaphore */
	tx_result = tx_semaphore_get(&idle_semaphore, TX_WAIT_FOREVER);
	if (TX_SUCCESS != tx_result) {
		result = LLMJVM_ERROR;
	}

	return result;
}

// See the header file for the function documentation
int32_t LLMJVM_IMPL_wakeupVM(void) {
	int32_t result = LLMJVM_OK;
	UINT tx_result = (UINT)0;

	/* Reset the schedule time */
	current_schedule_time = INT64_MAX;

	/* Release the semaphore */
	// Use the semaphore ceiling put API to limit the maximum value of the semaphore.
	// `LLMJVM_IMPL_ackWakeup` implementation assumes that the maximum semaphore count is 1.
	tx_result = tx_semaphore_ceiling_put(&idle_semaphore, IDLE_SEMAPHORE_CEILING);
	if ((TX_SUCCESS != tx_result) && (TX_CEILING_EXCEEDED != tx_result)) {
		result = LLMJVM_ERROR;
	}

	return result;
}

// See the header file for the function documentation
int32_t LLMJVM_IMPL_ackWakeup(void) {
	int32_t result = LLMJVM_OK;
	UINT tx_result = (UINT)0;

	// *** Resets the Idle semaphore to make the next call of LLMJVM_idleVM blocking.

	// ThreadX has no API to reset a semaphore, call tx_semaphore_get to reduce the
	// semaphore count to 0 without waiting time.
	//
	// No loop is needed here, because we assume that the maximum value is 1, thanks
	// to the use of the function tx_semaphore_ceiling_put in `LLMJVM_IMPL_wakeupVM`.
	tx_result = tx_semaphore_get(&idle_semaphore, TX_NO_WAIT);
	if ((TX_SUCCESS != tx_result) && (TX_NO_INSTANCE != tx_result)) {
		result = LLMJVM_ERROR;
	}

	// *** Cancels the schedule timer configured with LLMJVM_scheduleRequest.

	tx_result = tx_timer_deactivate(&schedule_timer);
	if (TX_SUCCESS != tx_result) {
		result = LLMJVM_ERROR;
	}

	// *** Resets the schedule time in order to allow a new timer configuration
	// in the next LLMJVM_scheduleRequest call.

	current_schedule_time = INT64_MAX;

	return result;
}

// See the header file for the function documentation
int32_t LLMJVM_IMPL_getCurrentTaskID(void) {
	return (int32_t)tx_thread_identify();
}

// See the header file for the function documentation
int32_t LLMJVM_IMPL_shutdown(void) {
	return LLMJVM_OK;
}

// See the header file for the function documentation
void LLMJVM_IMPL_setApplicationTime(int64_t t) {
	microej_time_set_application_time(t);
}

// See the header file for the function documentation
int64_t LLMJVM_IMPL_getCurrentTime(uint8_t sys) {
	return microej_time_get_current_time(sys);
}

// See the header file for the function documentation
int64_t LLMJVM_IMPL_getTimeNanos(void) {
	return microej_time_get_time_nanos();
}
