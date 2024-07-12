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

void JsonNode::set(const std::shared_ptr<JsonNode> &other) {
//    std::cout << "JsonNode::set(const JsonNode &other)" << std::endl;
    set(std::make_shared<JsonNode>(other->deepCopy()));
}

void JsonNode::set(std::shared_ptr<JsonNode> &&other) {
//    std::cout << "JsonNode::set(JsonNode &&other)" << std::endl;
    if (this == other.get())
        throw std::logic_error("Move Assignment an instance to itself is not logically correct. "
                               "(at: JsonNode::set(JsonNode &&other))");
    this->~JsonNode();
    switch (other->getContext().getType()) {
        case JsonType::OBJECT:
            new(this) ObjectNode(std::move(other->getData<Object>()));
            break;
        case JsonType::ARRAY:
            new(this) ArrayNode(std::move(other->getData<Array>()));
            break;
        case JsonType::STRING:
            new(this) StringNode(std::move(other->getData<String>()));
            break;
        case JsonType::NUMBER:
            new(this) NumberNode(other->getData<Number>());
            break;
        case JsonType::BOOL:
            new(this) BoolNode(other->getData<Bool>());
            break;
        case JsonType::NULLPTR:
            new(this) NullNode();
            break;
        default:
            throw std::runtime_error("Other node is not Initialized (at: JsonNode::set(JsonNode &&other))");
    }
}

JsonNode &JsonNode::operator=(const JsonNode &node) {
    return *this = std::move(node.deepCopy());
}

JsonNode &JsonNode::operator=(JsonNode &&node) {
    this->~JsonNode();
    switch (node.getContext().getType()) {
        case JsonType::OBJECT:
            new(this) ObjectNode(std::move(node.getData<Object>()));
            break;
        case JsonType::ARRAY:
            new(this) ArrayNode(std::move(node.getData<Array>()));
            break;
        case JsonType::STRING:
            new(this) StringNode(std::move(node.getData<String>()));
            break;
        case JsonType::NUMBER:
            new(this) NumberNode(node.getData<Number>());
            break;
        case JsonType::BOOL:
            new(this) BoolNode(node.getData<Bool>());
            break;
        case JsonType::NULLPTR:
            new(this) NullNode();
            break;
        default:
            throw std::runtime_error("Other node is not Initialized (at: JsonNode::operator=(JsonNode &&node))");
    }

    return *this;
}

JsonNode::JsonNode(const JsonNode &node) : JsonNode(std::move(node.deepCopy())) {
}

JsonNode::JsonNode(JsonNode &&node) {
    this->~JsonNode();
    switch (node.getContext().getType()) {
        case JsonType::OBJECT:
            new(this) ObjectNode(std::move(node.getData<Object>()));
            break;
        case JsonType::ARRAY:
            new(this) ArrayNode(std::move(node.getData<Array>()));
            break;
        case JsonType::STRING:
            new(this) StringNode(std::move(node.getData<String>()));
            break;
        case JsonType::NUMBER:
            new(this) NumberNode(node.getData<Number>());
            break;
        case JsonType::BOOL:
            new(this) BoolNode(node.getData<Bool>());
            break;
        case JsonType::NULLPTR:
            new(this) NullNode();
            break;
        default:
            throw std::runtime_error("Other node is not Initialized (at: JsonNode::operator=(JsonNode &&node))");
    }
}

JsonNode ObjectNode::deepCopy() const {
    Object obj;
    for (auto &&[K, V]: getData<Object>()) {
        obj[K] = std::make_shared<JsonNode>(V->deepCopy());
    }
    return JsonNode{ObjectNode{std::move(obj)}};
}

JsonNode ArrayNode::deepCopy() const {
    Array arr;
    for (auto &&i: getData<Array>()) {
        arr.push_back(std::make_shared<JsonNode>(i->deepCopy()));
    }
    return JsonNode{ArrayNode{std::move(arr)}};
}

JsonNode StringNode::deepCopy() const {
    return JsonNode{StringNode{getData<String>()}};
}

JsonNode NumberNode::deepCopy() const {
    return JsonNode{NumberNode{getData<Number>()}};
}

JsonNode BoolNode::deepCopy() const {
    return JsonNode{BoolNode{getData<Bool>()}};
}

JsonNode NullNode::deepCopy() const {
    return JsonNode(NullNode());
}
