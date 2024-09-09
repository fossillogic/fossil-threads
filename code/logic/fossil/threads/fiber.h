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
#ifndef FOSSIL_THREADS_FIBER_H
#define FOSSIL_THREADS_FIBER_H

#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
typedef LPVOID fossil_fiber_t;
#else
#define _XOPEN_SOURCE 700  // Ensure this is defined to use ucontext
#include <ucontext.h>
typedef ucontext_t* fossil_fiber_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a new fiber and returns its identifier.
 *
 * @param stack_size Size of the fiber's stack in bytes.
 * @param task Pointer to the task function for the fiber.
 * @param arg Argument to pass to the task function.
 * @return fossil_fiber_t Identifier for the created fiber.
 */
fossil_fiber_t fossil_fiber_create(size_t stack_size, void (*task)(void *), void *arg);

/**
 * @brief Switches execution to the specified fiber.
 *
 * @param fiber The fiber to switch to.
 */
void fossil_fiber_switch(fossil_fiber_t fiber);

/**
 * @brief Deletes the specified fiber and releases its resources.
 *
 * @param fiber The fiber to delete.
 */
void fossil_fiber_delete(fossil_fiber_t fiber);

/**
 * @brief Converts the calling thread into a fiber, allowing it to later switch back and forth between fibers.
 *
 * @param arg Argument to pass to the fiber's task.
 * @return fossil_fiber_t Identifier for the current fiber.
 */
fossil_fiber_t fossil_fiber_convert(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_THREADS_FRAMEWORK_H */
