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
#ifndef FOSSIL_THREADS_POOL_H
#define FOSSIL_THREADS_POOL_H

#include "threads.h"
#include "sync.h"

typedef struct task_queue_t {
    void *(*task_func)(void *);
    void *arg;
    struct task_queue_t *next;
} task_queue_t;

/* Task-based Concurrency (Thread Pool) */
typedef struct fossil_thread_pool_t {
    fossil_thread_t *threads;
    uint32_t num_threads;
    fossil_mutex_t mutex;
    fossil_cond_t cond;
    fossil_semaphore_t semaphore;
    task_queue_t *head;
    task_queue_t *tail;
    int32_t shutdown;
} fossil_thread_pool_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes a thread pool with the specified number of threads.
 *
 * @param pool Pointer to the thread pool.
 * @param num_threads Number of threads in the pool.
 * @return int32_t 0 if successful, -1 otherwise.
 */
int32_t fossil_thread_pool_create(fossil_thread_pool_t *pool, uint32_t num_threads);

/**
 * @brief Submits a task to the thread pool.
 *
 * @param pool Pointer to the thread pool.
 * @param task Pointer to the task function to be executed.
 * @param arg Argument to pass to the task function.
 * @return int32_t 0 if the task is successfully submitted, -1 otherwise.
 */
int32_t fossil_thread_pool_submit(fossil_thread_pool_t *pool, fossil_task_t task, fossil_argumet_t arg);

/**
 * @brief Destroys the thread pool and reclaims its resources.
 *
 * @param pool Pointer to the thread pool.
 * @return int32_t 0 if successful, -1 otherwise.
 */
int32_t fossil_thread_pool_destroy(fossil_thread_pool_t *pool);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_THREADS_FRAMEWORK_H */
