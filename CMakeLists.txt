cmake_minimum_required(VERSION 3.10)
project(SmartWebCache)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

include_directories(include)

add_executable(SmartWebCache
    src/main.cpp
    src/URLShortener.cpp
    src/LRUCache.cpp
    src/WebFetcher.cpp
    src/Validator.cpp
)

target_link_libraries(SmartWebCache PRIVATE ${CURL_LIBRARIES})
target_include_directories(SmartWebCache PRIVATE ${CURL_INCLUDE_DIRS})
