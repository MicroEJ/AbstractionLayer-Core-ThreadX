/*
 * C
 *
 * Copyright 2021-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLMJVM implementation over ThreadX.
 * @author MicroEJ Developer Team
 * @version 1.2.1
 */
#include <assert.h>
#include <LLMJVM_impl.h>

#include "microej_time.h"
#include "tx_api.h"
#include "tx_semaphore.h"

#define TIMER_INITIAL_TICKS_U (0xFFFFFFFFU)

static int64_t current_schedule_time = INT64_MAX;
static TX_SEMAPHORE idle_semaphore;
static TX_TIMER schedule_timer;
static void schedule_request_callback(ULONG param);

/**
 * @brief The callback for the schedule request timer.
 *
 * @param param Parameter passed during timer creation.
 */
static void schedule_request_callback(ULONG param) {
	(void) param;
	int32_t ret = LLMJVM_schedule();
	assert(LLMJVM_OK == ret);
}

/**
 * This function is called once during MicroJvm virtual machine Startup. It may be used to initialize specific data.<br>
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_initialize(void){
	int32_t result = LLMJVM_OK;
	UINT tx_result = (UINT) 0;
	static CHAR* timer_name = "schedule timer";
	static CHAR* semaphore_name = "idle semaphore";

	/* Create schedule timer */
	tx_result = tx_timer_create(&schedule_timer, timer_name,
			schedule_request_callback, (ULONG) 0U, (ULONG) TIMER_INITIAL_TICKS_U, (ULONG) 0U,
			TX_NO_ACTIVATE);
	if (TX_SUCCESS != tx_result) {
		result = LLMJVM_ERROR;
	}

	/* Create schedule semaphore */
	tx_result = tx_semaphore_create(&idle_semaphore, semaphore_name, (ULONG) 0);
	if (TX_SUCCESS != tx_result) {
		result = LLMJVM_ERROR;
	}

	microej_time_init();

	return result;
}

/**
 * This function is called once during the MicroJvm virtual machine initialization by the MicroJvm virtual machine task.
 * It can be useful if the MicroJvm virtual machine support needs to know the MicroJvm virtual machine task.
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_vmTaskStarted(void){
	return LLMJVM_OK;
}

/**
 * Schedule an alarm (or timer) that will be triggered at the given absolute (system) time.
 * If an alarm is already scheduled for an earlier time this function must do nothing,
 * otherwise it must configure the alarm. If the given absolute time has already been reached, this
 * function must call {@link LLMJVM_schedule}.
 * The scheduled alarm must call the function {@link LLMJVM_schedule} when it is triggered.
 * The specified time is in milliseconds.
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_scheduleRequest(int64_t absoluteTime){
	int64_t relative_time_milliseconds = 0;
	int32_t result = LLMJVM_OK;
	UINT tx_result = (UINT) 0;

	/* Check if the requested time is lower than the current schedule time */
	if (absoluteTime < current_schedule_time) {
		/* Save the new alarm schedule time */
		current_schedule_time = absoluteTime;

		/* Stop the schedule timer */
		tx_timer_deactivate(&schedule_timer);

		relative_time_milliseconds = absoluteTime - microej_time_get_current_time((uint8_t) 1);

		if (0 < relative_time_milliseconds) {
			/* Update the timer and launch it*/
			tx_result = tx_timer_change(&schedule_timer, (ULONG) microej_time_time_to_tick(relative_time_milliseconds), (ULONG) 0);
			tx_timer_activate(&schedule_timer);
			if (TX_SUCCESS != tx_result) {
				result = LLMJVM_ERROR;
			}
		} else {
			/* Absolute time has already been reached */
			result = LLMJVM_schedule();
		}
	}

	return result;
}

/**
 * Causes the MicroJvm virtual machine RTOS task to sleep until it is woken up by the {@link LLMJVM_wakeupVM} function.
 * This function is called by the MicroJvm virtual machine task.
 * @return {@link LLMJVM_OK} if wakeup occurred, {@link LLMJVM_INTERRUPTED} if the MicroJvm virtual machine task
 * has been interrupted, or {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_idleVM(void){
	int32_t result = LLMJVM_OK;
	UINT tx_result = (UINT) 0;

	/* Try to take the idle semaphore */
	tx_result = tx_semaphore_get(&idle_semaphore, TX_WAIT_FOREVER);
	if (TX_SUCCESS != tx_result) {
		result = LLMJVM_ERROR;
	}

	return result;
}

/**
 * Wake up the MicroJvm virtual machine RTOS task. If the MicroJvm virtual machine task is not sleeping,
 * the wakeup stays pending and the MicroJvm virtual machine will not sleep on the next {@link LLMJVM_idleVM} call unless
 * there is a call to {@link LLMJVM_ackWakeup} between this call and the next {@link LLMJVM_idleVM} call.
 * This function must be called only by the MicroJvm virtual machine code. If a task wants to wake up the MicroJvm
 * virtual machine, it must use the {@link LLMJVM_schedule} function (which may in turn call
 * this function).
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_wakeupVM(void) {
	int32_t result = LLMJVM_OK;
	UINT tx_result = (UINT) 0;

	/* Reset the schedule time */
	current_schedule_time = INT64_MAX;

	/* Release the semaphore */
	tx_result = tx_semaphore_put(&idle_semaphore);
	if (TX_SUCCESS != tx_result) {
		result = LLMJVM_ERROR;
	}

	return result;
}

/**
 * Clears any outstanding {@link LLMJVM_wakeupVM} request. After calling this function, a call to {@link LLMJVM_idleVM} will
 * result in a wait even if {@link LLMJVM_wakeupVM} has been called previously (provided no other {@link LLMJVM_wakeupVM} call
 * has occurred since the call to this function.
 * This function is called by the MicroJvm virtual machine task.
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_ackWakeup(void){
	return LLMJVM_OK;
}

/**
 * Returns the ID of the current task.
 */
int32_t LLMJVM_IMPL_getCurrentTaskID(void){
	return (int32_t) tx_thread_identify();
}

/**
 * This function is called during MicroJvm virtual machine end. It may be used to freed specific data.<br>
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_shutdown(void){
	return LLMJVM_OK;
}

/**
 * Sets the application time. The application time is the difference, measured in milliseconds,
 * between the current time and midnight, January 1, 1970 UTC.
 * This time does not change the system time.
 * @param t the application time to set in milliseconds
 */
void LLMJVM_IMPL_setApplicationTime(int64_t t){
	microej_time_set_application_time(t);
}

/**
 * Gets the system or the application time in milliseconds.
 * The system time is the running time since the start of the device. This time is independent from any user considerations
 * and cannot be changed.
 * The application time is the difference, measured in milliseconds, between the current time and midnight, January 1, 1970 UTC.
 * @param system if 1, get the system time, otherwise get the application time.
 */
int64_t LLMJVM_IMPL_getCurrentTime(uint8_t sys){
	return microej_time_get_current_time(sys);
}

/**
 * Gets the current timestamp in nanoseconds.
 * Only elapsed time between two calls is meaningful.
 * @return the current timestamp in nanoseconds
 */
int64_t LLMJVM_IMPL_getTimeNanos(void){
	return microej_time_get_time_nanos();
}
