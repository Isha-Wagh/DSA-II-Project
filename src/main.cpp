#include <iostream>
#include <filesystem>
#include "URLShortener.h"
#include "LRUCache.h"
#include "WebFetcher.h"
#include "Validator.h"

int main() {
    std::filesystem::create_directory("data");

    URLShortener shortener;
    shortener.loadMappings("data/url_mappings.txt");

    int cacheCap = 5;
    std::cout << "Enter LRU Cache Capacity (default 5): ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            cacheCap = std::stoi(input);
        } catch (...) {
            std::cout << "Invalid input. Using default capacity 5\n";
            cacheCap = 5;
        }
    }

    LRUCache cache(cacheCap);
    cache.loadCache("data/cache_dump.txt");

    std::string choice;
    while (true) {
        std::cout << "\nMenu:\n1. Shorten URL\n2. Restore Short URL\n3. Fetch URL Content\n4. Exit\nChoice: ";
        std::getline(std::cin, choice);

        if (choice == "1") {
            std::cout << "Enter long URL: ";
            std::string longURL;
            std::getline(std::cin, longURL);

            if (!Validator::isValidURL(longURL)) {
                std::cout << "Invalid URL format. Must start with http(s)://\n";
                continue;
            }

            std::string shortCode = shortener.shorten(longURL);
            std::cout << "Shortened code: " << shortCode << "\n";

        } else if (choice == "2") {
            std::cout << "Enter short code: ";
            std::string shortCode;
            std::getline(std::cin, shortCode);

            std::string longURL = shortener.restore(shortCode);
            if (longURL.empty())
                std::cout << "Short code not found.\n";
            else
                std::cout << "Original URL: " << longURL << "\n";

        } else if (choice == "3") {
            std::cout << "Enter URL or short code: ";
            std::string inputURL;
            std::getline(std::cin, inputURL);

            // Check if input is a short code
            std::string longURL = shortener.restore(inputURL);
            if (longURL.empty()) longURL = inputURL;

            if (!Validator::isValidURL(longURL)) {
                std::cout << "Invalid URL format.\n";
                continue;
            }

            if (cache.existsAndFresh(longURL)) {
                std::cout << "[Cache HIT]\n";
                CacheMetadata meta = cache.get(longURL);
                std::cout << meta.content.substr(0, 256) << (meta.content.size() > 256 ? "..." : "") << "\n";
            } else {
                std::cout << "[Cache MISS] Fetching...\n";
                CacheMetadata prevMeta;
                if (cache.existsAndFresh(longURL))
                    prevMeta = cache.get(longURL);
                CacheMetadata freshMeta = WebFetcher::fetch(longURL, prevMeta);
                cache.put(longURL, freshMeta);
                std::cout << freshMeta.content.substr(0, 256) << (freshMeta.content.size() > 256 ? "..." : "") << "\n";
            }

        } else if (choice == "4") {
            std::cout << "Exiting...\n";
            break;
        } else {
            std::cout << "Invalid choice. Try again.\n";
        }
    }

    shortener.saveMappings("data/url_mappings.txt");
    cache.saveCache("data/cache_dump.txt");

    return 0;
}
