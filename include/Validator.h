#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Validator {
public:
    static bool isValidURL(const std::string& url);
};

#endif
