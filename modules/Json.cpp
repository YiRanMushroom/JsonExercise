//
// Created by Yiran on 2024-07-04.
//

#include <map>
#include "Json.hpp"

using namespace n_Json;
using namespace n_BuilderHelper;

JsonBuilder &JsonBuilder::setString(const std::string &string) {
    helper = std::make_unique<BuilderHelper>(string);
    return *this;
}

JsonBuilder &JsonBuilder::setString(std::string &&string) {
    helper = std::make_unique<BuilderHelper>(std::move(string));
    return *this;
}

std::shared_ptr<JsonNode> JsonBuilder::build() {
    if (helper->expect() != ExpectType::OBJECT_Start)
        throw std::runtime_error("Invalid Json Format, cannot find the start of the object '{'");
    std::shared_ptr<JsonNode> node;
    helper->next<ObjectNode>([&](auto ptr) {
        node = ptr;
    });
    return node;
}

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

        const char left, right;

        SmartPrinter &printer;

        IndentPrintGuard(const char left, const char right, SmartPrinter &printer) : left(left), right(right),
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

    SmartPrinter &autoAppend(this SmartPrinter &self, const JsonNode &node) {
        switch (node.getContext().getType()) {
            case JsonType::OBJECT:
                return self(node.getData<Object>());
            case JsonType::ARRAY:
                return self(node.getData<Array>());
            case JsonType::STRING:
                return self(node.getData<String>());
            case JsonType::NUMBER:
                return self(node.getData<Number>());
            case JsonType::BOOL:
                return self(node.getData<Bool>());
            case JsonType::NULLPTR:
                return self(node.getData<NullPtr>());
            default:
                throw std::runtime_error("Invalid Json Format, cannot deduce the type of the token");
        }
    }

    SmartPrinter &operator()(this SmartPrinter &self, const Object &map) {
        IndentPrintGuard guard{'{', '}', self};

        size_t i = 0;
        const size_t lastIndex = map.size() - 1;
        for (auto &&[K, V]: map) {
            (self << '"' << K << '"').colonSpace().autoAppend(*V);
            if (i++ != lastIndex) self.commaNextLine();
        }

        return self;
    }

    SmartPrinter &operator()(this SmartPrinter &self, const Array &array) {
        IndentPrintGuard guard{'[', ']', self};

        for (size_t i = 0; i < array.size() - 1; i++) {
            self.autoAppend(*array[i]);
            self.commaNextLine();
        }
        self.autoAppend(*array.back());

        return self;
    }

    SmartPrinter &operator()(this SmartPrinter &self, const Number number) {
        return self << number;
    }

    SmartPrinter &operator()(this SmartPrinter &self, const Bool val) {
        self.ss << std::boolalpha << val;
        return self;
    }

    SmartPrinter &operator()(this SmartPrinter &self, const NullPtr nullPtr) {
        return self << "null";
    }

    SmartPrinter &operator()(this SmartPrinter &self, const String &string) {
        static const std::map<char, std::string> escapeMap = {
                {'\n', "\\n"},
                {'\r', "\\r"},
                {'\t', "\\t"},
                {'\b', "\\b"},
                {'\f', "\\f"},
                {'"',  "\\\""}
        };

        self << '"';
        for (char i: string) {
            if (escapeMap.find(i) != escapeMap.end()) {
                self << escapeMap.at(i);
            } else {
                self << i;
            }
        }
        return self << '"';
    }

    [[nodiscard]] std::string build() const {
        return ss.str();
    }
};

std::string JsonNode::toString() const {
    SmartPrinter printer;
    return printer.autoAppend(*this).build();
}