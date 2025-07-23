#ifndef WEBFETCHER_H
#define WEBFETCHER_H

#include <string>
#include "LRUCache.h"

class WebFetcher {
public:
    // Fetch with optional previous metadata for conditional GET (ETag/If-Modified-Since)
    static CacheMetadata fetch(const std::string& url, const CacheMetadata& prevMeta = {});
};

#endif
