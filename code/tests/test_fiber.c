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

FOSSIL_FIXEXIT(fixture_fiber);
// Test variables
fossil_fiber_t test_fiber;
fossil_fiber_t main_fiber;

void *fiber_task(void *arg) {
    int *num = (int *)arg;
    *num += 1;
    return NULL;
}

FOSSIL_SETUP(fixture_fiber) {
    fossil_fiber_create(&test_fiber, fiber_task, NULL);
}

FOSSIL_TEARDOWN(fixture_fiber) {
    fossil_fiber_destroy(&test_fiber);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test
// * * * * * * * * * * * * * * * * * * * * * * * *

// Test Case 1: Create and delete a fiber
FOSSIL_TEST(fossil_fiber_create_delete_case) {
    test_fiber = fossil_fiber_create(4096, fiber_task, NULL);
    ASSUME_NOT_CNULL(test_fiber);  // Check if the fiber was successfully created
    fossil_fiber_delete(test_fiber);  // Ensure deletion succeeds
}

// Test Case 2: Switch between main fiber and created fiber
FOSSIL_TEST(fossil_fiber_switch_case) {
    int value = 0;

    // Create the fiber with a task that increments the value
    test_fiber = fossil_fiber_create(4096, fiber_task, &value);
    ASSUME_NOT_CNULL(test_fiber);

    // Switch to the fiber, which should run its task
    fossil_fiber_switch(test_fiber);

    // Verify that the fiber task executed properly and modified the value
    ASSUME_ITS_EQUAL_I32(10, value);

    // Switch back to the main fiber (which is implicit here since the main fiber was converted)
}

// Test Case 3: Convert main thread to fiber and switch back
FOSSIL_TEST(fossil_fiber_convert_case) {
    int value = 0;

    // Convert the main thread to a fiber
    fossil_fiber_t fiber_main = fossil_fiber_convert(NULL);
    ASSUME_NOT_CNULL(fiber_main);  // Ensure the main fiber was successfully created

    // Create a new fiber with a task
    test_fiber = fossil_fiber_create(4096, fiber_task, &value);
    ASSUME_NOT_CNULL(test_fiber);

    // Switch to the test fiber and execute its task
    fossil_fiber_switch(test_fiber);
    ASSUME_ITS_EQUAL_I32(10, value);  // Check if the task ran

    // Switch back to the main fiber
    fossil_fiber_switch(fiber_main);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_fiber_tests) {
    ADD_TESTF(fossil_fiber_create_delete_case, fixture_fiber);
    ADD_TESTF(fossil_fiber_switch_case, fixture_fiber);
    ADD_TESTF(fossil_fiber_convert_case, fixture_fiber);
}