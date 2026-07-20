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
#include "fossil/threads/mutex.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <pthread.h>
#endif

static void fossil__mutex_zero(fossil_threads_mutex_t *m) {
    if (m) memset(m, 0, sizeof(*m));
}

/* ---------- Lifecycle ---------- */

int fossil_threads_mutex_init(fossil_threads_mutex_t *m) {
    if (!m) return FOSSIL_THREADS_MUTEX_EINVAL;
    fossil__mutex_zero(m);

#if defined(_WIN32)
    CRITICAL_SECTION *cs = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
    if (!cs) return FOSSIL_THREADS_MUTEX_ENOMEM;
    InitializeCriticalSection(cs);
    m->handle = cs;
#else
    pthread_mutex_t *pm = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (!pm) return FOSSIL_THREADS_MUTEX_ENOMEM;
    int rc = pthread_mutex_init(pm, NULL);
    if (rc != 0) {
        free(pm);
        if (rc == ENOMEM)
            return FOSSIL_THREADS_MUTEX_ENOMEM;
        else if (rc == EPERM)
            return FOSSIL_THREADS_MUTEX_EPERM;
        else
            return FOSSIL_THREADS_MUTEX_EINTERNAL;
    }
    m->handle = pm;
#endif

    m->valid = 1;
    m->locked = 0;
    m->recursive = 0;
    return FOSSIL_THREADS_MUTEX_OK;
}

void fossil_threads_mutex_dispose(fossil_threads_mutex_t *m) {
    if (!m || !m->valid) return;

#if defined(_WIN32)
    CRITICAL_SECTION *cs = (CRITICAL_SECTION*)m->handle;
    if (cs) {
        DeleteCriticalSection(cs);
        free(cs);
    }
#else
    pthread_mutex_t *pm = (pthread_mutex_t*)m->handle;
    if (pm) {
        int rc = pthread_mutex_destroy(pm);
        free(pm);
        (void)rc; // ignore destroy errors for now
    }
#endif
    fossil__mutex_zero(m);
}

/* ---------- Locking ---------- */

int fossil_threads_mutex_lock(fossil_threads_mutex_t *m) {
    if (!m || !m->valid) return FOSSIL_THREADS_MUTEX_EINVAL;

#if defined(_WIN32)
    EnterCriticalSection((CRITICAL_SECTION*)m->handle);
    m->locked = 1;
    return FOSSIL_THREADS_MUTEX_OK;
#else
    int rc = pthread_mutex_lock((pthread_mutex_t*)m->handle);
    if (rc == 0) {
        m->locked = 1;
        return FOSSIL_THREADS_MUTEX_OK;
    }
    if (rc == EINVAL)
        return FOSSIL_THREADS_MUTEX_EINVAL;
    if (rc == EDEADLK)
        return FOSSIL_THREADS_MUTEX_EDEADLK;
    if (rc == EPERM)
        return FOSSIL_THREADS_MUTEX_EPERM;
    return FOSSIL_THREADS_MUTEX_EINTERNAL;
#endif
}

int fossil_threads_mutex_unlock(fossil_threads_mutex_t *m) {
    if (!m || !m->valid) return FOSSIL_THREADS_MUTEX_EINVAL;

#if defined(_WIN32)
    LeaveCriticalSection((CRITICAL_SECTION*)m->handle);
    m->locked = 0;
    return FOSSIL_THREADS_MUTEX_OK;
#else
    int rc = pthread_mutex_unlock((pthread_mutex_t*)m->handle);
    if (rc == 0) {
        m->locked = 0;
        return FOSSIL_THREADS_MUTEX_OK;
    }
    if (rc == EINVAL)
        return FOSSIL_THREADS_MUTEX_EINVAL;
    if (rc == EPERM)
        return FOSSIL_THREADS_MUTEX_EUNLOCK;
    return FOSSIL_THREADS_MUTEX_EINTERNAL;
#endif
}

int fossil_threads_mutex_trylock(fossil_threads_mutex_t *m) {
    if (!m || !m->valid) return FOSSIL_THREADS_MUTEX_EINVAL;

#if defined(_WIN32)
    if (TryEnterCriticalSection((CRITICAL_SECTION*)m->handle)) {
        m->locked = 1;
        return FOSSIL_THREADS_MUTEX_OK;
    } else {
        return FOSSIL_THREADS_MUTEX_EBUSY;
    }
#else
    int rc = pthread_mutex_trylock((pthread_mutex_t*)m->handle);
    if (rc == 0) {
        m->locked = 1;
        return FOSSIL_THREADS_MUTEX_OK;
    }
    if (rc == EBUSY)
        return FOSSIL_THREADS_MUTEX_EBUSY;
    if (rc == EINVAL)
        return FOSSIL_THREADS_MUTEX_EINVAL;
    if (rc == EPERM)
        return FOSSIL_THREADS_MUTEX_EPERM;
    return FOSSIL_THREADS_MUTEX_EINTERNAL;
#endif
}

bool fossil_threads_mutex_is_locked(const fossil_threads_mutex_t *m) {
    if (!m || !m->valid) return false;
    return m->locked ? true : false;
}

bool fossil_threads_mutex_is_initialized(const fossil_threads_mutex_t *m) {
    if (!m) return false;
    return m->valid ? true : false;
}

void fossil_threads_mutex_reset(fossil_threads_mutex_t *m) {
    if (!m) return;
    fossil_threads_mutex_dispose(m);
    fossil__mutex_zero(m);
}
