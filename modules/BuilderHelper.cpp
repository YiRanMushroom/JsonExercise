//
// Created by Yiran on 2024-07-04.
//

#include <stdexcept>
#include <unordered_set>
#include "BuilderHelper.hpp"

using namespace n_BuilderHelper;

ExpectType BuilderHelper::expect() {
    using namespace std::string_literals;
    // try to read the next token and return the type of the token
    this->skip();
    // there are only 6 types of tokens, we need to check which type of token it is use the first character of the token
    switch (string[at]) {
        case '{':
            return ExpectType::OBJECT_Start;
        case '}':
            return ExpectType::OBJECT_End;
        case '[':
            return ExpectType::ARRAY_Start;
        case ']':
            return ExpectType::ARRAY_End;
        case '"':
            return ExpectType::STRING;
        case 't':
        case 'f':
            return ExpectType::BOOL;
        case 'n':
            return ExpectType::NULLPTR;
        case '+':
        case '-':
            return ExpectType::NUMBER;
        default:
            if (string[at] >= '0' && string[at] <= '9') {
                return ExpectType::NUMBER;
            } else {
                throw std::runtime_error("Invalid Character: "s + string[at] + " at position: "
                                         + std::to_string(at)
                                         + "\nBecause expect cannot deduce the type of the token"
                );
            }
    }
}

void BuilderHelper::skip() {
    // skip empty space, enter, tab, etc.
    while (at < string.size() && (string[at] == ' ' || string[at] == '\n' || string[at] == '\t')
           // we also want to skip comma, : and
           || string[at] == ',' || string[at] == ':'
            ) {
        at++;
    }
}

const std::unordered_map<char, char> BuilderHelper::escapeMap = {
        {'"',  '"'},
        {'\\', '\\'},
        {'/',  '/'},
        {'b',  '\b'},
        {'f',  '\f'},
        {'n',  '\n'},
        {'r',  '\r'},
        {'t',  '\t'}
};

char BuilderHelper::getEscape(char c) {
    if (escapeMap.find(c) != escapeMap.end()) {
        return escapeMap.at(c);
    } else {
        throw std::runtime_error(std::string("Invalid Escape Character: ") + c);
    }
}

bool shouldStop(const char c) {
    switch (c) {
        case '{':
        case '[':
        case '"':
        case 'f':
        case 't':
        case 'n':
        case '+':
        case '-':
            return true;
        default:
            if (c >= '0' && c <= '9') {
                return true;
            } else {
                return false;
            }
    }
}

void BuilderHelper::ready() {
    while (!shouldStop(string[at])) {
        at++;
    }
}

void BuilderHelper::nextChar() {
    at++;
}

std::string readFileIntoString(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

template<>
double BuilderHelper::next<double>() {
    size_t start = at;
    while (at < string.size() && (string[at] == '.' || (string[at] >= '0' && string[at] <= '9')
                                  || (string[at] == '-') || (string[at] == '+')
                                  || (string[at] == 'e'))) {
        at++;
    }
    if (at == start) {
        std::unreachable();
    }
    return std::stod(string.substr(start, at - start));
}

template<>
std::string BuilderHelper::next<std::string>() {
    std::string result;
    if (string[at] != '"') {
        std::unreachable();
    }
    at++;
    while (at < string.size() && string[at] != '"') {
        if (string[at] == '\\') {
            at++;
            result.push_back(getEscape(string[at]));
        } else {
            result.push_back(string[at]);
        }
        at++;
    }
    at++;
    return result;
}

template<>
bool BuilderHelper::next<bool>() {
    if (string.substr(at, 4) == "true") {
        at += 4;
        return true;
    } else if (string.substr(at, 5) == "false") {
        at += 5;
        return false;
    } else {
        std::unreachable();
    }
}

template<>
std::nullptr_t BuilderHelper::next<std::nullptr_t>() {
    if (string.substr(at, 4) == "null") {
        at += 4;
        return nullptr;
    } else {
        std::unreachable();
    }
}