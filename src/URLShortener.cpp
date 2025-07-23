#include "URLShortener.h"
#include <fstream>
#include <sstream>
#include <iostream>

URLShortener::URLShortener() : urlCount(0) {}

std::string URLShortener::encodeBase62(int num) const {
    std::string res;
    do {
        res = chars[num % 62] + res;
        num /= 62;
    } while (num);
    while (res.size() < 6) res = 'a' + res; // Pad to length 6 with 'a'
    return res;
}

std::string URLShortener::shorten(const std::string& longURL) {
    if (longToShort.count(longURL))
        return longToShort.at(longURL);
    std::string shortURL = encodeBase62(urlCount++);
    shortToLong[shortURL] = longURL;
    longToShort[longURL] = shortURL;
    return shortURL;
}

std::string URLShortener::restore(const std::string& shortURL) {
    if (shortToLong.count(shortURL))
        return shortToLong.at(shortURL);
    return ""; // Not found
}

void URLShortener::saveMappings(const std::string& filename) {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[URLShortener] Failed to save mappings to " << filename << std::endl;
        return;
    }
    for (auto& pair : shortToLong) {
        file << pair.first << "," << pair.second << "\n";
    }
}

void URLShortener::loadMappings(const std::string& filename) {
    shortToLong.clear();
    longToShort.clear();
    urlCount = 0;

    std::ifstream file(filename);
    if (!file.is_open()) return; // File may not exist on first run

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string shortCode, longURL;
        if (std::getline(iss, shortCode, ',') && std::getline(iss, longURL)) {
            shortToLong[shortCode] = longURL;
            longToShort[longURL] = shortCode;
            urlCount++;
        }
    }
}
