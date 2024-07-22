//
// Created by Yiran on 2024-07-13.
//

#include <sstream>
#include "Json.hpp"

namespace Json {
    struct SmartPrinter {
        size_t indent = 0;
        constexpr static size_t indentSize = 2;

        std::stringstream ss;

        SmartPrinter &nextLine() {
            ss << '\n';
            make_indent();
            return *this;
        }

        SmartPrinter &commaNextLine() {
            ss << ",\n";
            make_indent();
            return *this;
        }

        SmartPrinter &colonSpace() {
            ss << ": ";
            return *this;
        }

        SmartPrinter &make_indent() {
            for (size_t i = 0; i < indent; i++) {
                for (size_t j = 0; j < indentSize; j++) {
                    ss << ' ';
                }
            }
            return *this;
        }

        struct IndentPrintGuard {
            IndentPrintGuard() = delete;

            const char right;

            SmartPrinter &printer;

            IndentPrintGuard(const char left, const char right, SmartPrinter &printer) : right(right),
                                                                                         printer(printer) {
                printer << left;
                printer.indent++;
                printer.nextLine();
            }

            ~IndentPrintGuard() {
                printer.indent--;
                printer.nextLine();
                printer << right;
            }
        };

        SmartPrinter &operator<<(const std::string &str) {
            ss << str;
            return *this;
        }

        SmartPrinter &operator<<(const char c) {
            ss << c;
            return *this;
        }

        SmartPrinter &operator<<(const double d) {
            ss << d;
            return *this;
        }

/*        SmartPrinter &autoAppend(const Json &json) {
            return json.visit(*this);
        }*/

        SmartPrinter &operator()(const Object &map) {
            IndentPrintGuard guard{'{', '}', *this};

            size_t i = 0;
            const size_t lastIndex = map.size() - 1;
            for (auto &&[K, V]: map) {
                V.visit((*this << '"' << K << '"').colonSpace());
                if (i++ != lastIndex) this->commaNextLine();
            }

            return *this;
        }

        SmartPrinter &operator()(const Array &array) {
            IndentPrintGuard guard{'[', ']', *this};

            for (size_t i = 0; i < array.size() - 1; i++) {
                array[i].visit(*this).commaNextLine();
            }

            return array.back().visit(*this);
        }

        SmartPrinter &operator()(const Number number) {
            return *this << number;
        }

        SmartPrinter &operator()(const Bool val) {
            this->ss << std::boolalpha << val;
            return *this;
        }

        SmartPrinter &operator()(const NullPtr) {
            return *this << "null";
        }

        SmartPrinter &operator()(const String &string) {
            static const std::map<char, std::string> escapeMap = {
                    {'\n', "\\n"},
                    {'\r', "\\r"},
                    {'\t', "\\t"},
                    {'\b', "\\b"},
                    {'\f', "\\f"},
                    {'"',  "\\\""}
            };

            *this << '"';
            for (char i: string) {
                if (escapeMap.find(i) != escapeMap.end()) {
                    *this << escapeMap.at(i);
                } else {
                    *this << i;
                }
            }
            return *this << '"';
        }

        [[nodiscard]] std::string build() const {
            return ss.str();
        }
    };

    std::string Json::toString() {
        return this->visit(SmartPrinter{}).build();
    }
}