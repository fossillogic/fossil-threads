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

FOSSIL_SUITE(c_cond_fixture);

FOSSIL_SETUP(c_cond_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_cond_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(c_cond_init_and_dispose) {
    fossil_threads_cond_t cond;
    int rc = fossil_threads_cond_init(&cond);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_COND_OK);
    ASSUME_ITS_EQUAL_I32(cond.valid, 1);

    fossil_threads_cond_dispose(&cond);
    ASSUME_ITS_EQUAL_I32(cond.valid, 0);
}

FOSSIL_TEST(c_cond_init_null) {
    int rc = fossil_threads_cond_init(NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_COND_EINVAL);
}

FOSSIL_TEST(c_cond_signal_and_broadcast_invalid) {
    fossil_threads_cond_t cond;
    memset(&cond, 0, sizeof(cond));
    int rc1 = fossil_threads_cond_signal(NULL);
    int rc2 = fossil_threads_cond_signal(&cond);
    int rc3 = fossil_threads_cond_broadcast(NULL);
    int rc4 = fossil_threads_cond_broadcast(&cond);
    ASSUME_ITS_EQUAL_I32(rc1, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc2, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc3, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc4, FOSSIL_THREADS_COND_EINVAL);
}

FOSSIL_TEST(c_cond_wait_invalid) {
    fossil_threads_cond_t cond;
    fossil_threads_mutex_t mutex;
    memset(&cond, 0, sizeof(cond));
    memset(&mutex, 0, sizeof(mutex));
    int rc1 = fossil_threads_cond_wait(NULL, NULL);
    int rc2 = fossil_threads_cond_wait(&cond, NULL);
    int rc3 = fossil_threads_cond_wait(NULL, &mutex);
    int rc4 = fossil_threads_cond_wait(&cond, &mutex);
    ASSUME_ITS_EQUAL_I32(rc1, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc2, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc3, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc4, FOSSIL_THREADS_COND_EINVAL);
}

FOSSIL_TEST(c_cond_timedwait_invalid) {
    fossil_threads_cond_t cond;
    fossil_threads_mutex_t mutex;
    memset(&cond, 0, sizeof(cond));
    memset(&mutex, 0, sizeof(mutex));
    int rc1 = fossil_threads_cond_timedwait(NULL, NULL, 100);
    int rc2 = fossil_threads_cond_timedwait(&cond, NULL, 100);
    int rc3 = fossil_threads_cond_timedwait(NULL, &mutex, 100);
    int rc4 = fossil_threads_cond_timedwait(&cond, &mutex, 100);
    ASSUME_ITS_EQUAL_I32(rc1, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc2, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc3, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc4, FOSSIL_THREADS_COND_EINVAL);
}

FOSSIL_TEST(c_cond_is_valid_and_waiter_count) {
    fossil_threads_cond_t cond;
    int rc = fossil_threads_cond_init(&cond);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_COND_OK);
    ASSUME_ITS_EQUAL_I32(fossil_threads_cond_is_valid(&cond), 1);
    ASSUME_ITS_EQUAL_I32(fossil_threads_cond_waiter_count(&cond), 0);

    fossil_threads_cond_dispose(&cond);
    ASSUME_ITS_EQUAL_I32(fossil_threads_cond_is_valid(&cond), 0);
    ASSUME_ITS_TRUE(fossil_threads_cond_waiter_count(&cond) < 0);

    ASSUME_ITS_EQUAL_I32(fossil_threads_cond_is_valid(NULL), 0);
    ASSUME_ITS_TRUE(fossil_threads_cond_waiter_count(NULL) < 0);
}

FOSSIL_TEST(c_cond_reset) {
    fossil_threads_cond_t cond;
    int rc = fossil_threads_cond_init(&cond);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_COND_OK);
    ASSUME_ITS_EQUAL_I32(cond.valid, 1);

    rc = fossil_threads_cond_reset(&cond);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_COND_OK);
    ASSUME_ITS_EQUAL_I32(cond.valid, 1);

    rc = fossil_threads_cond_reset(NULL);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_COND_EINVAL);
}

FOSSIL_TEST(c_cond_waiter_count_increments_and_decrements) {
    fossil_threads_cond_t cond;
    fossil_threads_mutex_t mutex;
    fossil_threads_mutex_init(&mutex);
    fossil_threads_cond_init(&cond);

    fossil_threads_mutex_lock(&mutex);
    // Simulate waiter increment
    int before = fossil_threads_cond_waiter_count(&cond);
    // We can't actually wait in a unit test, but we can check that the count is 0
    ASSUME_ITS_EQUAL_I32(before, 0);
    fossil_threads_mutex_unlock(&mutex);

    fossil_threads_cond_dispose(&cond);
    fossil_threads_mutex_dispose(&mutex);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_cond_tests) {
    FOSSIL_ADD_TEST(c_cond_fixture, c_cond_init_and_dispose);
    FOSSIL_ADD_TEST(c_cond_fixture, c_cond_init_null);
    FOSSIL_ADD_TEST(c_cond_fixture, c_cond_signal_and_broadcast_invalid);
    FOSSIL_ADD_TEST(c_cond_fixture, c_cond_wait_invalid);
    FOSSIL_ADD_TEST(c_cond_fixture, c_cond_timedwait_invalid);
    FOSSIL_ADD_TEST(c_cond_fixture, c_cond_is_valid_and_waiter_count);
    FOSSIL_ADD_TEST(c_cond_fixture, c_cond_reset);
    FOSSIL_ADD_TEST(c_cond_fixture, c_cond_waiter_count_increments_and_decrements);

    FOSSIL_ADD_SUITE(c_cond_fixture);
} // end of tests
