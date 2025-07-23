#include "WebFetcher.h"
#include <curl/curl.h>
#include <iostream>
#include <regex>
#include <ctime>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

struct HeaderData {
    std::string etag;
    std::string lastModified;
    long maxAge = 0;
};

size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
    size_t totalSize = size * nitems;
    std::string header(buffer, totalSize);
    HeaderData* hd = reinterpret_cast<HeaderData*>(userdata);

    std::regex etagRe(R"(ETag:\s*([^\r\n]+))", std::regex::icase);
    std::regex lastModRe(R"(Last-Modified:\s*([^\r\n]+))", std::regex::icase);
    std::regex cacheControlRe(R"(Cache-Control:\s*max-age=(\d+))", std::regex::icase);

    std::smatch match;
    if (std::regex_search(header, match, etagRe)) {
        hd->etag = match[1];
    }
    if (std::regex_search(header, match, lastModRe)) {
        hd->lastModified = match[1];
    }
    if (std::regex_search(header, match, cacheControlRe)) {
        hd->maxAge = std::stol(match[1]);
    }
    return totalSize;
}

CacheMetadata WebFetcher::fetch(const std::string& url, const CacheMetadata& prevMeta) {
    CURL* curl = curl_easy_init();
    CacheMetadata meta;
    if (!curl) {
        std::cerr << "[WebFetcher] curl init failed" << std::endl;
        return meta;
    }

    std::string buffer;
    HeaderData headerData;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerData);

    struct curl_slist *chunk = nullptr;
    if (!prevMeta.etag.empty()) {
        std::string ifNoneMatch = "If-None-Match: " + prevMeta.etag;
        chunk = curl_slist_append(chunk, ifNoneMatch.c_str());
    }
    if (!prevMeta.lastModified.empty()) {
        std::string ifModifiedSince = "If-Modified-Since: " + prevMeta.lastModified;
        chunk = curl_slist_append(chunk, ifModifiedSince.c_str());
    }
    if (chunk) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }

    CURLcode res = curl_easy_perform(curl);
    long response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

    if (chunk) curl_slist_free_all(chunk);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "[WebFetcher] curl perform error: " << curl_easy_strerror(res) << std::endl;
        meta.content = "Failed to fetch content.";
        meta.cacheExpiry = std::time(nullptr) + 60; // retry in 60 seconds
        return meta;
    }

    if (response_code == 304) { // Not Modified
        meta = prevMeta;
    } else if (response_code == 200) { // OK
        meta.content = buffer;
        meta.etag = headerData.etag;
        meta.lastModified = headerData.lastModified;

        time_t now = std::time(nullptr);
        meta.cacheExpiry = now + (headerData.maxAge > 0 ? headerData.maxAge : 300); // default 5 min
    } else {
        std::cerr << "[WebFetcher] HTTP response code: " << response_code << std::endl;
        meta.content = "Failed to fetch content. HTTP code: " + std::to_string(response_code);
        meta.cacheExpiry = std::time(nullptr) + 60;
    }

    return meta;
}
