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

FOSSIL_SUITE(cpp_cond_fixture);

FOSSIL_SETUP(cpp_cond_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_cond_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

using fossil::threads::Cond;

FOSSIL_TEST(cpp_cond_raii_init_and_dispose) {
    Cond* cond = nullptr;
    try {
        cond = new Cond();
        ASSUME_ITS_TRUE(cond != nullptr);
    } catch (...) {
        ASSUME_ITS_TRUE(false); // Should not throw
    }
    delete cond;
    // No explicit check for valid, as RAII handles it
}

FOSSIL_TEST(cpp_cond_raii_signal_and_broadcast) {
    Cond cond;
    int rc1 = cond.signal();
    int rc2 = cond.broadcast();
    ASSUME_ITS_EQUAL_I32(rc1, FOSSIL_THREADS_COND_OK);
    ASSUME_ITS_EQUAL_I32(rc2, FOSSIL_THREADS_COND_OK);
}

FOSSIL_TEST(cpp_cond_raii_wait_invalid_mutex) {
    Cond cond;
    int rc1 = cond.wait(nullptr);
    int rc2 = cond.timed_wait(nullptr, 100);
    ASSUME_ITS_EQUAL_I32(rc1, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc2, FOSSIL_THREADS_COND_EINVAL);
}

FOSSIL_TEST(cpp_cond_raii_wait_and_timedwait) {
    Cond cond;
    fossil_threads_mutex_t mutex;
    memset(&mutex, 0, sizeof(mutex));
    // mutex is not initialized, so wait should fail
    int rc1 = cond.wait(&mutex);
    int rc2 = cond.timed_wait(&mutex, 100);
    ASSUME_ITS_EQUAL_I32(rc1, FOSSIL_THREADS_COND_EINVAL);
    ASSUME_ITS_EQUAL_I32(rc2, FOSSIL_THREADS_COND_EINVAL);
}

FOSSIL_TEST(cpp_cond_is_valid_and_waiter_count) {
    using fossil::threads::Cond;
    Cond cond;
    ASSUME_ITS_TRUE(cond.is_valid());
    ASSUME_ITS_EQUAL_I32(cond.waiter_count(), 0);

    cond.~Cond(); // Explicitly call destructor to dispose
    // After destruction, cond is not valid, but accessing it is UB.
    // So, create a new Cond and test invalid handle via native_handle().
    fossil_threads_cond_t* invalid = nullptr;
    ASSUME_ITS_EQUAL_I32(fossil_threads_cond_is_valid(invalid), 0);
    ASSUME_ITS_TRUE(fossil_threads_cond_waiter_count(invalid) < 0);
}

FOSSIL_TEST(cpp_cond_reset) {
    using fossil::threads::Cond;
    Cond cond;
    ASSUME_ITS_TRUE(cond.is_valid());

    int rc = cond.reset();
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_COND_OK);
    ASSUME_ITS_TRUE(cond.is_valid());

    // Reset with nullptr (simulate error)
    rc = fossil_threads_cond_reset(nullptr);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_THREADS_COND_EINVAL);
}

FOSSIL_TEST(cpp_cond_waiter_count_increments_and_decrements) {
    using fossil::threads::Cond;
    Cond cond;
    fossil_threads_mutex_t mutex;
    fossil_threads_mutex_init(&mutex);

    fossil_threads_mutex_lock(&mutex);
    int before = cond.waiter_count();
    ASSUME_ITS_EQUAL_I32(before, 0);
    fossil_threads_mutex_unlock(&mutex);

    fossil_threads_mutex_dispose(&mutex);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_cond_tests) {
    FOSSIL_ADD_TEST(cpp_cond_fixture, cpp_cond_raii_init_and_dispose);
    FOSSIL_ADD_TEST(cpp_cond_fixture, cpp_cond_raii_signal_and_broadcast);
    FOSSIL_ADD_TEST(cpp_cond_fixture, cpp_cond_raii_wait_invalid_mutex);
    FOSSIL_ADD_TEST(cpp_cond_fixture, cpp_cond_raii_wait_and_timedwait);
    FOSSIL_ADD_TEST(cpp_cond_fixture, cpp_cond_is_valid_and_waiter_count);
    FOSSIL_ADD_TEST(cpp_cond_fixture, cpp_cond_reset);
    FOSSIL_ADD_TEST(cpp_cond_fixture, cpp_cond_waiter_count_increments_and_decrements);

    FOSSIL_ADD_SUITE(cpp_cond_fixture);
} // end of tests
