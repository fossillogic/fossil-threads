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
#ifndef FOSSIL_THREADS_MUTEX_H
#define FOSSIL_THREADS_MUTEX_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdbool.h>

#if defined(_WIN32) && defined(FOSSIL_THREADS_BUILD_DLL)
#  define FOSSIL_THREADS_API __declspec(dllexport)
#elif defined(_WIN32) && defined(FOSSIL_THREADS_USE_DLL)
#  define FOSSIL_THREADS_API __declspec(dllimport)
#else
#  define FOSSIL_THREADS_API
#endif

/* ---------- Types ---------- */

typedef struct fossil_threads_mutex {
    void *handle;      /* CRITICAL_SECTION* or pthread_mutex_t* */
    int   valid;       /* 1 if initialized */
    int   locked;      /* 1 if currently locked, 0 otherwise */
    int   recursive;   /* 1 if recursive mutex, 0 otherwise */
} fossil_threads_mutex_t;

/* ---------- Lifecycle ---------- */

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/* 
 * Initializes a mutex object.
 * 
 * Parameters:
 *   m - Pointer to a fossil_threads_mutex_t structure to initialize.
 * 
 * Returns:
 *   0 on success, or a nonzero error code on failure.
 * 
 * Notes:
 *   - The mutex must be disposed with fossil_threads_mutex_dispose when no longer needed.
 *   - The mutex is initially unlocked.
 */
FOSSIL_THREADS_API int fossil_threads_mutex_init(fossil_threads_mutex_t *m);

/* 
 * Disposes (destroys) a mutex object.
 * 
 * Parameters:
 *   m - Pointer to a fossil_threads_mutex_t structure to dispose.
 * 
 * Notes:
 *   - Safe to call on a zeroed or already-disposed mutex.
 *   - After disposal, the mutex should not be used unless re-initialized.
 */
FOSSIL_THREADS_API void fossil_threads_mutex_dispose(fossil_threads_mutex_t *m);

/* ---------- Locking ---------- */

/* 
 * Locks the mutex, blocking the calling thread until the mutex becomes available.
 * 
 * Parameters:
 *   m - Pointer to a fossil_threads_mutex_t structure to lock.
 * 
 * Returns:
 *   0 on success, or a nonzero error code on failure.
 * 
 * Notes:
 *   - If the mutex is already locked by another thread, this call will block.
 *   - Recursive locking is not supported unless implemented by the platform.
 */
FOSSIL_THREADS_API int fossil_threads_mutex_lock(fossil_threads_mutex_t *m);

/* 
 * Unlocks the mutex.
 * 
 * Parameters:
 *   m - Pointer to a fossil_threads_mutex_t structure to unlock.
 * 
 * Returns:
 *   0 on success, or a nonzero error code on failure.
 * 
 * Notes:
 *   - Only the thread that locked the mutex should unlock it.
 *   - Unlocking an unlocked mutex results in undefined behavior.
 */
FOSSIL_THREADS_API int fossil_threads_mutex_unlock(fossil_threads_mutex_t *m);

/* 
 * Attempts to lock the mutex without blocking.
 * 
 * Parameters:
 *   m - Pointer to a fossil_threads_mutex_t structure to try to lock.
 * 
 * Returns:
 *   0 if the mutex was successfully locked.
 *   FOSSIL_THREADS_MUTEX_EBUSY if the mutex is already locked.
 *   Other nonzero error code on failure.
 * 
 * Notes:
 *   - This function never blocks.
 *   - Useful for implementing timed or non-blocking synchronization.
 */
FOSSIL_THREADS_API int fossil_threads_mutex_trylock(fossil_threads_mutex_t *m);

/* 
 * Checks if the mutex is currently locked.
 * 
 * Parameters:
 *   m - Pointer to a fossil_threads_mutex_t structure to check.
 * 
 * Returns:
 *   true if the mutex is locked, false otherwise.
 * 
 * Notes:
 *   - This is a non-blocking check.
 *   - The result may be immediately outdated due to race conditions.
 */
FOSSIL_THREADS_API bool fossil_threads_mutex_is_locked(const fossil_threads_mutex_t *m);

/* 
 * Checks if the mutex has been initialized.
 * 
 * Parameters:
 *   m - Pointer to a fossil_threads_mutex_t structure to check.
 * 
 * Returns:
 *   true if the mutex is initialized, false otherwise.
 */
FOSSIL_THREADS_API bool fossil_threads_mutex_is_initialized(const fossil_threads_mutex_t *m);

/* 
 * Resets the mutex to an unlocked and uninitialized state.
 * 
 * Parameters:
 *   m - Pointer to a fossil_threads_mutex_t structure to reset.
 * 
 * Notes:
 *   - This is equivalent to disposing and zeroing the mutex.
 *   - After reset, the mutex must be re-initialized before use.
 */
FOSSIL_THREADS_API void fossil_threads_mutex_reset(fossil_threads_mutex_t *m);

/* Error codes */
enum {
    FOSSIL_THREADS_MUTEX_OK        = 0,   /* Success */
    FOSSIL_THREADS_MUTEX_EINVAL    = 22,  /* Invalid argument */
    FOSSIL_THREADS_MUTEX_EBUSY     = 16,  /* Already locked (trylock only) */
    FOSSIL_THREADS_MUTEX_EINTERNAL = 199, /* Internal error */
    FOSSIL_THREADS_MUTEX_ENOMEM    = 12,  /* Out of memory */
    FOSSIL_THREADS_MUTEX_EPERM     = 1,   /* Operation not permitted */
    FOSSIL_THREADS_MUTEX_EDEADLK   = 35,  /* Deadlock detected */
    FOSSIL_THREADS_MUTEX_ENOTINIT  = 100, /* Mutex not initialized */
    FOSSIL_THREADS_MUTEX_EUNLOCK   = 101  /* Unlock of unlocked mutex */
};

#ifdef __cplusplus
}
#include <stdexcept>
#include <utility>
#include <thread>
#include <chrono>
#include <atomic>
#include <type_traits>

namespace fossil {

    namespace threads {

        /**
         * @brief A thin C++ RAII wrapper around the C mutex API provided by Fossil Threads.
         *
         * This class owns a fossil_threads_mutex_t and exposes a C++-friendly interface:
         *  - RAII initialization and disposal in ctor/dtor
         *  - non-copyable, movable
         *  - lock/unlock/try_lock operations that translate error codes into exceptions
         *  - timed try-lock helpers implemented via repeated non-blocking attempts
         *  - a small LockGuard nested class for scoped locking
         *
         * The underlying C mutex implementation may be platform-specific (e.g. CRITICAL_SECTION
         * on Windows or pthread_mutex_t on POSIX). This wrapper does not attempt to emulate
         * features not provided by the underlying implementation (for example, fairness or
         * recursive locking).
         */
        class Mutex {
        public:
            /**
             * @brief Construct and initialize the underlying C mutex.
             */
            Mutex()
            : m_{nullptr, 0, 0, 0}, initialized_{false}
            {
            int rc = fossil_threads_mutex_init(&m_);
            if (rc != FOSSIL_THREADS_MUTEX_OK) {
                throw std::runtime_error("Failed to initialize mutex");
            }
            initialized_.store(true, std::memory_order_release);
            }

            /**
             * @brief Destructor disposes the underlying C mutex if it is initialized.
             */
            ~Mutex() {
            if (initialized_.load(std::memory_order_acquire)) {
                fossil_threads_mutex_dispose(&m_);
                initialized_.store(false, std::memory_order_release);
            }
            }

            /**
             * @brief Move constructor.
             */
            Mutex(Mutex&& other) noexcept
            : m_(std::exchange(other.m_, fossil_threads_mutex_t{nullptr, 0, 0, 0})),
              initialized_(other.initialized_.load(std::memory_order_acquire))
            {
            other.initialized_.store(false, std::memory_order_release);
            }

            /**
             * @brief Move assignment operator.
             */
            Mutex& operator=(Mutex&& other) noexcept {
            if (this != &other) {
                if (initialized_.load(std::memory_order_acquire)) {
                fossil_threads_mutex_dispose(&m_);
                }
                m_ = std::exchange(other.m_, fossil_threads_mutex_t{nullptr, 0, 0, 0});
                initialized_.store(other.initialized_.load(std::memory_order_acquire),
                          std::memory_order_release);
                other.initialized_.store(false, std::memory_order_release);
            }
            return *this;
            }

            Mutex(const Mutex&) = delete;
            Mutex& operator=(const Mutex&) = delete;

            /**
             * @brief Lock the mutex, blocking until the lock is acquired.
             */
            void lock() {
            if (!initialized_.load(std::memory_order_acquire)) {
                throw std::runtime_error("Lock on uninitialized mutex");
            }
            int rc = fossil_threads_mutex_lock(&m_);
            if (rc != FOSSIL_THREADS_MUTEX_OK) {
                throw std::runtime_error("Failed to lock mutex");
            }
            }

            /**
             * @brief Unlock the mutex.
             */
            void unlock() {
            if (!initialized_.load(std::memory_order_acquire)) {
                throw std::runtime_error("Unlock on uninitialized mutex");
            }
            int rc = fossil_threads_mutex_unlock(&m_);
            if (rc != FOSSIL_THREADS_MUTEX_OK) {
                throw std::runtime_error("Failed to unlock mutex");
            }
            }

            /**
             * @brief Attempt to lock the mutex without blocking.
             */
            bool try_lock() {
            if (!initialized_.load(std::memory_order_acquire)) {
                throw std::runtime_error("Try_lock on uninitialized mutex");
            }
            int rc = fossil_threads_mutex_trylock(&m_);
            if (rc == FOSSIL_THREADS_MUTEX_OK) return true;
            if (rc == FOSSIL_THREADS_MUTEX_EBUSY) return false;
            throw std::runtime_error("Failed to try_lock mutex");
            }

            /**
             * @brief Attempt to lock the mutex, waiting up to rel_time duration.
             */
            bool try_lock_for(const std::chrono::steady_clock::duration& rel_time) {
            if (!initialized_.load(std::memory_order_acquire)) {
                throw std::runtime_error("Timed try_lock on uninitialized mutex");
            }

            if (rel_time <= std::chrono::steady_clock::duration::zero()) {
                return try_lock();
            }

            auto deadline = std::chrono::steady_clock::now() + rel_time;
            while (std::chrono::steady_clock::now() < deadline) {
                int rc = fossil_threads_mutex_trylock(&m_);
                if (rc == FOSSIL_THREADS_MUTEX_OK) return true;
                if (rc != FOSSIL_THREADS_MUTEX_EBUSY) {
                throw std::runtime_error("Failed to try_lock mutex");
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return false;
            }

            /**
             * @brief Convenience overload: try_lock_for with milliseconds.
             */
            bool try_lock_for(const std::chrono::milliseconds& ms) {
            return try_lock_for(std::chrono::duration_cast<std::chrono::steady_clock::duration>(ms));
            }

            /**
             * @brief Convenience overload: try_lock_for with seconds.
             */
            bool try_lock_for(const std::chrono::seconds& s) {
            return try_lock_for(std::chrono::duration_cast<std::chrono::steady_clock::duration>(s));
            }

            /**
             * @brief Attempt to lock the mutex until the specified time point.
             */
            bool try_lock_until(const std::chrono::steady_clock::time_point& tp) {
            if (!initialized_.load(std::memory_order_acquire)) {
                throw std::runtime_error("Timed try_lock on uninitialized mutex");
            }

            if (std::chrono::steady_clock::now() >= tp) {
                return try_lock();
            }

            while (std::chrono::steady_clock::now() < tp) {
                int rc = fossil_threads_mutex_trylock(&m_);
                if (rc == FOSSIL_THREADS_MUTEX_OK) return true;
                if (rc != FOSSIL_THREADS_MUTEX_EBUSY) {
                throw std::runtime_error("Failed to try_lock mutex");
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return false;
            }

            /**
             * @brief A small RAII lock helper that locks the given Mutex on construction
             * and unlocks it on destruction.
             */
            class LockGuard {
            public:
            explicit LockGuard(Mutex& m) : m_(m) { m_.lock(); }
            ~LockGuard() noexcept { try { m_.unlock(); } catch (...) {} }
            LockGuard(const LockGuard&) = delete;
            LockGuard& operator=(const LockGuard&) = delete;
            private:
            Mutex& m_;
            };

        private:
            fossil_threads_mutex_t m_;
            std::atomic<bool> initialized_;
        };

        static_assert(!std::is_copy_constructible_v<Mutex>, "Mutex must not be copyable");
        static_assert(std::is_nothrow_move_constructible_v<Mutex>, "Mutex should be noexcept-movable");

    } // namespace threads

} // namespace fossil

#endif

#endif /* FOSSIL_THREADS_MUTEX_H */
