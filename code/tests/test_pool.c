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

// Test variables
fossil_thread_pool_t test_pool;

void *simple_task(void *arg) {
    int *num = (int *)arg;
    *num += 1;
    return NULL;
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test
// * * * * * * * * * * * * * * * * * * * * * * * *

// Test Case 1: Create and destroy a thread pool
FOSSIL_TEST(fossil_thread_pool_create_and_destroy) {
    ASSUME_ITS_EQUAL_I32(0, fossil_thread_pool_create(&test_pool, 4));
    ASSUME_ITS_EQUAL_I32(0, fossil_thread_pool_destroy(&test_pool));
}

// Test Case 2: Submit task to thread pool and check result
FOSSIL_TEST(fossil_thread_pool_submit_task) {
    int value = 0;
    ASSUME_ITS_EQUAL_I32(0, fossil_thread_pool_create(&test_pool, 4));
    ASSUME_ITS_EQUAL_I32(0, fossil_thread_pool_submit(&test_pool, simple_task, &value));
    ASSUME_ITS_EQUAL_I32(0, fossil_thread_pool_destroy(&test_pool));
    ASSUME_ITS_EQUAL_I32(1, value);
}

// Test Case 3: Submit multiple tasks to the thread pool
FOSSIL_TEST(fossil_thread_pool_multiple_tasks) {
    int values[5] = {0, 0, 0, 0, 0};
    ASSUME_ITS_EQUAL_I32(0, fossil_thread_pool_create(&test_pool, 4));

    for (int i = 0; i < 5; i++) {
        ASSUME_ITS_EQUAL_I32(0, fossil_thread_pool_submit(&test_pool, simple_task, &values[i]));
    }

    ASSUME_ITS_EQUAL_I32(0, fossil_thread_pool_destroy(&test_pool));

    for (int i = 0; i < 5; i++) {
        ASSUME_ITS_EQUAL_I32(1, values[i]);
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_pool_tests) {
    ADD_TEST(fossil_thread_pool_create_and_destroy);
    ADD_TEST(fossil_thread_pool_submit_task);
    ADD_TEST(fossil_thread_pool_multiple_tasks);
}
