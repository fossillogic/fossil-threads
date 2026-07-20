/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2013
 *
 * Copyright (C) 2013-Current Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include <fossil/maip/framework.h>
#include "fossil/threads/framework.h"


// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_SUITE(c_thread_fixture);

FOSSIL_SETUP(c_thread_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_thread_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

/* ---------- Lifecycle ---------- */

static void *test_thread_func_noop(void *arg) {
    (void)arg;
#if defined(_WIN32)
    return (void*)(uintptr_t)0;
#else
    return NULL;
#endif
}

static void *test_thread_func_sleep(void *arg) {
    unsigned int ms = arg ? *(unsigned int *)arg : 10;
    fossil_threads_thread_sleep_ms(ms);
#if defined(_WIN32)
    return (void *)(uintptr_t)ms;
#else
    return (void *)(uintptr_t)ms;
#endif
}

/* ---------- Lifecycle ---------- */

FOSSIL_TEST(c_thread_dispose_null_safe) {
    fossil_threads_thread_dispose(NULL);
    ASSUME_ITS_TRUE(1);
}

FOSSIL_TEST(c_thread_create_twice_should_fail) {
    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);

    int rc = fossil_threads_thread_create(&thread, test_thread_func_noop, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    rc = fossil_threads_thread_create(&thread, test_thread_func_noop, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_EBUSY);

    fossil_threads_thread_join(&thread, NULL);
    fossil_threads_thread_dispose(&thread);
}

FOSSIL_TEST(c_thread_join_twice_should_fail) {
    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);

    int rc = fossil_threads_thread_create(&thread, test_thread_func_noop, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    rc = fossil_threads_thread_join(&thread, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    rc = fossil_threads_thread_join(&thread, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_EDETACHED);

    fossil_threads_thread_dispose(&thread);
}

FOSSIL_TEST(c_thread_detach_twice_should_fail) {
    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);

    int rc = fossil_threads_thread_create(&thread, test_thread_func_noop, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    rc = fossil_threads_thread_detach(&thread);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    rc = fossil_threads_thread_detach(&thread);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_EDETACHED);

    fossil_threads_thread_dispose(&thread);
}

FOSSIL_TEST(c_thread_sleep_and_return_value) {
    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);

    unsigned int ms = 25;
    void *ret = NULL;
    int rc = fossil_threads_thread_create(&thread, test_thread_func_sleep, &ms);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    rc = fossil_threads_thread_join(&thread, &ret);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);
    ASSUME_ITS_EQUAL_I32((unsigned int)(uintptr_t)ret, ms);

    fossil_threads_thread_dispose(&thread);
}

FOSSIL_TEST(c_thread_equal_null_and_self) {
    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);

    ASSUME_ITS_TRUE(fossil_threads_thread_equal(&thread, &thread));
    ASSUME_ITS_TRUE(!fossil_threads_thread_equal(&thread, NULL));
    ASSUME_ITS_TRUE(!fossil_threads_thread_equal(NULL, &thread));
    ASSUME_ITS_TRUE(fossil_threads_thread_equal(NULL, NULL));

    fossil_threads_thread_dispose(&thread);
}

FOSSIL_TEST(c_thread_create_and_join) {
    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);

    int rc = fossil_threads_thread_create(&thread, test_thread_func_noop, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    rc = fossil_threads_thread_join(&thread, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    fossil_threads_thread_dispose(&thread);
}

FOSSIL_TEST(c_thread_create_and_detach) {
    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);

    int rc = fossil_threads_thread_create(&thread, test_thread_func_noop, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    rc = fossil_threads_thread_detach(&thread);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    fossil_threads_thread_dispose(&thread);
}

FOSSIL_TEST(c_thread_yield_and_sleep) {
    int rc = fossil_threads_thread_yield();
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);
    rc = fossil_threads_thread_sleep_ms(5);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);
}

FOSSIL_TEST(c_thread_create_invalid_args) {
    int rc = fossil_threads_thread_create(NULL, test_thread_func_noop, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_EINVAL);

    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);
    rc = fossil_threads_thread_create(&thread, NULL, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_EINVAL);

    fossil_threads_thread_dispose(&thread);
}

FOSSIL_TEST(c_thread_priority_set_get) {
    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);

    int rc = fossil_threads_thread_set_priority(&thread, 5);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);
    ASSUME_ITS_EQUAL_I32(fossil_threads_thread_get_priority(&thread), 5);

    rc = fossil_threads_thread_set_priority(NULL, 1);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_EINVAL);
    ASSUME_ITS_EQUAL_I32(fossil_threads_thread_get_priority(NULL), FOSSIL_THREADS_EINVAL);

    fossil_threads_thread_dispose(&thread);
}

FOSSIL_TEST(c_thread_cancel_and_is_running) {
    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);

    int rc = fossil_threads_thread_cancel(&thread);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_ENOTSTARTED);

    ASSUME_ITS_TRUE(!fossil_threads_thread_is_running(NULL));
    ASSUME_ITS_TRUE(!fossil_threads_thread_is_running(&thread));

    rc = fossil_threads_thread_create(&thread, test_thread_func_sleep, NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    ASSUME_ITS_TRUE(fossil_threads_thread_is_running(&thread));

    fossil_threads_thread_join(&thread, NULL);
    ASSUME_ITS_TRUE(!fossil_threads_thread_is_running(&thread));

    fossil_threads_thread_dispose(&thread);
}

FOSSIL_TEST(c_thread_get_retval) {
    fossil_threads_thread_t thread;
    fossil_threads_thread_init(&thread);

    ASSUME_ITS_TRUE(fossil_threads_thread_get_retval(NULL) == NULL);
    ASSUME_ITS_TRUE(fossil_threads_thread_get_retval(&thread) == NULL);

    unsigned int ms = 42;
    int rc = fossil_threads_thread_create(&thread, test_thread_func_sleep, &ms);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_OK);

    fossil_threads_thread_join(&thread, NULL);
    void *ret = fossil_threads_thread_get_retval(&thread);
    ASSUME_ITS_EQUAL_I32((unsigned int)(uintptr_t)ret, ms);

    fossil_threads_thread_dispose(&thread);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_thread_tests) {    
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_dispose_null_safe);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_create_twice_should_fail);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_join_twice_should_fail);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_detach_twice_should_fail);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_sleep_and_return_value);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_equal_null_and_self);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_create_and_join);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_create_and_detach);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_yield_and_sleep);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_create_invalid_args);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_priority_set_get);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_cancel_and_is_running);
    FOSSIL_ADD_TEST(c_thread_fixture, c_thread_get_retval);

    FOSSIL_ADD_SUITE(c_thread_fixture);
} // end of tests
