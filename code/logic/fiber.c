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

/* -------- Fiber Implementation -------- */

fossil_fiber_t fossil_fiber_create(size_t stack_size, void (*task)(void *), void *arg) {
#ifdef _WIN32
    return CreateFiber(stack_size, (LPFIBER_START_ROUTINE)task, arg);
#else
    ucontext_t *fiber = (ucontext_t *)malloc(sizeof(ucontext_t));
    if (!fiber) return NULL;
    getcontext(fiber);
    fiber->uc_stack.ss_sp = malloc(stack_size);
    fiber->uc_stack.ss_size = stack_size;
    makecontext(fiber, (void (*)(void))task, 1, arg);
    return fiber;
#endif
}

void fossil_fiber_switch(fossil_fiber_t fiber) {
#ifdef _WIN32
    SwitchToFiber(fiber);
#else
    ucontext_t *current_context = (ucontext_t *)malloc(sizeof(ucontext_t));
    getcontext(current_context);
    swapcontext(current_context, (ucontext_t *)fiber);
#endif
}

void fossil_fiber_delete(fossil_fiber_t fiber) {
#ifdef _WIN32
    DeleteFiber(fiber);
#else
    ucontext_t *f = (ucontext_t *)fiber;
    free(f->uc_stack.ss_sp);
    free(f);
#endif
}

fossil_fiber_t fossil_fiber_convert(void *arg) {
#ifdef _WIN32
    return ConvertThreadToFiber(arg);
#else
    return NULL; // Not available in POSIX
#endif
}
