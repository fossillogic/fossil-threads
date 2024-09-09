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
#include "fossil/threads/pool.h"
#include <stdlib.h>

/* Task-based Concurrency (Thread Pool) */

static void *worker_thread(void *arg) {
    fossil_thread_pool_t *pool = (fossil_thread_pool_t *)arg;

    while (1) {
        fossil_mutex_lock(&pool->mutex);

        while (pool->head == NULL && !pool->shutdown) {
            fossil_cond_wait(&pool->cond, &pool->mutex);
        }

        if (pool->shutdown) {
            fossil_mutex_unlock(&pool->mutex);
            break;
        }

        task_queue_t *task = pool->head;
        pool->head = task->next;
        if (pool->head == NULL) {
            pool->tail = NULL;
        }

        fossil_mutex_unlock(&pool->mutex);

        task->task_func(task->arg);
        free(task);
    }

    return NULL;
}

int32_t fossil_thread_pool_create(fossil_thread_pool_t *pool, uint32_t num_threads) {
    pool->threads = (fossil_thread_t *)malloc(num_threads * sizeof(fossil_thread_t));
    if (!pool->threads) return -1;

    pool->head = NULL;
    pool->tail = NULL;
    pool->shutdown = 0;

    if (fossil_mutex_create(&pool->mutex) != 0 ||
        fossil_cond_create(&pool->cond) != 0 ||
        fossil_semaphore_create(&pool->semaphore, 0) != 0) {
        free(pool->threads);
        return -1;
    }

    for (uint32_t i = 0; i < num_threads; i++) {
        if (fossil_thread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            fossil_thread_pool_destroy(pool);
            return -1;
        }
    }
    return 0;
}

int32_t fossil_thread_pool_submit(fossil_thread_pool_t *pool, void *(*task)(void *), void *arg) {
    task_queue_t *new_task = (task_queue_t *)malloc(sizeof(task_queue_t));
    if (!new_task) return -1;

    new_task->task_func = task;
    new_task->arg = arg;
    new_task->next = NULL;

    fossil_mutex_lock(&pool->mutex);

    if (pool->tail) {
        pool->tail->next = new_task;
    } else {
        pool->head = new_task;
    }
    pool->tail = new_task;

    fossil_cond_signal(&pool->cond);
    fossil_mutex_unlock(&pool->mutex);

    return 0;
}

int32_t fossil_thread_pool_destroy(fossil_thread_pool_t *pool) {
    fossil_mutex_lock(&pool->mutex);
    pool->shutdown = 1;
    fossil_cond_broadcast(&pool->cond);
    fossil_mutex_unlock(&pool->mutex);

    for (uint32_t i = 0; i < pool->num_threads; i++) {
        fossil_thread_join(pool->threads[i], NULL);
    }

    // Free remaining tasks in the queue
    task_queue_t *task = pool->head;
    while (task) {
        task_queue_t *next = task->next;
        free(task);
        task = next;
    }

    fossil_mutex_destroy(&pool->mutex);
    fossil_cond_destroy(&pool->cond);
    fossil_semaphore_destroy(&pool->semaphore);
    free(pool->threads);

    return 0;
}
