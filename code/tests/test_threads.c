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
fossil_thread_t test_thread;
fossil_thread_attr_t test_attr;

void *test_task(void *arg) {
    int *num = (int *)arg;
    *num += 1;
    return NULL;
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test
// * * * * * * * * * * * * * * * * * * * * * * * *

// Test Case 1: Create and join a thread successfully
FOSSIL_TEST(fossil_thread_create_and_join) {
    int value = 0;
    ASSUME_ITS_EQUAL_I32(0, fossil_thread_create(&test_thread, NULL, test_task, &value));
    ASSUME_ITS_EQUAL_I32(0, fossil_thread_join(test_thread, NULL));
    ASSUME_ITS_EQUAL_I32(1, value);
}

// Test Case 2: Attempt to create a thread with NULL task
FOSSIL_TEST(fossil_thread_create_null_task) {
    ASSUME_ITS_EQUAL_I32(-1, fossil_thread_create(&test_thread, NULL, NULL, NULL));
}

// Test Case 3: Create a thread with specific attributes
FOSSIL_TEST(fossil_thread_create_with_attr) {
    fossil_thread_attr_create(&test_attr);
    int value = 0;
    ASSUME_ITS_EQUAL_I32(0, fossil_thread_create(&test_thread, &test_attr, test_task, &value));
    ASSUME_ITS_EQUAL_I32(0, fossil_thread_join(test_thread, NULL));
    ASSUME_ITS_EQUAL_I32(1, value);
    fossil_thread_attr_erase(&test_attr);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_threaded_tests) {
    ADD_TEST(fossil_thread_create_and_join);
    ADD_TEST(fossil_thread_create_null_task);
    ADD_TEST(fossil_thread_create_with_attr);
}
