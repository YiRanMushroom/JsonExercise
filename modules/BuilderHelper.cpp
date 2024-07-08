//
// Created by Yiran on 2024-07-04.
//

#include <stdexcept>
#include <unordered_set>
#include "BuilderHelper.hpp"
#include "Json.hpp"

using namespace n_BuilderHelper;

ExpectType BuilderHelper::expect() {
    using namespace std::string_literals;
    // try to read the next token and return the type of the token
    this->skip();
    // there are only 6 types of tokens, we need to check which type of token it is use the first character of the token
    switch (now()) {
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
            if (now() >= '0' && now() <= '9') {
                return ExpectType::NUMBER;
            } else {
                throw std::runtime_error("Invalid Character: "s + now() + " at position: "
                                         + std::to_string(at)
                                         + "\nBecause expect cannot deduce the type of the token"
                );
            }
    }
}

void BuilderHelper::skip() {
    // skip empty space, enter, tab, etc.
    while (at < getString().size() && (now() == ' ' || now() == '\n' || now() == '\t'
                                       // we also want to skip comma, : and
                                       || now() == ',' || now() == ':')
            ) {
        at++;
    }
}

char BuilderHelper::getEscape(char c) {
    const static std::unordered_map<char, char> escapeMap = {
            {'"',  '"'},
            {'\\', '\\'},
            {'/',  '/'},
            {'b',  '\b'},
            {'f',  '\f'},
            {'n',  '\n'},
            {'r',  '\r'},
            {'t',  '\t'}
    };

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
    while (!shouldStop(now())) {
        at++;
    }
}

void BuilderHelper::nextChar() {
    at++;
}

const char &BuilderHelper::now() const {
    return (*shardString)[at];
}

template<>
void n_BuilderHelper::BuilderHelper::next<void>(
        const std::function<void(std::shared_ptr<n_Json::JsonNode> &&)> &provider) {
    using namespace n_Json;

    switch (expect()) {
        case ExpectType::OBJECT_Start:
            this->next<ObjectNode>(provider);
            break;
        case ExpectType::ARRAY_Start:
            this->next<ArrayNode>(provider);
            break;
        case ExpectType::STRING:
            this->next<StringNode>(provider);
            break;
        case ExpectType::NUMBER:
            this->next<NumberNode>(provider);
            break;
        case ExpectType::BOOL:
            this->next<BoolNode>(provider);
            break;
        case ExpectType::NULLPTR:
            this->next<NullNode>(provider);
            break;
        default:
            throw std::runtime_error("Invalid Json Format, cannot deduce the type of the token");
    }
}

std::string n_BuilderHelper::readFileIntoString(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

using namespace n_Json;

template<>
String BuilderHelper::read<String>() {
    std::string result;
    if (getString()[at] != '"') {
        std::unreachable();
    }
    at++;
    while (at < getString().size() && now() != '"') {
        if (now() == '\\') {
            at++;
            result.push_back(getEscape(now()));
        } else {
            result.push_back(now());
        }
        at++;
    }
    at++;
    return result;
}

template<>
Number BuilderHelper::read<Number>() {
    size_t start = at;
    while (at < getString().size() && (now() == '.' || (now() >= '0' && now() <= '9')
                                       || (now() == '-') || (now() == '+')
                                       || (now() == 'e'))) {
        at++;
    }
    if (at == start) {
        std::unreachable();
    }
    return std::stod(getString().substr(start, at - start));
}

template<>
Bool BuilderHelper::read<Bool>() {
    if (getString().substr(at, 4) == "true") {
        at += 4;
        return true;
    } else if (getString().substr(at, 5) == "false") {
        at += 5;
        return false;
    } else {
        std::unreachable();
    }
}

template<>
NullPtr BuilderHelper::read<NullPtr>() {
    if (getString().substr(at, 4) == "null") {
        at += 4;
        return nullptr;
    } else {
        std::unreachable();
    }
}

template<>
Object BuilderHelper::read<Object>() {
    Object map; // create a map to store the key value pair

    nextChar();

    while (expect() != ExpectType::OBJECT_End) {
        ready();
        std::string key = read < std::string > ();

        ready();
        this->next([&](std::shared_ptr<JsonNode> &&ptr) {
            map[std::move(key)] = ptr;
        });
    }

    nextChar();
    return map;
}

template<>
Array BuilderHelper::read<Array>() {
    Array array;

    nextChar();

    while (expect() != ExpectType::ARRAY_End) {
        ready();

        this->next([&](std::shared_ptr<JsonNode> &&ptr) {
            array.push_back(ptr);
        });
    }

    nextChar();
    return array;
}

