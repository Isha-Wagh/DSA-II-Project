#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <string>
#include <list>
#include <unordered_map>
#include <ctime>

struct CacheMetadata {
    std::string content;
    std::string etag;
    std::string lastModified;
    time_t cacheExpiry; // Expiry timestamp in UNIX time

    CacheMetadata() : cacheExpiry(0) {}
};

class LRUCache {
private:
    int capacity;
    typedef std::pair<std::string, CacheMetadata> CacheNode;  // <url, cache metadata>
    std::list<CacheNode> cacheList;                           // Most recent front
    std::unordered_map<std::string, std::list<CacheNode>::iterator> cacheMap;

public:
    LRUCache(int cap);

    bool existsAndFresh(const std::string& url);
    CacheMetadata get(const std::string& url);
    void put(const std::string& url, const CacheMetadata& meta);
    void saveCache(const std::string& filename);
    void loadCache(const std::string& filename);
};

#endif
