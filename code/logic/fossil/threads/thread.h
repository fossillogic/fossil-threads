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
#ifndef FOSSIL_THREADS_THREAD_H
#define FOSSIL_THREADS_THREAD_H

#include <errno.h>      /* for errno, EINTR, ESRCH */
#include <signal.h>     /* for pthread_kill */
#include <time.h>       /* for nanosleep */
#include <stdlib.h>     /* for malloc/free */
#include <string.h>     /* for memset, etc. */

#ifdef __cplusplus
extern "C"
{
#endif

/* ---------- Configuration / visibility ---------- */
#if defined(_WIN32) && defined(FOSSIL_THREADS_BUILD_DLL)
#  define FOSSIL_THREADS_API __declspec(dllexport)
#elif defined(_WIN32) && defined(FOSSIL_THREADS_USE_DLL)
#  define FOSSIL_THREADS_API __declspec(dllimport)
#else
#  define FOSSIL_THREADS_API
#endif

/* ---------- Types ---------- */

/* ---------- Thread API ---------- */

/* User thread entry point: return value is optionally retrieved by join(). */
typedef void* (*fossil_threads_thread_func)(void *arg);

/* -------------------------------------------------------------------------
** Fossil Threads: Thread Handle
** 
** Opaque-ish POD structure. Fields are public for binary compatibility
** and tooling introspection, but should not be modified directly by users.
**
** Portable across POSIX and Windows; members that differ by platform
** are normalized to simple types (void*, unsigned long, int).
** ------------------------------------------------------------------------- */
typedef struct fossil_threads_thread {
    /* --------------------------------------------------------------
    ** Core identity and handle
    ** -------------------------------------------------------------- */
    void *handle;              /* OS handle: HANDLE (Win) or pthread_t* (POSIX) */
    unsigned long id;          /* OS thread ID */
    void *retval;              /* Result from thread join() (set by wrapper) */

    /* --------------------------------------------------------------
    ** Lifecycle and joinability
    ** -------------------------------------------------------------- */
    int   joinable;            /* 1 if joinable, 0 if detached */
    int   started;             /* 1 once successfully created */
    int   finished;            /* 1 after thread returns normally */
    int   cancelled;           /* 1 if cooperative cancel acknowledged */
    int   cancel_requested;    /* 1 if cancel requested (set by cancel()) */

    /* --------------------------------------------------------------
    ** Scheduling and priority
    ** -------------------------------------------------------------- */
    int   priority;            /* thread priority (FOSSIL_THREADS_PRIORITY_*) */
    int   policy;              /* scheduling policy hint (extended use) */

    /* --------------------------------------------------------------
    ** Timing and diagnostics
    ** -------------------------------------------------------------- */
    unsigned long long start_time_ns; /* timestamp when thread started */
    unsigned long long end_time_ns;   /* timestamp when thread finished */
    unsigned long exec_time_ns;       /* derived or measured execution time */
    int   exit_code;                  /* numeric return or mapped OS exit code */
    int   error_code;                 /* last Fossil thread error code */

    /* --------------------------------------------------------------
    ** Reserved / extension
    ** -------------------------------------------------------------- */
    void *userdata;            /* optional pointer for thread-local context */
    void *internal;            /* internal library-specific pointer */
    unsigned int version;      /* ABI version for structure compatibility */
    unsigned int reserved[2];  /* reserved for future growth */
} fossil_threads_thread_t;

/* ---------- Lifecycle ---------- */

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * Create a new thread.
 * Initializes the thread structure and starts execution of the specified function.
 * The thread is created in a joinable state by default.
 *
 * @param thread Pointer to the thread structure to initialize.
 * @param func   Function pointer to the thread entry point.
 * @param arg    Argument to pass to the thread function.
 * @return       0 on success, error code otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_thread_create(
    fossil_threads_thread_t *thread, 
    fossil_threads_thread_func func, 
    void *arg
);

/**
 * Join a thread.
 * Waits for the specified thread to finish execution. Optionally retrieves the
 * return value from the thread function.
 *
 * @param thread Pointer to the thread structure.
 * @param retval Pointer to receive the thread's return value (may be NULL).
 * @return       0 on success, error code otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_thread_join(
    fossil_threads_thread_t *thread, 
    void **retval
);

/**
 * Detach a thread.
 * Marks the thread as detached, releasing resources when it finishes.
 * After detaching, join() is invalid for this thread.
 *
 * @param thread Pointer to the thread structure.
 * @return       0 on success, error code otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_thread_detach(
    fossil_threads_thread_t *thread
);

/**
 * Initialize a thread structure.
 * Sets all fields of the thread structure to a safe, zeroed state.
 * Does not start a thread.
 *
 * @param thread Pointer to the thread structure to initialize.
 */
FOSSIL_THREADS_API void fossil_threads_thread_init(
    fossil_threads_thread_t *thread
);

/**
 * Dispose of a thread structure.
 * Cleans up any resources associated with the thread structure.
 * Does not join or detach running threads.
 *
 * @param thread Pointer to the thread structure to dispose.
 */
FOSSIL_THREADS_API void fossil_threads_thread_dispose(
    fossil_threads_thread_t *thread
);

/* ---------- Management / utilities ---------- */

/**
 * Yield the processor to another thread.
 * Hints the OS scheduler to switch to another thread.
 *
 * @return 0 on success, error code otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_thread_yield(void);

/**
 * Sleep for a specified duration (in milliseconds).
 * Pauses the current thread for the given number of milliseconds.
 *
 * @param ms Number of milliseconds to sleep.
 * @return   0 on success, error code otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_thread_sleep_ms(
    unsigned int ms
);

/**
 * Get the current thread ID.
 * Retrieves the OS-specific identifier for the calling thread.
 *
 * @return The current thread ID as an unsigned long.
 */
FOSSIL_THREADS_API unsigned long fossil_threads_thread_id(void);

/**
 * Compare two thread objects for equality.
 * Checks if both thread structures refer to the same underlying thread.
 *
 * @param t1 Pointer to the first thread structure.
 * @param t2 Pointer to the second thread structure.
 * @return   1 if equal, 0 otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_thread_equal(
    const fossil_threads_thread_t *t1, 
    const fossil_threads_thread_t *t2
);

/* ---------- Extended API ---------- */

/*
 * Set thread priority.
 * @param thread Pointer to the thread structure.
 * @param priority Priority value (OS-dependent, e.g., 0=normal, higher=more priority).
 * @return 0 on success, error code otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_thread_set_priority(
    fossil_threads_thread_t *thread,
    int priority
);

/*
 * Get thread priority.
 * @param thread Pointer to the thread structure.
 * @return Priority value, or negative error code.
 */
FOSSIL_THREADS_API int fossil_threads_thread_get_priority(
    const fossil_threads_thread_t *thread
);

/*
 * Request thread cancellation (cooperative).
 * @param thread Pointer to the thread structure.
 * @return 0 on success, error code otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_thread_cancel(
    fossil_threads_thread_t *thread
);

/*
 * Check if thread is running.
 * @param thread Pointer to the thread structure.
 * @return 1 if running, 0 otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_thread_is_running(
    const fossil_threads_thread_t *thread
);

/*
 * Get thread return value (if finished).
 * @param thread Pointer to the thread structure.
 * @return Return value pointer, or NULL if not finished.
 */
FOSSIL_THREADS_API void* fossil_threads_thread_get_retval(
    const fossil_threads_thread_t *thread
);

/* ============================================================================
** Fossil Threads Error Codes
** Mirrors common errno / GetLastError() semantics,
** with Fossil-specific extensions for cross-platform consistency.
** ==========================================================================*/
enum {
    /* ---------------------------------------------------------------
    ** Generic / success
    ** --------------------------------------------------------------- */
    FOSSIL_THREADS_OK            = 0,    /* success / no error */

    /* ---------------------------------------------------------------
    ** Standard error mappings
    ** --------------------------------------------------------------- */
    FOSSIL_THREADS_EPERM         = 1,    /* operation not permitted / wrong usage */
    FOSSIL_THREADS_ENOMEM        = 12,   /* memory allocation failed */
    FOSSIL_THREADS_EBUSY         = 16,   /* resource busy / wrong state */
    FOSSIL_THREADS_EINVAL        = 22,   /* invalid argument */
    FOSSIL_THREADS_ENOSYS        = 38,   /* function not implemented */
    FOSSIL_THREADS_ETIMEDOUT     = 110,  /* operation timed out */
    FOSSIL_THREADS_EDEADLK       = 35,   /* deadlock detected */
    FOSSIL_THREADS_EAGAIN        = 11,   /* resource temporarily unavailable */
    FOSSIL_THREADS_EINTR         = 4,    /* interrupted system call */
    FOSSIL_THREADS_EIO           = 5,    /* I/O or handle-related error */

    /* ---------------------------------------------------------------
    ** Thread lifecycle errors
    ** --------------------------------------------------------------- */
    FOSSIL_THREADS_ENOTSTARTED   = 201,  /* thread not started yet */
    FOSSIL_THREADS_EFINISHED     = 202,  /* thread already finished */
    FOSSIL_THREADS_EJOINED       = 203,  /* thread already joined */
    FOSSIL_THREADS_EDETACHED     = 204,  /* thread is detached (not joinable) */
    FOSSIL_THREADS_ECANCELLED    = 205,  /* thread was cancelled (cooperative) */
    FOSSIL_THREADS_ESTATE        = 206,  /* invalid thread state transition */

    /* ---------------------------------------------------------------
    ** Synchronization and signaling
    ** --------------------------------------------------------------- */
    FOSSIL_THREADS_ELOCK         = 210,  /* mutex lock failed */
    FOSSIL_THREADS_EUNLOCK       = 211,  /* mutex unlock failed */
    FOSSIL_THREADS_ECONDWAIT     = 212,  /* condition wait failed */
    FOSSIL_THREADS_ECONDSTATE    = 213,  /* invalid condition state */
    FOSSIL_THREADS_EBARRIER      = 214,  /* barrier wait/init failed */
    FOSSIL_THREADS_ETHREADID     = 215,  /* could not retrieve thread id */

    /* ---------------------------------------------------------------
    ** OS / internal errors
    ** --------------------------------------------------------------- */
    FOSSIL_THREADS_EINTERNAL     = 250,  /* generic internal failure */
    FOSSIL_THREADS_EOSFAIL       = 251,  /* underlying OS call failed */
    FOSSIL_THREADS_EUNSUPPORTED  = 252,  /* unsupported operation on platform */
    FOSSIL_THREADS_ESTATECORRUPT = 253,  /* corrupted thread object or memory */
    FOSSIL_THREADS_EPARTIAL      = 254  /* partial or best-effort implementation */
};

/* ---------- Thread Pool API ---------- */

/* Forward declaration for thread pool handle */
typedef struct fossil_threads_pool fossil_threads_pool_t;

/*
 * Create a thread pool.
 * @param num_threads Number of worker threads.
 * @return Pointer to thread pool, or NULL on failure.
 */
FOSSIL_THREADS_API fossil_threads_pool_t* fossil_threads_pool_create(
    size_t num_threads
);

/*
 * Destroy a thread pool.
 * @param pool Pointer to thread pool.
 */
FOSSIL_THREADS_API void fossil_threads_pool_destroy(
    fossil_threads_pool_t *pool
);

/*
 * Submit a task to the thread pool.
 * @param pool Pointer to thread pool.
 * @param func Task function (same signature as thread entry).
 * @param arg Argument to pass to the task function.
 * @return 0 on success, error code otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_pool_submit(
    fossil_threads_pool_t *pool,
    fossil_threads_thread_func func,
    void *arg
);

/*
 * Wait for all tasks in the pool to finish.
 * @param pool Pointer to thread pool.
 * @return 0 on success, error code otherwise.
 */
FOSSIL_THREADS_API int fossil_threads_pool_wait(
    fossil_threads_pool_t *pool
);

/*
 * Get number of threads in the pool.
 * @param pool Pointer to thread pool.
 * @return Number of threads.
 */
FOSSIL_THREADS_API size_t fossil_threads_pool_size(
    const fossil_threads_pool_t *pool
);

#ifdef __cplusplus
}
#include <stdexcept>
#include <utility>

namespace fossil {

    namespace threads {

        /**
         * @brief C++ wrapper for fossil_threads_thread_t.
         *
         * Provides a RAII-style interface for thread management using the Fossil Logic thread API.
         * Disallows copy semantics; supports move semantics.
         */
        class Thread {
        public:
            /**
             * @brief Thread entry point function type.
             * Signature matches fossil_threads_thread_func.
             */
            using Func = void*(*)(void*);

            /**
             * @brief Default constructor.
             * Initializes the thread structure to a safe state; does not start a thread.
             */
            Thread() {
                fossil_threads_thread_init(&native_);
            }

            /**
             * @brief Construct and start a thread.
             * Initializes and creates a new thread executing the given function.
             * @param func Function pointer to thread entry.
             * @param arg Argument to pass to thread function (default nullptr).
             * @throws std::runtime_error on failure.
             */
            explicit Thread(Func func, void* arg = nullptr) {
                fossil_threads_thread_init(&native_);
                if (fossil_threads_thread_create(&native_, func, arg) != 0) {
                    throw std::runtime_error("Failed to create thread");
                }
            }

            /**
             * @brief Destructor.
             * Disposes of the thread structure; does not join or detach running threads.
             */
            ~Thread() {
                fossil_threads_thread_dispose(&native_);
            }

            /**
             * @brief Deleted copy constructor.
             * Threads cannot be copied.
             */
            Thread(const Thread&) = delete;

            /**
             * @brief Deleted copy assignment operator.
             * Threads cannot be copied.
             */
            Thread& operator=(const Thread&) = delete;

            /**
             * @brief Move constructor.
             * Transfers thread ownership; source is reset to default state.
             * @param other Thread to move from.
             */
            Thread(Thread&& other) noexcept {
                native_ = other.native_;
                other.native_ = {};
            }

            /**
             * @brief Move assignment operator.
             * Disposes current thread and transfers ownership from other.
             * @param other Thread to move from.
             * @return Reference to this thread.
             */
            Thread& operator=(Thread&& other) noexcept {
                if (this != &other) {
                    fossil_threads_thread_dispose(&native_);
                    native_ = other.native_;
                    other.native_ = {};
                }
                return *this;
            }

            /**
             * @brief Join the thread.
             * Waits for thread completion and optionally retrieves return value.
             * @param retval Pointer to receive thread return value (may be nullptr).
             * @return 0 on success, error code otherwise.
             */
            int join(void** retval = nullptr) {
                return fossil_threads_thread_join(&native_, retval);
            }

            /**
             * @brief Detach the thread.
             * Marks thread as detached; resources released when finished.
             * @return 0 on success, error code otherwise.
             */
            int detach() {
                return fossil_threads_thread_detach(&native_);
            }

            /**
             * @brief Get thread ID.
             * Returns OS-specific thread identifier.
             * @return Thread ID.
             */
            unsigned long id() const {
                return native_.id;
            }

            /**
             * @brief Check if thread is joinable.
             * @return true if joinable, false otherwise.
             */
            bool joinable() const {
                return native_.joinable != 0;
            }

            /**
             * @brief Yield processor to another thread.
             * Static utility function.
             */
            static void yield() {
                fossil_threads_thread_yield();
            }

            /**
             * @brief Sleep for specified milliseconds.
             * Static utility function.
             * @param ms Number of milliseconds to sleep.
             */
            static void sleep_ms(unsigned int ms) {
                fossil_threads_thread_sleep_ms(ms);
            }

            /**
             * @brief Get current thread ID.
             * Static utility function.
             * @return Current thread ID.
             */
            static unsigned long current_id() {
                return fossil_threads_thread_id();
            }

            /**
             * @brief Compare two threads for equality.
             * Static utility function.
             * @param t1 First thread.
             * @param t2 Second thread.
             * @return true if equal, false otherwise.
             */
            static bool equal(const Thread& t1, const Thread& t2) {
                return fossil_threads_thread_equal(&t1.native_, &t2.native_) != 0;
            }

            /**
             * @brief Get native thread handle (mutable).
             * @return Pointer to native thread structure.
             */
            fossil_threads_thread_t* native_handle() { return &native_; }

            /**
             * @brief Get native thread handle (const).
             * @return Pointer to native thread structure.
             */
            const fossil_threads_thread_t* native_handle() const { return &native_; }

            // Extended API

            /**
             * @brief Set thread priority.
             * @param priority Priority value (OS-dependent).
             * @return 0 on success, error code otherwise.
             */
            int set_priority(int priority) {
                return fossil_threads_thread_set_priority(&native_, priority);
            }

            /**
             * @brief Get thread priority.
             * @return Priority value, or negative error code.
             */
            int get_priority() const {
                return fossil_threads_thread_get_priority(&native_);
            }

            /**
             * @brief Request thread cancellation (cooperative).
             * @return 0 on success, error code otherwise.
             */
            int cancel() {
                return fossil_threads_thread_cancel(&native_);
            }

            /**
             * @brief Check if thread is running.
             * @return true if running, false otherwise.
             */
            bool is_running() const {
                return fossil_threads_thread_is_running(&native_) != 0;
            }

            /**
             * @brief Get thread return value (if finished).
             * @return Return value pointer, or nullptr if not finished.
             */
            void* get_retval() const {
                return fossil_threads_thread_get_retval(&native_);
            }

        private:
            /**
             * @brief Native thread structure.
             * Holds OS-specific thread handle and state.
             */
            fossil_threads_thread_t native_{};
        };

    } // namespace threads

} // namespace fossil

#endif

#endif /* FOSSIL_THREADS_THREAD_H */
