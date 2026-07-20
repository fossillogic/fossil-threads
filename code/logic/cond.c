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
#define _POSIX_C_SOURCE 200809L
#include "fossil/threads/cond.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <pthread.h>
#  include <errno.h>
#  include <time.h>
#endif

static void fossil__cond_zero(fossil_threads_cond_t *c) {
    if (c) memset(c, 0, sizeof(*c));
}

/* ---------- Lifecycle ---------- */

int fossil_threads_cond_init(fossil_threads_cond_t *c) {
    if (!c) return FOSSIL_THREADS_COND_EINVAL;
    fossil__cond_zero(c);

#if defined(_WIN32)
    CONDITION_VARIABLE *cv = (CONDITION_VARIABLE*)malloc(sizeof(CONDITION_VARIABLE));
    if (!cv) return FOSSIL_THREADS_COND_ENOMEM;
    InitializeConditionVariable(cv);
    c->handle = cv;
#else
    pthread_cond_t *pc = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    if (!pc) return FOSSIL_THREADS_COND_ENOMEM;
    if (pthread_cond_init(pc, NULL) != 0) {
        free(pc);
        return FOSSIL_THREADS_COND_EINTERNAL;
    }
    c->handle = pc;
#endif
    c->valid = 1;
    c->is_broadcast = 0;
    c->waiters = 0;
    return FOSSIL_THREADS_COND_OK;
}

void fossil_threads_cond_dispose(fossil_threads_cond_t *c) {
    if (!c || !c->valid) return;

#if defined(_WIN32)
    CONDITION_VARIABLE *cv = (CONDITION_VARIABLE*)c->handle;
    free(cv); /* Windows condition vars don’t need explicit destruction */
#else
    pthread_cond_t *pc = (pthread_cond_t*)c->handle;
    if (pc) {
        pthread_cond_destroy(pc);
        free(pc);
    }
#endif
    fossil__cond_zero(c);
}

/* ---------- Wait / signal ---------- */

int fossil_threads_cond_wait(fossil_threads_cond_t *c, fossil_threads_mutex_t *m) {
    if (!c || !m || !c->valid || !m->valid) return FOSSIL_THREADS_COND_EINVAL;

    c->waiters++;
#if defined(_WIN32)
    if (!SleepConditionVariableCS(
        (CONDITION_VARIABLE*)c->handle,
        (CRITICAL_SECTION*)m->handle,
        INFINITE))
    {
        c->waiters--;
        return FOSSIL_THREADS_COND_EINTERNAL;
    }
    c->waiters--;
    return FOSSIL_THREADS_COND_OK;
#else
    int rc = pthread_cond_wait((pthread_cond_t*)c->handle,
                               (pthread_mutex_t*)m->handle);
    c->waiters--;
    if (rc == 0) return FOSSIL_THREADS_COND_OK;
    if (rc == EINVAL) return FOSSIL_THREADS_COND_EINVAL;
    if (rc == EAGAIN) return FOSSIL_THREADS_COND_EAGAIN;
    if (rc == ENOMEM) return FOSSIL_THREADS_COND_ENOMEM;
    if (rc == EPERM)  return FOSSIL_THREADS_COND_EPERM;
    if (rc == EBUSY)  return FOSSIL_THREADS_COND_EBUSY;
    if (rc == EDEADLK) return FOSSIL_THREADS_COND_EDEADLK;
    if (rc == ENOENT) return FOSSIL_THREADS_COND_ENOENT;
    if (rc == ENOSYS) return FOSSIL_THREADS_COND_ENOSYS;
    return FOSSIL_THREADS_COND_EINTERNAL;
#endif
}

int fossil_threads_cond_timedwait(fossil_threads_cond_t *c,
                                  fossil_threads_mutex_t *m,
                                  unsigned int ms) {
    if (!c || !m || !c->valid || !m->valid) return FOSSIL_THREADS_COND_EINVAL;

    c->waiters++;
#if defined(_WIN32)
    BOOL ok = SleepConditionVariableCS(
        (CONDITION_VARIABLE*)c->handle,
        (CRITICAL_SECTION*)m->handle,
        ms
    );
    c->waiters--;
    if (ok) return FOSSIL_THREADS_COND_OK;
    if (GetLastError() == ERROR_TIMEOUT) return FOSSIL_THREADS_COND_ETIMEDOUT;
    return FOSSIL_THREADS_COND_EINTERNAL;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    ts.tv_sec += ms / 1000u;
    ts.tv_nsec += (long)(ms % 1000u) * 1000000L;
    if (ts.tv_nsec >= 1000000000L) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000L;
    }

    int rc = pthread_cond_timedwait((pthread_cond_t*)c->handle,
                                    (pthread_mutex_t*)m->handle,
                                    &ts);
    c->waiters--;
    if (rc == 0) return FOSSIL_THREADS_COND_OK;
    if (rc == ETIMEDOUT) return FOSSIL_THREADS_COND_ETIMEDOUT;
    if (rc == EINVAL) return FOSSIL_THREADS_COND_EINVAL;
    if (rc == EAGAIN) return FOSSIL_THREADS_COND_EAGAIN;
    if (rc == ENOMEM) return FOSSIL_THREADS_COND_ENOMEM;
    if (rc == EPERM)  return FOSSIL_THREADS_COND_EPERM;
    if (rc == EBUSY)  return FOSSIL_THREADS_COND_EBUSY;
    if (rc == EDEADLK) return FOSSIL_THREADS_COND_EDEADLK;
    if (rc == ENOENT) return FOSSIL_THREADS_COND_ENOENT;
    if (rc == ENOSYS) return FOSSIL_THREADS_COND_ENOSYS;
    return FOSSIL_THREADS_COND_EINTERNAL;
#endif
}

int fossil_threads_cond_signal(fossil_threads_cond_t *c) {
    if (!c || !c->valid) return FOSSIL_THREADS_COND_EINVAL;

    c->is_broadcast = 0;
#if defined(_WIN32)
    WakeConditionVariable((CONDITION_VARIABLE*)c->handle);
    return FOSSIL_THREADS_COND_OK;
#else
    int rc = pthread_cond_signal((pthread_cond_t*)c->handle);
    if (rc == 0) return FOSSIL_THREADS_COND_OK;
    if (rc == EINVAL) return FOSSIL_THREADS_COND_EINVAL;
    if (rc == ENOMEM) return FOSSIL_THREADS_COND_ENOMEM;
    if (rc == EBUSY) return FOSSIL_THREADS_COND_EBUSY;
    if (rc == EPERM) return FOSSIL_THREADS_COND_EPERM;
    if (rc == EDEADLK) return FOSSIL_THREADS_COND_EDEADLK;
    if (rc == ENOENT) return FOSSIL_THREADS_COND_ENOENT;
    if (rc == EAGAIN) return FOSSIL_THREADS_COND_EAGAIN;
    if (rc == ENOSYS) return FOSSIL_THREADS_COND_ENOSYS;
    return FOSSIL_THREADS_COND_EINTERNAL;
#endif
}

int fossil_threads_cond_broadcast(fossil_threads_cond_t *c) {
    if (!c || !c->valid) return FOSSIL_THREADS_COND_EINVAL;

    c->is_broadcast = 1;
#if defined(_WIN32)
    WakeAllConditionVariable((CONDITION_VARIABLE*)c->handle);
    return FOSSIL_THREADS_COND_OK;
#else
    int rc = pthread_cond_broadcast((pthread_cond_t*)c->handle);
    if (rc == 0) return FOSSIL_THREADS_COND_OK;
    if (rc == EINVAL) return FOSSIL_THREADS_COND_EINVAL;
    if (rc == ENOMEM) return FOSSIL_THREADS_COND_ENOMEM;
    if (rc == EBUSY) return FOSSIL_THREADS_COND_EBUSY;
    if (rc == EPERM) return FOSSIL_THREADS_COND_EPERM;
    if (rc == EDEADLK) return FOSSIL_THREADS_COND_EDEADLK;
    if (rc == ENOENT) return FOSSIL_THREADS_COND_ENOENT;
    if (rc == EAGAIN) return FOSSIL_THREADS_COND_EAGAIN;
    if (rc == ENOSYS) return FOSSIL_THREADS_COND_ENOSYS;
    return FOSSIL_THREADS_COND_EINTERNAL;
#endif
}

int fossil_threads_cond_is_valid(const fossil_threads_cond_t *c) {
    if (!c) return 0;
    return c->valid ? 1 : 0;
}

int fossil_threads_cond_waiter_count(const fossil_threads_cond_t *c) {
    if (!c || !c->valid) return -1;
    return c->waiters;
}

int fossil_threads_cond_reset(fossil_threads_cond_t *c) {
    if (!c) return FOSSIL_THREADS_COND_EINVAL;
    fossil_threads_cond_dispose(c);
    return fossil_threads_cond_init(c);
}
