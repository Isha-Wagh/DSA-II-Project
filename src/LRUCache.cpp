#include "LRUCache.h"
#include <fstream>
#include <sstream>
#include <iostream>

LRUCache::LRUCache(int cap) : capacity(cap) {}

bool LRUCache::existsAndFresh(const std::string& url) {
    auto it = cacheMap.find(url);
    if (it == cacheMap.end())
        return false;

    CacheMetadata &meta = it->second->second;
    time_t now = std::time(nullptr);

    // Check expiration
    if (meta.cacheExpiry != 0 && meta.cacheExpiry < now)
        return false;

    return true;
}

CacheMetadata LRUCache::get(const std::string& url) {
    auto it = cacheMap.find(url);
    if (it == cacheMap.end())
        return CacheMetadata();

    // Move to front
    cacheList.splice(cacheList.begin(), cacheList, it->second);
    return it->second->second;
}

void LRUCache::put(const std::string& url, const CacheMetadata& meta) {
    auto it = cacheMap.find(url);
    if (it != cacheMap.end()) {
        it->second->second = meta;
        cacheList.splice(cacheList.begin(), cacheList, it->second);
    } else {
        cacheList.push_front({url, meta});
        cacheMap[url] = cacheList.begin();

        if ((int)cacheList.size() > capacity) {
            auto last = cacheList.end();
            --last;
            cacheMap.erase(last->first);
            cacheList.pop_back();
        }
    }
}

void LRUCache::saveCache(const std::string& filename) {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[LRUCache] Failed to save cache to " << filename << std::endl;
        return;
    }

    for (const auto &item : cacheList) {
        const auto& url = item.first;
        const auto& meta = item.second;

        // Format: url|etag|lastModified|expiry|content_length|content
        file << url << "|"
             << meta.etag << "|"
             << meta.lastModified << "|"
             << meta.cacheExpiry << "|"
             << meta.content.size() << "|"
             << meta.content << "\n";
    }
}

void LRUCache::loadCache(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        return;

    std::string line;
    while (std::getline(file, line)) {
        size_t pos[6];
        pos[0] = line.find('|');
        if (pos[0] == std::string::npos) continue;
        pos[1] = line.find('|', pos[0] + 1);
        if (pos[1] == std::string::npos) continue;
        pos[2] = line.find('|', pos[1] + 1);
        if (pos[2] == std::string::npos) continue;
        pos[3] = line.find('|', pos[2] + 1);
        if (pos[3] == std::string::npos) continue;
        pos[4] = line.find('|', pos[3] + 1);
        if (pos[4] == std::string::npos) continue;

        std::string url = line.substr(0, pos[0]);
        std::string etag = line.substr(pos[0] + 1, pos[1] - pos[0] - 1);
        std::string lastModified = line.substr(pos[1] + 1, pos[2] - pos[1] - 1);
        time_t cacheExpiry = std::stoll(line.substr(pos[2] + 1, pos[3] - pos[2] - 1));
        size_t content_len = std::stoull(line.substr(pos[3] + 1, pos[4] - pos[3] - 1));
        std::string content = line.substr(pos[4] + 1);

        // Defensive: truncate content if longer than specified length
        if (content.size() > content_len)
            content = content.substr(0, content_len);

        CacheMetadata meta;
        meta.etag = etag;
        meta.lastModified = lastModified;
        meta.cacheExpiry = cacheExpiry;
        meta.content = content;

        put(url, meta);
    }
}
