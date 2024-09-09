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
#include "fossil/threads/fiber.h"
#include <stdlib.h>

/* -------- Fiber Implementation -------- */

#ifdef _WIN32
void fossil_fiber_delete(fossil_fiber_t fiber) {
    DeleteFiber(fiber);
}

fossil_fiber_t fossil_fiber_convert(void *arg) {
    return ConvertThreadToFiber(arg);
}
#else

void *fiber_start(void *arg) {
    fossil_fiber_t *fiber = (fossil_fiber_t *)arg;
    if (fiber->task) {
        fiber->task(fiber->arg);
    }
    free(fiber);  // Free the fiber structure after the task is complete
    return NULL;
}

fossil_fiber_t fossil_fiber_create(size_t stack_size, void *(*task)(void *), void *arg) {
    fossil_fiber_t *fiber = malloc(sizeof(fossil_fiber_t));
    if (!fiber) return NULL;

    fiber->task = task;
    fiber->arg = arg;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (stack_size > 0) {
        pthread_attr_setstacksize(&attr, stack_size);
    }

    // Create a POSIX thread instead of a fiber
    if (pthread_create(&fiber->thread, &attr, fiber_start, fiber) != 0) {
        free(fiber);
        return (fossil_fiber_t){0};  // Return an invalid fiber value
    }

    pthread_attr_destroy(&attr);  // Clean up the thread attributes object
    return *fiber;
}

void fossil_fiber_delete(fossil_fiber_t fiber) {
    pthread_join(fiber.thread, NULL);  // Wait for the thread to finish
}

fossil_fiber_t fossil_fiber_convert(void *arg) {
    return (fossil_fiber_t){0};  // Not available in POSIX
}
#endif
