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

#ifdef _WIN32

static LPVOID main_fiber = NULL;
static fossil_fiber_t *current_fiber = NULL;

static void fiber_entry(void *arg) {
    fossil_fiber_t *fiber = (fossil_fiber_t *)arg;
    fiber->task(fiber->arg);
    fossil_fiber_delete((fossil_fiber_t)fiber);
    SwitchToFiber(main_fiber);
}

fossil_fiber_t fossil_fiber_create(size_t stack_size, void (*task)(void *), void *arg) {
    fossil_fiber_t *fiber = (fossil_fiber_t *)malloc(sizeof(fossil_fiber_t));
    if (!fiber) {
        perror("Failed to allocate memory for fiber");
        exit(EXIT_FAILURE);
    }

    fiber->task = task;
    fiber->arg = arg;
    fiber->stack_size = stack_size;

    fiber->fiber = CreateFiber(stack_size, fiber_entry, fiber);
    if (!fiber->fiber) {
        perror("Failed to create fiber");
        free(fiber);
        exit(EXIT_FAILURE);
    }

    return (fossil_fiber_t)fiber;
}

void fossil_fiber_switch(fossil_fiber_t fiber) {
    fossil_fiber_t *next_fiber = (fossil_fiber_t *)fiber;
    if (current_fiber) {
        SwitchToFiber(next_fiber->fiber);
    } else {
        current_fiber = next_fiber;
        main_fiber = ConvertThreadToFiber(NULL);
        SwitchToFiber(next_fiber->fiber);
    }
}

void fossil_fiber_delete(fossil_fiber_t fiber) {
    fossil_fiber_t *fiber_to_delete = (fossil_fiber_t *)fiber;
    if (fiber_to_delete) {
        DeleteFiber(fiber_to_delete->fiber);
        free(fiber_to_delete);
    }
}

fossil_fiber_t fossil_fiber_convert(void *arg) {
    fossil_fiber_t *fiber = (fossil_fiber_t *)malloc(sizeof(fossil_fiber_t));
    if (!fiber) {
        perror("Failed to allocate memory for fiber");
        exit(EXIT_FAILURE);
    }

    fiber->task = NULL;
    fiber->arg = arg;
    fiber->stack_size = 0;
    fiber->fiber = ConvertThreadToFiber(fiber);

    if (!fiber->fiber) {
        perror("Failed to convert thread to fiber");
        free(fiber);
        exit(EXIT_FAILURE);
    }

    return (fossil_fiber_t)fiber;
}

#else

static pthread_t main_thread;
static fossil_fiber_t *current_fiber = NULL;

static void *fiber_entry(void *arg) {
    fossil_fiber_t *fiber = (fossil_fiber_t *)arg;
    fiber->task(fiber->arg);
    fossil_fiber_delete((fossil_fiber_t)fiber);
    pthread_exit(NULL);
}

fossil_fiber_t fossil_fiber_create(size_t stack_size, void (*task)(void *), void *arg) {
    fossil_fiber_t *fiber = (fossil_fiber_t *)malloc(sizeof(fossil_fiber_t));
    if (!fiber) {
        perror("Failed to allocate memory for fiber");
        exit(EXIT_FAILURE);
    }
    
    fiber->task = task;
    fiber->arg = arg;
    fiber->stack_size = stack_size;

    pthread_attr_init(&fiber->attr);
    pthread_attr_setstacksize(&fiber->attr, stack_size);

    if (pthread_create(&fiber->thread, &fiber->attr, fiber_entry, fiber) != 0) {
        perror("Failed to create thread");
        free(fiber);
        exit(EXIT_FAILURE);
    }

    pthread_attr_destroy(&fiber->attr);
    return (fossil_fiber_t)fiber;
}

void fossil_fiber_switch(fossil_fiber_t fiber) {
    fossil_fiber_t *next_fiber = (fossil_fiber_t *)fiber;
    if (current_fiber) {
        pthread_join(current_fiber->thread, NULL);
    }
    current_fiber = next_fiber;
    pthread_join(next_fiber->thread, NULL);
}

void fossil_fiber_delete(fossil_fiber_t fiber) {
    fossil_fiber_t *fiber_to_delete = (fossil_fiber_t *)fiber;
    if (fiber_to_delete) {
        pthread_cancel(fiber_to_delete->thread);
        free(fiber_to_delete);
    }
}

fossil_fiber_t fossil_fiber_convert(void *arg) {
    return NULL; // Not available in POSIX
}

#endif
