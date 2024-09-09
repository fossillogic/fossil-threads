/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/threads/threads.h"
#include <stdlib.h>

#ifdef _WIN32
// Windows-specific thread start function wrapper
static DWORD WINAPI fossil_thread_wrapper(LPVOID arg) {
    void *(*task)(void *) = ((void ***)arg)[0];  // Extract the task function
    void *task_arg = ((void ***)arg)[1];         // Extract the task argument
    free(arg);  // Free the allocated memory
    task(task_arg);  // Call the original task function
    return 0;
}
#endif

/* -------- Kernel Threads Implementation -------- */

int32_t fossil_thread_create(fossil_thread_t *thread, fossil_thread_attr_t *attr, void *(*task)(void *), void *arg) {
#ifdef _WIN32
    void **thread_data = malloc(2 * sizeof(void *));
    thread_data[0] = task;
    thread_data[1] = arg;

    DWORD thread_id;
    *thread = CreateThread(
        attr ? (LPSECURITY_ATTRIBUTES) attr : NULL,
        attr ? attr->stack_size : 0,
        fossil_thread_wrapper,  // Call the wrapper instead of the original task
        thread_data,            // Pass both task and argument to the wrapper
        attr && attr->detach_state ? CREATE_SUSPENDED : 0,
        &thread_id
    );
    return *thread ? 0 : -1;
#else
    return pthread_create(thread, attr, task, arg);
#endif
}

int32_t fossil_thread_join(fossil_thread_t thread, void **retval) {
#ifdef _WIN32
    DWORD res = WaitForSingleObject(thread, INFINITE);
    if (res == WAIT_OBJECT_0 && retval) {
        GetExitCodeThread(thread, (LPDWORD)retval);
    }
    CloseHandle(thread);
    return res == WAIT_OBJECT_0 ? 0 : -1;
#else
    return pthread_join(thread, retval);
#endif
}

int32_t fossil_thread_detach(fossil_thread_t thread) {
#ifdef _WIN32
    return CloseHandle(thread) ? 0 : -1;
#else
    return pthread_detach(thread);
#endif
}

int32_t fossil_thread_attr_create(fossil_thread_attr_t *attr) {
#ifdef _WIN32
    attr->stack_size = 0; // Default stack size
    attr->detach_state = 0; // Default joinable
    return 0;
#else
    return pthread_attr_init(attr);
#endif
}

int32_t fossil_thread_attr_erase(fossil_thread_attr_t *attr) {
#ifdef _WIN32
    (void)attr; // No need for explicit cleanup on Windows
    return 0;
#else
    return pthread_attr_destroy(attr);
#endif
}
