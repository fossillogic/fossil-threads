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
#include <fossil/unittest/framework.h>
#include <fossil/mockup/framework.h>
#include <fossil/xassume.h>

#include "fossil/threads/framework.h"

FOSSIL_FIXEXIT(fixture_sync);
// Test variables
fossil_semaphore_t test_semaphore;
fossil_mutex_t test_mutex;
fossil_cond_t test_cond;
int shared_counter = 0;

FOSSIL_SETUP(fixture_sync) {
    fossil_mutex_create(&test_mutex);
    fossil_cond_create(&test_cond);
    fossil_semaphore_create(&test_semaphore, 0);
}

FOSSIL_TEARDOWN(fixture_sync) {
    fossil_mutex_destroy(&test_mutex);
    fossil_cond_destroy(&test_cond);
    fossil_semaphore_destroy(&test_semaphore);
}

void *waiter(void *arg) {
    fossil_mutex_lock(&test_mutex);
    while (shared_counter == 0) {
        fossil_cond_wait(&test_cond, &test_mutex);
    }
    shared_counter++;
    fossil_mutex_unlock(&test_mutex);
    return NULL;
}

void *multi_waiter(void *arg) {
    fossil_mutex_lock(&test_mutex);
    while (shared_counter == 0) {
        fossil_cond_wait(&test_cond, &test_mutex);
    }
    shared_counter++;
    fossil_mutex_unlock(&test_mutex);
    return NULL;
}

void *increment_task(void *arg) {
    fossil_semaphore_wait(&test_semaphore);
    int *num = (int *)arg;
    *num += 1;
    return NULL;
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test
// * * * * * * * * * * * * * * * * * * * * * * * *

// Test Case 1: Lock and unlock mutex
FOSSIL_TEST(fossil_mutex_lock_unlock_case) {
    ASSUME_ITS_EQUAL_I32(0, fossil_mutex_lock(&test_mutex));
    shared_counter++;
    ASSUME_ITS_EQUAL_I32(0, fossil_mutex_unlock(&test_mutex));
    ASSUME_ITS_EQUAL_I32(1, shared_counter);
}

// Test Case 2: Condition wait and signal
FOSSIL_TEST(fossil_cond_wait_signal) {
    fossil_thread_t waiter_thread;
    fossil_thread_create(&waiter_thread, NULL, waiter, NULL);

    fossil_mutex_lock(&test_mutex);
    shared_counter = 1;
    fossil_cond_signal(&test_cond);
    fossil_mutex_unlock(&test_mutex);

    fossil_thread_join(waiter_thread, NULL);
    ASSUME_ITS_EQUAL_I32(2, shared_counter);
}

// Test Case 3: Broadcast signal to multiple waiting threads
FOSSIL_TEST(fossil_cond_broadcast_case) {
    fossil_thread_t threads[3];

    for (int i = 0; i < 3; i++) {
        fossil_thread_create(&threads[i], NULL, multi_waiter, NULL);
    }

    fossil_mutex_lock(&test_mutex);
    shared_counter = 1;
    fossil_cond_broadcast(&test_cond);
    fossil_mutex_unlock(&test_mutex);

    for (int i = 0; i < 3; i++) {
        fossil_thread_join(threads[i], NULL);
    }

    ASSUME_ITS_EQUAL_I32(4, shared_counter);
}

// Test Case 1: Initialize and post semaphore
FOSSIL_TEST(fossil_semaphore_init_post) {
    ASSUME_ITS_EQUAL_I32(0, fossil_semaphore_post(&test_semaphore));
}

// Test Case 2: Wait on semaphore
FOSSIL_TEST(fossil_semaphore_wait_case) {
    fossil_semaphore_post(&test_semaphore);
    ASSUME_ITS_EQUAL_I32(0, fossil_semaphore_wait(&test_semaphore));
}

// Test Case 3: Semaphore synchronization between threads
FOSSIL_TEST(fossil_semaphore_thread_sync) {
    fossil_thread_t test_thread;
    int value = 0;

    fossil_thread_create(&test_thread, NULL, increment_task, &value);
    fossil_semaphore_post(&test_semaphore);
    fossil_thread_join(test_thread, NULL);

    ASSUME_ITS_EQUAL_I32(1, value);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_sync_tests) {
    ADD_TESTF(fossil_mutex_lock_unlock_case, fixture_sync);
    ADD_TESTF(fossil_cond_wait_signal, fixture_sync);
    ADD_TESTF(fossil_cond_broadcast_case, fixture_sync);
    ADD_TESTF(fossil_semaphore_init_post, fixture_sync);
    ADD_TESTF(fossil_semaphore_wait_case, fixture_sync);
    ADD_TESTF(fossil_semaphore_thread_sync, fixture_sync);
}