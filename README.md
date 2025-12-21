<div align="center">

# Venturi

### High-Performance, LAN-First Media Server

![Status](https://img.shields.io/badge/Status-Active_Development-orange?style=for-the-badge)
![Language](https://img.shields.io/badge/C++-20-blue?style=for-the-badge&logo=c%2B%2B)

*An opinionated, C++20 media server experiment exploring the limits of direct-play performance and library normalization.*

</div>

## 📖 The Core Concept

Existing media servers (Plex, Jellyfin) are Swiss Army Knives. They try to support every client, network condition, and media type, often relying on aggressive on-the-fly transcoding.

**Venturi is different.** It's built on three specific principles:

1. **Direct Play is the Norm:** 99% of playback should be "Disk → Socket" with zero transcoding.
2. **Normalize Up Front:** Instead of burning CPU to transcode during playback, Venturi analyzes and, if requested, optimizes media *before* you press play.
3. **Tight Scope:** Its focused purely on Movies/TV over LAN. No photos, no books, no WAN complexity.

> **⚠️ Status:** This project is currently in **Early Active Development**. The core streaming pipeline works, but expect rough edges and architectural refactors.

## ⚡ Technical Philosophy

### 1. The "Boring File" Strategy

Venturi aims to make your library "boring." A background job system analyzes incoming media for exotic codecs or high bitrates. It then proposes actions (Optimize or Replace) to convert files into standard, streaming-friendly formats.

**Result:** The hot path remains incredibly lean, reducing runtime CPU/GPU load.

### 2. LAN-First/Only

Venturi is designed for the modern home network. By prioritizing LAN streaming, we can strip away WAN-specific overhead. This allows us to focus on local performance metrics like **Time-to-First-Frame (TTFF)** and assume high network performance.

### 3. Native C++ Architecture

Built on **C++20** and **Boost.Asio/Beast**, the HTTP stack is fully asynchronous. The goal is a pipeline where server overhead is negligible compared to the media throughput.

## 🗺️ Roadmap & Current Status

### Implemented Features

- [x] **Async HTTP Server:** Non-blocking I/O using Boost.Beast.
- [x] **Filesystem Scanning:** Recursive directory traversal and basic container identification.
- [x] **Direct Play:** Basic streaming for compatible MP4/MKV containers.

### Known Limitations (Active Engineering)

- **Byte-Range Seeking Bug:** The current `Boost.Beast` file body implementation eagerly sends data to EOF, disregarding precise range headers.
- **Refactor In-Progress:** Moving from runtime polymorphism (virtual functions) to static polymorphism (Concepts/Templates).

### Upcoming

- [ ] **FFmpeg Integration:** Background job scheduler for media analysis and normalization.
- [ ] **Dashboard:** Web UI for visualizing media library and performance metrics.


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

<div align="center">
  <i>Built with ❤️ and C++</i>
</div>