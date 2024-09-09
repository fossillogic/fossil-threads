# **Threads Library by Fossil Logic**

Threads is a versatile, platform-independent library that provides a comprehensive suite of threading utilities and synchronization primitives. Designed to facilitate the development of high-performance, multi-threaded applications, Threads is implemented in both C and C++ and optimized for consistent performance across various operating systems. The library uses the Meson build system, ensuring an efficient development and deployment process.

## Key Features

1. **Cross-Platform Compatibility**: Offers uniform threading and synchronization capabilities across major operating systems, including Windows, macOS, and Linux.
2. **Modular Design**: Comprises modular components that can be easily integrated and customized to fit specific project requirements.
3. **Efficient Build System**: Uses the Meson build system, known for its fast build times and user-friendly syntax, for both C and C++ components.
4. **High Performance**: Built in C and C++ to ensure maximum performance and resource efficiency, suitable for both desktop and embedded systems.
5. **Extensive Documentation and Examples**: Provides comprehensive documentation and example code to help developers get started quickly and effectively.

## Threading Utilities

Threads includes a variety of threading utilities essential for building multi-threaded applications:

- **Thread Creation and Management**: Functions for creating, joining, detaching, and managing threads.
- **Thread Pooling**: Implements thread pools to manage and reuse a pool of worker threads.
- **Fiber Threads**: Supports fiber threads for lightweight cooperative multitasking.

## Synchronization Primitives

Threads provides a robust set of synchronization primitives to ensure safe and efficient multi-threaded programming:

- **Mutex**: Functions for initializing, locking, and unlocking mutexes, preventing race conditions in concurrent environments.
- **Condition Variables**: Includes `fossil_cond` for thread synchronization, allowing threads to wait for certain conditions to be met.
- **Semaphores**: Provides custom semaphores for signaling and controlling access to limited resources. This custom implementation replaces deprecated semaphore headers for broader compatibility.

## Algorithms and Utilities

Threads also includes a suite of algorithms and utility functions tailored for multi-threaded programming:

- **Task Scheduling**: Provides mechanisms for scheduling tasks in a multi-threaded environment.
- **Thread Affinity**: Functions to set and get the affinity of threads, optimizing CPU usage.
- **Error Handling**: Includes robust error handling mechanisms for threading operations.

## Prerequisites

Before using Threads, ensure you have the following installed:

- **Meson Build System**: This project requires Meson. If it's not already installed, you can follow the [installation instructions on the Meson website](https://mesonbuild.com/Getting-meson.html).

## Adding Dependency

1. **Install Meson Build System**: Ensure that Meson `1.3` or newer is installed on your system. You can install or upgrade it using pip:

   ```sh
   python -m pip install meson           # to install Meson
   python -m pip install --upgrade meson # to upgrade Meson
   ```

2. **Adding Wrap File**: Navigate to the `subprojects` directory and create a `.wrap` file, such as `fossil-threads.wrap`, with the following content:

   ```ini
   # ======================
   # Git Wrap package definition
   # ======================
   [wrap-git]
   url = https://github.com/fossillogic/fossil-threads.git
   revision = v0.1.0

   [provide]
   fossil-threads = fossil_threads_dep
   ```

3. **Integrate the New Dependency**: To integrate the dependency into your Meson project, add the following line to your `meson.build` file:

   ```ini
   dep = dependency('fossil-threads')
   ```

   This line imports the `fossil-threads` dependency, making it available for use in your project.

## Configure Options

Threads offers configurable options to tailor the build process to your needs:

- **Running Tests**: To enable testing, configure the build with `-Dwith_test=enabled`.

Example:

```sh
meson setup builddir -Dwith_test=enabled
```

## Contributing and Support

For those interested in contributing, reporting issues, or seeking support, please open an issue on the project repository or visit the [Fossil Logic Docs](https://fossillogic.com/docs) for more information. Your feedback and contributions are always welcome.
