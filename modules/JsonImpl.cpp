#include <stdexcept>
#include <fstream>
#include <sstream>
#include "JsonForwardHeader.hpp"
#include "Json.hpp"

namespace Json {
    struct Builder {
        const std::string_view sv;
        size_t pos = 0;

        explicit Builder(const std::string_view &sv) : sv(sv) {}

        char now() {
            return sv[pos];
        }

        enum class Signal {
            STRING,
            OBJECT,
            ARRAY,
            NUMBER,
            BOOL,
            NULLPTR,
            ObjectEnd,
            ArrayEnd
        };

        Signal nextType() {
            while (pos < sv.size()) {
                switch (now()) {
                    case '"':
                        return Signal::STRING;
                    case '{':
                        return Signal::OBJECT;
                    case '[':
                        return Signal::ARRAY;
                    case 't':
                    case 'f':
                        return Signal::BOOL;
                    case 'n':
                        return Signal::NULLPTR;
                    case '-':
                    case '+':
                    case '.':
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        return Signal::NUMBER;
                    case '}':
                        return Signal::ObjectEnd;
                    case ']':
                        return Signal::ArrayEnd;
                    default:
                        break;
                }
                pos++;
            }
            throw std::runtime_error("Invalid Json Format, cannot deduce the type of the token");
        }

        template<class Type = void>
        Json build() {
            switch (nextType()) {
                case Signal::OBJECT:
                    return build<Object>();
                case Signal::ARRAY:
                    return build<Array>();
                case Signal::STRING:
                    return build<String>();
                case Signal::NUMBER:
                    return build<Number>();
                case Signal::BOOL:
                    return build<Bool>();
                case Signal::NULLPTR:
                    return build<NullPtr>();
                default:
                    throw std::runtime_error("Invalid Json Format, cannot deduce the type of the token");
            }
        }

        String readString() {
            const static std::map<char, char> escapeMap = {
                    {'"',  '"'},
                    {'\\', '\\'},
                    {'/',  '/'},
                    {'b',  '\b'},
                    {'f',  '\f'},
                    {'n',  '\n'},
                    {'r',  '\r'},
                    {'t',  '\t'}
            };
            ++pos;
            String res;
            while (pos < sv.size() && now() != '"') {
                if (now() == '\\') {
                    pos++;
                    res.push_back(escapeMap.at(now()));
                } else {
                    res.push_back(now());
                }
                pos++;
            }
            pos++;
            return res;
        }

        template<>
        Json build<String>() {
            return Json{readString()};
        }

        template<>
        Json build<Number>() {

            size_t start = pos;
            while (pos < sv.size() && (now() == '.' || (now() >= '0' && now() <= '9')
                                       || (now() == '-') || (now() == '+')
                                       || (now() == 'e'))) {
                pos++;
            }

            return Json{std::stod(sv.substr(start, pos - start).data())};
        }

        template<>
        Json build<Bool>() {
            if (sv.substr(pos, 4) == "true") {
                pos += 4;
                return Json{true};
            } else if (sv.substr(pos, 5) == "false") {
                pos += 5;
                return Json{false};
            } else {
                throw std::runtime_error("Invalid Json Format, cannot deduce the type of the token");
            }
        }

        template<>
        Json build<NullPtr>() {
            if (sv.substr(pos, 4) == "null") {
                pos += 4;
                return {};
            } else {
                throw std::runtime_error("Invalid Json Format, cannot deduce the type of the token");
            }
        }

        template<>
        Json build<Object>() {
            Object obj;
            pos++;

            while (nextType() != Signal::ObjectEnd) {
                String str = readString();
                obj[std::move(str)] = build();
            }

            pos++;
            return Json{std::move(obj)};
        }

        template<>
        Json build<Array>() {
            Array arr;
            pos++;
            while (nextType() != Signal::ArrayEnd) {
                arr.push_back(build());
            }
            pos++;
            return Json{std::move(arr)};
        }
    };


    Json parseJson(const std::string &str) {
        return Builder(str).build();
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

    Json parseJsonFromFile(const std::string &filename) {
        return parseJson(readFileIntoString(filename));
    }
}