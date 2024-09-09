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
#include "fossil/threads/sync.h"

int32_t fossil_mutex_create(fossil_mutex_t *mutex) {
#ifdef _WIN32
    *mutex = CreateMutex(NULL, FALSE, NULL);
    return *mutex ? 0 : -1;
#else
    return pthread_mutex_init(mutex, NULL);
#endif
}

int32_t fossil_mutex_lock(fossil_mutex_t *mutex) {
#ifdef _WIN32
    return WaitForSingleObject(*mutex, INFINITE) == WAIT_OBJECT_0 ? 0 : -1;
#else
    return pthread_mutex_lock(mutex);
#endif
}

int32_t fossil_mutex_unlock(fossil_mutex_t *mutex) {
#ifdef _WIN32
    return ReleaseMutex(*mutex) ? 0 : -1;
#else
    return pthread_mutex_unlock(mutex);
#endif
}

int32_t fossil_mutex_destroy(fossil_mutex_t *mutex) {
#ifdef _WIN32
    return CloseHandle(*mutex) ? 0 : -1;
#else
    return pthread_mutex_destroy(mutex);
#endif
}

int32_t fossil_cond_create(fossil_cond_t *cond) {
#ifdef _WIN32
    *cond = CreateEvent(NULL, FALSE, FALSE, NULL);
    return *cond ? 0 : -1;
#else
    return pthread_cond_init(cond, NULL);
#endif
}

int32_t fossil_cond_wait(fossil_cond_t *cond, fossil_mutex_t *mutex) {
#ifdef _WIN32
    ReleaseMutex(*mutex);
    WaitForSingleObject(*cond, INFINITE);
    return WaitForSingleObject(*cond, INFINITE) == WAIT_OBJECT_0 ? 0 : -1;
#else
    return pthread_cond_wait(cond, mutex);
#endif
}

int32_t fossil_cond_signal(fossil_cond_t *cond) {
#ifdef _WIN32
    return SetEvent(*cond) ? 0 : -1;
#else
    return pthread_cond_signal(cond);
#endif
}

int32_t fossil_cond_broadcast(fossil_cond_t *cond) {
#ifdef _WIN32
    return SetEvent(*cond) ? 0 : -1;
#else
    return pthread_cond_broadcast(cond);
#endif
}

int32_t fossil_cond_destroy(fossil_cond_t *cond) {
#ifdef _WIN32
    return CloseHandle(*cond) ? 0 : -1;
#else
    return pthread_cond_destroy(cond);
#endif
}

#ifdef _WIN32
int32_t fossil_semaphore_create(fossil_semaphore_t *sem, unsigned int value) {
    *sem = CreateSemaphore(NULL, value, LONG_MAX, NULL);
    return *sem ? 0 : -1;
}

int32_t fossil_semaphore_wait(fossil_semaphore_t *sem) {
    return WaitForSingleObject(*sem, INFINITE) == WAIT_OBJECT_0 ? 0 : -1;
}

int32_t fossil_semaphore_post(fossil_semaphore_t *sem) {
    return ReleaseSemaphore(*sem, 1, NULL) ? 0 : -1;
}

int32_t fossil_semaphore_destroy(fossil_semaphore_t *sem) {
    return CloseHandle(*sem) ? 0 : -1;
}
#else
int32_t fossil_semaphore_create(fossil_semaphore_t *sem, unsigned int value) {
    int32_t status = pthread_mutex_init(&sem->mutex, NULL);
    if (status != 0) return -1;

    status = pthread_cond_init(&sem->cond, NULL);
    if (status != 0) {
        pthread_mutex_destroy(&sem->mutex);
        return -1;
    }

    sem->value = value;
    return 0;
}

int32_t fossil_semaphore_wait(fossil_semaphore_t *sem) {
    pthread_mutex_lock(&sem->mutex);
    while (sem->value == 0) {
        pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    sem->value--;
    pthread_mutex_unlock(&sem->mutex);
    return 0;
}

int32_t fossil_semaphore_post(fossil_semaphore_t *sem) {
    pthread_mutex_lock(&sem->mutex);
    sem->value++;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
    return 0;
}

int32_t fossil_semaphore_destroy(fossil_semaphore_t *sem) {
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->cond);
    return 0;
}
#endif
