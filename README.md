# **Fossil Threads by Fossil Logic**

**Fossil Threads** is a lightweight, portable multithreading library written in pure C with no external dependencies. Designed for cross-platform applications, embedded systems, and performance-critical software, Fossil Threads provides simple, efficient abstractions for thread creation, synchronization, and management while maintaining minimal footprint and maximum portability.

### Key Features

- **Cross-Platform Support**  
  Works reliably on Windows, macOS, Linux, and embedded platforms.

- **Zero External Dependencies**  
  Written entirely in C for maximum portability, auditability, and ease of integration.

- **Thread Abstraction**  
  Simple APIs for creating, joining, and managing threads across platforms.

- **Synchronization Primitives**  
  Includes mutexes, condition variables, semaphores, and lightweight locks for safe concurrent programming.

- **Lightweight and Efficient**  
  Optimized for minimal resource usage while maintaining high performance.

- **Modular Design**  
  Easily extended or integrated into existing projects without imposing heavy dependencies.

## Getting Started

## ***Prerequisites***

To get started, ensure you have the following installed:

- **Meson Build System**: If you don’t have Meson `1.10.0` or newer installed, follow the installation instructions on the official [Meson website](https://mesonbuild.com/Getting-meson.html).

### Adding Dependency

#### Adding via Meson Git Wrap

To add a git-wrap, place a `.wrap` file in `subprojects` with the Git repo URL and revision, then use `dependency('fossil-threads')` in `meson.build` so Meson can fetch and build it automatically.

#### Integrate the Dependency:

Add the `fossil-threads.wrap` file in your `subprojects` directory and include the following content:

```ini
[wrap-git]
url = https://github.com/fossillogic/fossil-threads.git
revision = v1.0.0

[provide]
dependency_names = fossil-threads
```

**Note**: For the best experience, always use the latest releases. Visit the [releases](https://github.com/fossillogic/fossil-threads/releases) page for the latest versions.

## Build Configuration Options

Customize your build with the following Meson options:
	•	Enable Tests
To run the built-in test suite, configure Meson with:

```sh
meson setup builddir -Dwith_test=enabled
```

### Tests Double as Samples

The project is designed so that **test cases serve two purposes**:

- ✅ **Unit Tests** – validate the framework’s correctness.  
- 📖 **Usage Samples** – demonstrate how to use these libraries through test cases.  

This approach keeps the codebase compact and avoids redundant “hello world” style examples.  
Instead, the same code that proves correctness also teaches usage.  

This mirrors the **Meson build system** itself, which tests its own functionality by using Meson to test Meson.  
In the same way, Fossil Logic validates itself by demonstrating real-world usage in its own tests via Fossil Test.  

```bash
meson test -C builddir -v
```

Running the test suite gives you both verification and practical examples you can learn from.

## Contributing and Support

For those interested in contributing, reporting issues, or seeking support, please open an issue on the project repository or visit the [Fossil Logic Docs](https://fossillogic.com/docs) for more information. Your feedback and contributions are always welcome.