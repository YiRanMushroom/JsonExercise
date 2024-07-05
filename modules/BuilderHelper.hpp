#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>

enum class ExpectType {
    OBJECT_Start,
    OBJECT_End,
    ARRAY_Start,
    ARRAY_End,
    STRING,
    NUMBER,
    BOOL,
    NULLPTR
};

class BuilderHelper {
public:
    explicit BuilderHelper(const std::string &string) : string(string) {}

    explicit BuilderHelper(std::string &&string) : string(std::move(string)) {}

    ExpectType expect();

    template<typename R>
    R next() {
        std::unreachable();
    }

    void skip();

    const static std::unordered_map<char, char> escapeMap;

    static char getEscape(char c);

    void ready();

    void nextChar();

private:
    std::string string;

    size_t at = 0;

public:
    BuilderHelper() = delete;

    BuilderHelper(BuilderHelper &) = delete;

};

template<>
double BuilderHelper::next<double>();

template<>
std::string BuilderHelper::next<std::string>();

template<>
bool BuilderHelper::next<bool>();

template<>
std::nullptr_t BuilderHelper::next<std::nullptr_t>();

std::string readFileIntoString(const std::string &filename);