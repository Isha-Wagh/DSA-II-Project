#ifndef URLSHORTENER_H
#define URLSHORTENER_H

#include <string>
#include <unordered_map>

class URLShortener {
private:
    std::unordered_map<std::string, std::string> shortToLong, longToShort;
    int urlCount;
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::string encodeBase62(int num) const;

public:
    URLShortener();
    std::string shorten(const std::string& longURL);
    std::string restore(const std::string& shortURL);
    void saveMappings(const std::string& filename);
    void loadMappings(const std::string& filename);
};

#endif
