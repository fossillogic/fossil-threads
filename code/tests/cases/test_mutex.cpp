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

FOSSIL_SUITE(cpp_mutex_fixture);

FOSSIL_SETUP(cpp_mutex_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_mutex_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(cpp_thread_mutex_trylock_success) {
    fossil::threads::Mutex m;
    bool locked = m.try_lock();
    ASSUME_ITS_EQUAL_I32(locked, true);
    m.unlock();

    // Also exercise the timed helper with zero-duration fallback to try_lock.
    bool locked_zero = m.try_lock_for(std::chrono::milliseconds(0));
    ASSUME_ITS_EQUAL_I32(locked_zero, true);
    m.unlock();
}

FOSSIL_TEST(cpp_thread_mutex_lock_blocks_other_thread_trylock) {
    fossil::threads::Mutex m;
    std::atomic<bool> started{false};

    std::thread t([&]() {
        m.lock();
        started.store(true, std::memory_order_release);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        m.unlock();
    });

    // wait for the helper thread to acquire the lock
    while (!started.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }

    // while the other thread holds the lock, try_lock should fail
    bool locked_while_held = m.try_lock();
    ASSUME_ITS_EQUAL_I32(locked_while_held, false);

    // timed try_lock_for with a small timeout should also fail while held
    bool timed_short = m.try_lock_for(std::chrono::milliseconds(50));
    ASSUME_ITS_EQUAL_I32(timed_short, false);

    t.join();

    // after join (thread released the lock), try_lock_for should succeed
    bool locked_after = m.try_lock_for(std::chrono::seconds(1));
    ASSUME_ITS_EQUAL_I32(locked_after, true);
    m.unlock();

    // try_lock_until: compute a near-future deadline and expect success
    auto tp = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    bool until_ok = m.try_lock_until(tp);
    ASSUME_ITS_EQUAL_I32(until_ok, true);
    m.unlock();
}

FOSSIL_TEST(cpp_thread_mutex_lock_unlock_sequence) {
    // Basic lock/unlock sequence using LockGuard RAII
    fossil::threads::Mutex m;
    for (int i = 0; i < 3; ++i) {
        fossil::threads::Mutex::LockGuard g(m);
    }

    // Exercise move-construction and move-assignment semantics.
    // After moving, the moved-from object must not be used.
    fossil::threads::Mutex moved_ctor(std::move(m));
    fossil::threads::Mutex moved_assign;
    moved_assign = std::move(moved_ctor);

    {
        fossil::threads::Mutex::LockGuard g(moved_assign);
    }

    // If we reached here without exceptions, consider the sequence successful.
    ASSUME_ITS_EQUAL_I32(1, 1);
}

FOSSIL_TEST(cpp_thread_mutex_raii_basic) {
    fossil::threads::Mutex m;
    // Should be able to lock/unlock without exceptions
    m.lock();
    m.unlock();
    // Try lock should succeed when unlocked
    bool ok = m.try_lock();
    ASSUME_ITS_EQUAL_I32(ok, true);
    m.unlock();
}

FOSSIL_TEST(cpp_thread_mutex_raii_lockguard) {
    fossil::threads::Mutex m;
    {
        fossil::threads::Mutex::LockGuard g(m);
        // Should be locked here (no direct way to check, but no deadlock)
    }
    // Should be unlocked here
    bool ok = m.try_lock();
    ASSUME_ITS_EQUAL_I32(ok, true);
    m.unlock();
}

FOSSIL_TEST(cpp_thread_mutex_raii_move_ctor) {
    fossil::threads::Mutex m;
    m.lock();
    m.unlock();
    fossil::threads::Mutex m2(std::move(m));
    // m should not be used anymore
    bool ok = m2.try_lock();
    ASSUME_ITS_EQUAL_I32(ok, true);
    m2.unlock();
}

FOSSIL_TEST(cpp_thread_mutex_raii_move_assign) {
    fossil::threads::Mutex m;
    fossil::threads::Mutex m2;
    m.lock();
    m.unlock();
    m2 = std::move(m);
    // m should not be used anymore
    bool ok = m2.try_lock();
    ASSUME_ITS_EQUAL_I32(ok, true);
    m2.unlock();
}

FOSSIL_TEST(cpp_thread_mutex_raii_try_lock_for) {
    fossil::threads::Mutex m;
    bool ok = m.try_lock_for(std::chrono::milliseconds(10));
    ASSUME_ITS_EQUAL_I32(ok, true);
    m.unlock();
}

FOSSIL_TEST(cpp_thread_mutex_raii_try_lock_until) {
    fossil::threads::Mutex m;
    auto tp = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
    bool ok = m.try_lock_until(tp);
    ASSUME_ITS_EQUAL_I32(ok, true);
    m.unlock();
}

FOSSIL_TEST(cpp_thread_mutex_raii_exceptions) {
    fossil::threads::Mutex m;
    // forcibly move-from to invalidate
    fossil::threads::Mutex m2(std::move(m));
    try {
        m.lock();
        ASSUME_ITS_TRUE(false); // Should not reach here
    } catch (const std::runtime_error&) {
        ASSUME_ITS_TRUE(true);
    }
    try {
        m.try_lock();
        ASSUME_ITS_TRUE(false);
    } catch (const std::runtime_error&) {
        ASSUME_ITS_TRUE(true);
    }
    try {
        m.unlock();
        ASSUME_ITS_TRUE(false);
    } catch (const std::runtime_error&) {
        ASSUME_ITS_TRUE(true);
    }
}

FOSSIL_TEST_GROUP(cpp_mutex_tests) {
    FOSSIL_ADD_TEST(cpp_mutex_fixture, cpp_thread_mutex_trylock_success);
    FOSSIL_ADD_TEST(cpp_mutex_fixture, cpp_thread_mutex_lock_blocks_other_thread_trylock);
    FOSSIL_ADD_TEST(cpp_mutex_fixture, cpp_thread_mutex_lock_unlock_sequence);
    FOSSIL_ADD_TEST(cpp_mutex_fixture, cpp_thread_mutex_raii_basic);
    FOSSIL_ADD_TEST(cpp_mutex_fixture, cpp_thread_mutex_raii_lockguard);
    FOSSIL_ADD_TEST(cpp_mutex_fixture, cpp_thread_mutex_raii_move_ctor);
    FOSSIL_ADD_TEST(cpp_mutex_fixture, cpp_thread_mutex_raii_move_assign);
    FOSSIL_ADD_TEST(cpp_mutex_fixture, cpp_thread_mutex_raii_try_lock_for);
    FOSSIL_ADD_TEST(cpp_mutex_fixture, cpp_thread_mutex_raii_try_lock_until);
    FOSSIL_ADD_TEST(cpp_mutex_fixture, cpp_thread_mutex_raii_exceptions);

    FOSSIL_ADD_SUITE(cpp_mutex_fixture);
} // end of tests
