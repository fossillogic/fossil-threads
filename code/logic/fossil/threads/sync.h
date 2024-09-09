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
#ifndef FOSSIL_THREADS_SYNC_H
#define FOSSIL_THREADS_SYNC_H

#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
typedef HANDLE fossil_mutex_t;
typedef HANDLE fossil_cond_t;
typedef HANDLE fossil_semaphore_t;
#else
#include <pthread.h>
typedef pthread_mutex_t fossil_mutex_t;
typedef pthread_cond_t fossil_cond_t;
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    unsigned int value;
} fossil_semaphore_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize a mutex.
 *  @param mutex Pointer to the mutex object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_mutex_create(fossil_mutex_t *mutex);

/** Lock a mutex.
 *  @param mutex Pointer to the mutex object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_mutex_lock(fossil_mutex_t *mutex);

/** Unlock a mutex.
 *  @param mutex Pointer to the mutex object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_mutex_unlock(fossil_mutex_t *mutex);

/** Destroy a mutex.
 *  @param mutex Pointer to the mutex object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_mutex_destroy(fossil_mutex_t *mutex);

/** Initialize a condition variable.
 *  @param cond Pointer to the condition variable object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_cond_create(fossil_cond_t *cond);

/** Wait on a condition variable.
 *  @param cond Pointer to the condition variable object.
 *  @param mutex Pointer to the associated mutex object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_cond_wait(fossil_cond_t *cond, fossil_mutex_t *mutex);

/** Signal a condition variable.
 *  @param cond Pointer to the condition variable object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_cond_signal(fossil_cond_t *cond);

/** Broadcast a condition variable.
 *  @param cond Pointer to the condition variable object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_cond_broadcast(fossil_cond_t *cond);

/** Destroy a condition variable.
 *  @param cond Pointer to the condition variable object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_cond_destroy(fossil_cond_t *cond);

/** Initialize a semaphore.
 *  @param sem Pointer to the semaphore object.
 *  @param value Initial value of the semaphore.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_semaphore_create(fossil_semaphore_t *sem, unsigned int value);

/** Wait on a semaphore.
 *  @param sem Pointer to the semaphore object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_semaphore_wait(fossil_semaphore_t *sem);

/** Post (signal) a semaphore.
 *  @param sem Pointer to the semaphore object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_semaphore_post(fossil_semaphore_t *sem);

/** Destroy a semaphore.
 *  @param sem Pointer to the semaphore object.
 *  @return 0 on success, or an error code on failure.
 */
int32_t fossil_semaphore_destroy(fossil_semaphore_t *sem);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_THREADS_FRAMEWORK_H */
