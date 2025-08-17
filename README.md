# Quick C++ Lock Benchmarks

A minimal C++ project to compare different synchronization primitives:

- __spin-unsafe__: a DIY spinlock, and a bad one at that. It should crash!
- __spin__: an improved spinlock.
- __mutex__: standard `std::mutex`.
- __hybrid__: a hybrid lock (spin + mutex, a.k.a. adaptive mutex).

## Spin Unsafe

The idea for this repository was to demonstrate how bad the `spin-unsafe` implementation is. It's based on some original code I found in production.

### Why this is problematic?

- __Busy-waiting (spinlock)__: that `while (bLocked) {/* busy-wait */ }` will spin the CPU endlessly until `bLocked` becomes `false`.
    - Wastes CPU cycles.
    - If the "lock holder" is preempted, this can stall other threads completely.
- __No memory ordering guarantees__: `bLocked` is a plain `bool`, so even if one thread sets it to `false`, another thread might not see that change immediately due to compiler or CPU reordering.
- __Non-atomic flag__: Setting and reading `bLocked` isn’t thread-safe without `std::atomic`, `std::atomic_flag`, or proper synchronization.
- __Reentrancy hazard__: If the thread holding `bLocked` somehow calls code that re-enters this block, you’ve got a deadlock.

### The Alternatives

`mutex.cpp` and `hybrid.cpp` provide much better approaches:

- No busy-wait, CPU isn’t wasted.
- Correct memory ordering and visibility guaranteed.
- Exception-safe (lock is released automatically if an exception is thrown).
- No need to manually flip flags.

## Clone & Build

```bash
$ git clone https://github.com/NuLL3rr0r/quick-cpp-lock-benchmarks.git
$ cd quick-cpp-lock-benchmarks
$ make
```
