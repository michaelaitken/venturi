<div align="center">

# Venturi

### A C++20 & Boost.Asio Networking Sandbox

![Status](https://img.shields.io/badge/Status-WIP-orange?style=for-the-badge)
![Language](https://img.shields.io/badge/C++-20-blue?style=for-the-badge&logo=c%2B%2B)

*An engineering sandbox exploring asynchronous network I/O, modern C++ memory models, and custom HTTP server implementation via a LAN-first media server.*

</div>

## Context

The goal of Venturi is to provide a dedicated environment to deepen my understanding of low-level systems programming using modern C++. Building a multi-threaded HTTP server using `Boost.Asio/Beast` forces a hands-on approach to concurrency, TCP socket management, parsing network headers, and handling modern C++ lifetimes in asynchronous callbacks.

> **Status:** This project is a **Work in Progress / Educational Sandbox**. It is not polished for production use. The core streaming pipeline works, but expect rough edges and architectural refactors as I use this to test and explore new C++ paradigms.
>
> *Update (Feb 2026): Active development is currently paused as I prepare for my relocation to Austin, TX in April. I will resume explorations once settled.*

## Technical Constraints & Architecture

To focus on the networking and backend architecture, I established strict constraints for this project:

### 1. The "Boring File" Strategy

Rather than dealing with on-the-fly transcoding (which masks networking performance bottlenecks behind CPU/GPU bottlenecks), Venturi aims to make the library "boring." A background job system would propose actions to normalize media into streaming-friendly formats. As a result, the hot path remains incredibly lean, allowing me to focus strictly on "Disk to Socket" throughput.

### 2. LAN-First/Only

By stripping away WAN-specific overhead and assuming high network performance, I can focus entirely on local performance metrics like Time-to-First-Frame (TTFF) and raw I/O efficiency, without worrying about protocol encryption.

### 3. Native C++ & Asynchronous I/O

Built entirely on **C++20** and **Boost.Asio/Beast**. The HTTP stack is fully asynchronous, providing me with a playground to explore and manage thread pools, `io_context` lifecycles, and non-blocking I/O.

## What I've Learnt & Current Status

### Implemented So Far

- [x] **Async HTTP Server:** Non-blocking I/O and session management using Boost.Beast.
- [x] **Filesystem Scanning:** Recursive directory traversal and basic container identification.
- [x] **Direct Play:** Basic streaming for compatible MP4/MKV containers.

### Active Development & Known Limitations

- **Boost Beast Byte-Range Seeking:** The current `Boost.Beast` file body implementation sends data to EOF, ignoring the end of range requests. I am currently looking into solutions, like a custom file body to properly respect precise HTTP `Range` headers, which is needed for video seeking and scrubbing over the network.

- **C++20 Polymorphism:** I am actively refactoring the architecture to move away from runtime polymorphism (virtual functions/v-tables) toward static polymorphism (C++20 Concepts and Templates) to reduce runtime overhead on the hot path.

### Future Explorations

- [ ] **FFmpeg Integration:** Building a background job scheduler for media analysis and pre-normalization.

## 🛠️ Build & Run

### Prerequisites

- GCC 11+ (C++20 support)
- CMake 3.22+

### Quick Start

1. **Clone Repo:**

    ```bash
    git clone https://github.com/michaelaitken/venturi.git
    cd venturi
    ```

2. **Prepare Media:**
    Create a folder named `media` in the root directory.

    ```bash
    mkdir media
    # Drop a test video file here
    ```

3. **Run:**

    ```bash
    make run
    ```

    *Starts server on port 8080.*

---
