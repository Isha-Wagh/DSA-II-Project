#include "Validator.h"
#include <regex>

bool Validator::isValidURL(const std::string& url) {
    std::regex url_regex(
        R"(^(https?://)[a-zA-Z0-9\-\.]+\.[a-zA-Z]{2,}(:[0-9]{1,5})?(/[^\s]*)?$)",
        std::regex::icase
    );
    return std::regex_match(url, url_regex);
}
