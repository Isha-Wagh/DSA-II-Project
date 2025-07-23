# Smart Web Caching with URL Shortening and LRU Optimization

A C++ project that simulates a smart web caching mechanism with a built-in URL shortener, Least Recently Used (LRU) cache, robust error handling, and basic HTTP caching (ETag/Cache-Control).  
This project demonstrates Operating Systems, Web Systems, and Data Structures & Algorithms (DSA) concepts, with persistence and scalable, maintainable code architecture.

## Features

- **URL Shortener:** Converts long URLs to short, unique codes and vice versa.
- **LRU Cache:** Efficiently caches web content for recently visited URLs, evicting the least recently used when full.
- **Web Fetching (libcurl):** Fetches content using HTTP(S), stores response, ETag, and basic cache headers.
- **HTTP Caching Semantics:** Honors ETag, Cache-Control: max-age, and Last-Modified for efficiency.
- **Persistence:** URL mappings and cache contents survive program restarts (saved in `/data`).
- **Validation & Error Handling:** Input URLs are checked before usage. All I/O/network errors handled gracefully.
- **Configurable & Scalable:** Cache capacity adjustable at runtime.

## Project Folder Structure
```
SMART_WEB_CACHE/
│
├── include/           # Header files
│   ├── URLShortener.h
│   ├── LRUCache.h
│   ├── WebFetcher.h
│   └── Validator.h
│
├── src/               # Source files
│   ├── main.cpp
│   ├── URLShortener.cpp
│   ├── LRUCache.cpp
│   ├── WebFetcher.cpp
│   └── Validator.cpp
│
├── build/             # Persistence (auto-created)
│   ├── url_mappings.txt
│   └── cache_dump.txt
│
└── CMakeLists.txt
```




## Dependencies

- **C++17** or later
- **libcurl** (for HTTP/HTTPS web fetching)

### Install libcurl

- **Ubuntu/Debian:**  
  `sudo apt-get install libcurl4-openssl-dev`

- **macOS (Homebrew):**  
  `brew install curl`

## Building and Running

**With CMake (recommended):**

mkdir build
cd build
cmake ..
make
./SmartWebCache


**Or direct g++ command:**

g++ src/*.cpp -Iinclude -lcurl -o SmartWebCache
./SmartWebCache


## Usage

You’ll see a menu with these options:

1. Shorten URL – Input a long URL, receive a short code.
2. Restore Short URL – Enter a code, get back the original long URL.
3. Fetch URL Content – Enter a URL or short code, fetch/display content (cached if possible).
4. Exit – Saves state and exits.

- **Cache HIT:** Instant response from in-memory cache.
- **Cache MISS:** Fetches from web, then caches the result.
- **Persistence:** Exiting and relaunching keeps the same short codes and cache (if not expired).

## Example

Shorten URL
Enter long URL: https://www.example.com/sample
Shortened code: abcd12

Restore Short URL
Enter short code: abcd12
Original URL: https://www.example.com/sample

Fetch URL Content
Enter URL or short code: abcd12
[Cache MISS] Fetching...
HTML content snippet returned...

## Testing

Manual and automated tests cover:

- URL shortening with restoration
- LRU cache insertion/eviction
- Persistence (restart and restore functionality)
- HTTP cache headers (ETag, max-age)
- Input validation (invalid URLs are rejected)



## How it Works

- LRU cache is implemented with a doubly linked list and hash table for O(1) insert/lookup.
- URL shortener uses incremental base62 encoding.
- WebFetcher uses [libcurl](https://curl.se/libcurl/) and conditional HTTP GET for caching.
- On exit, mappings and cache contents are written to `/data`.

## Contributing

Issues and PRs are welcome!  
Please open an issue for bugs, enhancements, or questions.

---
