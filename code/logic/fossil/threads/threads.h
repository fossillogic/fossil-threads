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
#ifndef FOSSIL_THREADS_THREAD_H
#define FOSSIL_THREADS_THREAD_H

#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
typedef HANDLE fossil_thread_t;
typedef struct {
    DWORD stack_size;
    int32_t detach_state;
} fossil_thread_attr_t;
typedef LPVOID *(*fossil_task_t)(LPVOID *);
typedef LPVOID fossil_argumet_t;
#else
#include <pthread.h>
typedef pthread_t fossil_thread_t;
typedef pthread_attr_t fossil_thread_attr_t;
typedef void *(*fossil_task_t)(void *);
typedef void * fossil_argumet_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a new thread with the specified attributes and starts execution at the specified task function.
 *
 * @param thread Pointer to store the created thread ID.
 * @param attr Pointer to the thread attributes, or NULL for default attributes.
 * @param task Pointer to the task function to be executed by the thread.
 * @param arg Argument to pass to the task function.
 * @return int32_t 0 if the thread is successfully created, -1 otherwise.
 */
int32_t fossil_thread_create(fossil_thread_t *thread, fossil_thread_attr_t *attr, fossil_task_t task, fossil_argumet_t arg);

/**
 * @brief Waits for the specified thread to terminate and retrieves the thread's exit status.
 *
 * @param thread The thread ID to wait for.
 * @param retval Pointer to store the thread's exit status.
 * @return int32_t 0 if successful, -1 otherwise.
 */
int32_t fossil_thread_join(fossil_thread_t thread, void **retval);

/**
 * @brief Detaches the specified thread, allowing its resources to be automatically reclaimed when it exits.
 *
 * @param thread The thread ID to detach.
 * @return int32_t 0 if successful, -1 otherwise.
 */
int32_t fossil_thread_detach(fossil_thread_t thread);

/**
 * @brief Creates thread attributes with default values.
 *
 * @param attr Pointer to store the created thread attributes.
 * @return int32_t 0 if successful, -1 otherwise.
 */
int32_t fossil_thread_attr_create(fossil_thread_attr_t *attr);

/**
 * @brief Destroys the specified thread attributes and reclaims any resources associated with them.
 *
 * @param attr Pointer to the thread attributes to destroy.
 * @return int32_t 0 if successful, -1 otherwise.
 */
int32_t fossil_thread_attr_erase(fossil_thread_attr_t *attr);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_THREADS_FRAMEWORK_H */
