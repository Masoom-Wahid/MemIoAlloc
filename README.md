# Custom Memory Allocator (`mem_io`)

## Overview

`mem_io` is a custom memory allocator implemented in C, providing functionality similar to the standard `malloc`, `free`, and `realloc` functions. It is designed for learning purposes for myself to understand how memory management works at a low level in C programming. The allocator manages its own heap space and includes features like splitting and merging memory blocks to reduce fragmentation, thread safety with mutex locks, and debugging tools to trace memory usage.

## Features

- **Custom Memory Management**: Manages memory allocation and deallocation without relying on the standard library's allocator.
- **Thread Safety**: Uses `pthread_mutex_t` locks to ensure thread-safe operations.
- **Memory Optimization**: Implements splitting and merging of memory blocks to minimize fragmentation.
- **Large Allocation Handling**: Uses `mmap` for large memory allocations exceeding a predefined threshold.
- **Debugging Tools**: Provides functions to log internal states and trace memory blocks for debugging purposes.

## Files

- `memory.h`: Header file containing type definitions, macro definitions, and function declarations.
- `memory.c`: Source file containing the implementation of the memory allocator functions.

## Getting Started

### Prerequisites

- A C compiler (e.g., `gcc`)
- POSIX-compliant operating system (for `sbrk`, `mmap`, and `pthread` functionalities)
- `pthread` library (for thread safety)

### Installation

No special installation is required. Simply include the `memory.h` and `memory.c` files in your project directory.

### Compilation

When compiling your program, make sure to link the `pthread` library:

```bash
gcc -o my_program my_program.c memory.c -pthread
```

Replace `my_program.c` with the name of your C source file that utilizes the custom allocator.

## Usage

### Including the Allocator

In your C source files where you want to use the custom allocator, include the header:

```c
#include "memory.h"
```

### Allocating Memory

Use `mem_io_malloc` to allocate memory:

```c
void* ptr = mem_io_malloc(size_t size);
```

### Freeing Memory

Use `mem_io_free` to deallocate memory:

```c
mem_io_free(void* ptr);
```

### Reallocating Memory

Use `mem_io_realloc` to resize previously allocated memory:

```c
void* new_ptr = mem_io_realloc(void* ptr, size_t new_size);
```

### Debugging Functions

- **Log Internal State**: 

  ```c
  mem_io_log_trace();
  ```

  Prints the allocator's internal state, including heap boundaries and allocation counts.

- **Stack Trace**:

  ```c
  mem_io_stack_trace();
  ```

  Prints detailed information about each memory block managed by the allocator.

## Examples

### Basic Allocation and Deallocation

```c
#include <stdio.h>
#include "memory.h"

int main() {
    int* array = (int*) mem_io_malloc(10 * sizeof(int));
    if (array == NULL) {
        perror("Allocation failed");
        return EXIT_FAILURE;
    }

    // Use the allocated memory
    for (int i = 0; i < 10; i++) {
        array[i] = i * i;
    }

    // Free the allocated memory
    mem_io_free(array);

    return EXIT_SUCCESS;
}
```

### Reallocating Memory

```c
#include <stdio.h>
#include "memory.h"

int main() {
    char* buffer = (char*) mem_io_malloc(50);
    if (buffer == NULL) {
        perror("Allocation failed");
        return EXIT_FAILURE;
    }

    // Use the buffer
    snprintf(buffer, 50, "Hello, World!");

    // Resize the buffer
    buffer = (char*) mem_io_realloc(buffer, 100);
    if (buffer == NULL) {
        perror("Reallocation failed");
        return EXIT_FAILURE;
    }

    // Continue using the buffer
    strncat(buffer, " Additional text.", 100 - strlen(buffer) - 1);

    // Free the buffer
    mem_io_free(buffer);

    return EXIT_SUCCESS;
}
```

### Debugging Allocations

```c
#include <stdio.h>
#include "memory.h"

int main() {
    int* data = (int*) mem_io_malloc(5 * sizeof(int));

    // Simulate some operations
    data[0] = 42;

    // Log internal state
    mem_io_log_trace();

    // Stack trace
    mem_io_stack_trace();

    // Clean up
    mem_io_free(data);

    return EXIT_SUCCESS;
}
```

## Function Reference

### Allocation Functions

- **`void* mem_io_malloc(size_t size);`**

  Allocates `size` bytes of memory and returns a pointer to the allocated memory. The memory is not initialized.

- **`void mem_io_free(void* ptr);`**

  Frees the memory space pointed to by `ptr`, which must have been returned by a previous call to `mem_io_malloc` or `mem_io_realloc`.

- **`void* mem_io_realloc(void* ptr, size_t size);`**

  Changes the size of the memory block pointed to by `ptr` to `size` bytes. The contents will be unchanged in the range from the start of the region up to the minimum of the old and new sizes.

### Debugging Functions

- **`void mem_io_log_trace(void);`**

  Logs the internal state of the memory allocator, including heap start and end addresses, allocation counts, and error counts.

- **`void mem_io_stack_trace(void);`**

  Prints detailed information about each memory block, including its address, size, status flags, and magic number for validation.

## Internal Design

### Memory Blocks

- Each memory block is represented by the `mem_io_block` structure, which contains metadata about the block, such as size, pointers to next and previous blocks, status flags, and a magic number for validation.

### Heap Management

- **Heap Expansion**: Uses `sbrk` to expand the heap when more memory is needed.
- **Large Allocations**: Uses `mmap` for allocations larger than `MEM_IO_MMAP_THRESHOLD` (default is 128 KB).

### Free List Management

- Implements a free list to keep track of available memory blocks.
- Coalesces adjacent free blocks during deallocation to reduce fragmentation.

### Thread Safety

- All allocation and deallocation functions are protected by a mutex (`mem_lock`) to ensure thread safety.

### Flags and Magic Numbers

- **Flags**: Used to indicate the status of a memory block (free, used, corrupted, mmap).
- **Magic Number**: Each block contains a magic number (`MEM_IO_MAGIC_NUMBER`) for validation to detect memory corruption.

## Configuration

### Constants and Macros

- **`MEM_IO_PAGE`**: The size of memory increments when expanding the heap (default is 4096 bytes).
- **`MEM_IO_FLAG_*`**: Flags used to represent the status of memory blocks.
- **`MEM_IO_MAGIC_NUMBER`**: Magic number (`0xDEADBEEF`) used for block validation.
- **`MEM_IO_MMAP_THRESHOLD`**: Threshold for using `mmap` instead of `sbrk` (default is 128 KB).
- **`ALIGN(size)`**: Macro to align memory sizes to the nearest multiple of the system's word size.

## Limitations

- **Not Production-Ready**: This allocator is intended for educational purposes and may not handle all edge cases or be optimized for production use.
- **Error Handling**: Limited error handling; in some cases, the program may exit upon encountering an error.
- **Fragmentation**: While it implements basic strategies to reduce fragmentation, it may not be as efficient as standard allocators.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests to improve the allocator.

### Reporting Bugs

If you encounter any bugs or unexpected behavior, please open an issue with detailed information:

- Description of the problem
- Steps to reproduce
- Expected behavior
- Actual behavior
- Environment details (compiler version, OS, etc.)

### Suggestions

If you have ideas for enhancements or optimizations, please open an issue or submit a pull request with your proposed changes.

## Acknowledgments

- Inspired by low-level memory management concepts.
- Utilizes POSIX system calls (`sbrk`, `mmap`, `munmap`) for memory operations.
- Thread safety implemented using POSIX threads (`pthread`).

---

