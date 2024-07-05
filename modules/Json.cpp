//
// Created by Yiran on 2024-07-04.
//

#include <map>
#include "Json.hpp"

JsonBuilder &JsonBuilder::setString(const std::string &string) {
    helper = std::make_unique<BuilderHelper>(string);
    return *this;
}

JsonBuilder &JsonBuilder::setString(std::string &&string) {
    helper = std::make_unique<BuilderHelper>(std::move(string));
    return *this;
}

JsonNode JsonBuilder::build() {
    if (helper->expect() != ExpectType::OBJECT_Start)
        throw std::runtime_error("Invalid Json Format, cannot find the start of the object '{'");
    helper->ready();
    return nextObject();
}

JsonNode JsonBuilder::nextObject() {
    Map map; // create a map to store the key value pair
    // read K, V until we reach the end of the object

    helper->nextChar();

    while (helper->expect() != ExpectType::OBJECT_End) {
        helper->ready();
        // read key
        std::string key = helper->next<std::string>();
        // ready for read the value.
        helper->ready();
        switch (helper->expect()) {
            case ExpectType::OBJECT_Start:
                map[std::move(key)] = nextObject();
                break;
            case ExpectType::ARRAY_Start:
                map[std::move(key)] = nextArray();
                break;
            case ExpectType::STRING:
                map[std::move(key)] = JsonNode(
                        JsonType::STRING, std::make_shared<StringDataHolder>(helper->next<std::string>()));
                break;
            case ExpectType::NUMBER:
                map[std::move(key)] = JsonNode(
                        JsonType::NUMBER, std::make_shared<NumberDataHolder>(helper->next<double>()));
                break;
            case ExpectType::BOOL:
                map[std::move(key)] = JsonNode(
                        JsonType::BOOL, std::make_shared<BoolDataHolder>(helper->next<bool>()));
                break;
            case ExpectType::NULLPTR:
                map[std::move(key)] = JsonNode(JsonType::NULLPTR, helper->next<nullptr_t>());
                break;
            default:
                throw std::runtime_error("Invalid Json Format, cannot deduce the type of the token");
        }
    }
    helper->nextChar();
    return JsonNode{JsonType::OBJECT, std::make_shared<ObjectDataHolder>(std::move(map))};
}

JsonNode JsonBuilder::nextArray() {
    std::vector<JsonNode> array;

    helper->nextChar();

    while (helper->expect() != ExpectType::ARRAY_End) {
        helper->ready();
        switch (helper->expect()) {
            case ExpectType::OBJECT_Start:
                array.push_back(nextObject());
                break;
            case ExpectType::ARRAY_Start:
                array.push_back(nextArray());
                break;
            case ExpectType::STRING:
                array.emplace_back(
                        JsonType::STRING, std::make_shared<StringDataHolder>(helper->next<std::string>()));
                break;
            case ExpectType::NUMBER:
                array.emplace_back(
                        JsonType::NUMBER, std::make_shared<NumberDataHolder>(helper->next<double>()));
                break;
            case ExpectType::BOOL:
                array.emplace_back(
                        JsonType::BOOL, std::make_shared<BoolDataHolder>(helper->next<bool>()));
                break;
            case ExpectType::NULLPTR:
                array.emplace_back(JsonType::NULLPTR, std::make_shared<NullDataHolder>());
                break;
            default:
                throw std::runtime_error("Invalid Json Format, cannot deduce the type of the token");
        }
    }
    helper->nextChar();
    return JsonNode{JsonType::ARRAY, std::make_shared<ArrayDataHolder>(std::move(array))};
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

    SmartPrinter &commaSpace() {
        ss << ", ";
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

    struct indentGuard {
        indentGuard() = delete;

        explicit indentGuard(SmartPrinter &printer) : printer(printer) {
            printer.indent++;
        }

        ~indentGuard() {
            printer.indent--;
        }

        SmartPrinter &printer;
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
        switch (node.type) {
            case JsonType::OBJECT:
                return self((ObjectDataHolder &) *node.data);
            case JsonType::ARRAY:
                return self((ArrayDataHolder &) *node.data);
            case JsonType::STRING:
                return self((StringDataHolder &) *node.data);
            case JsonType::NUMBER:
                return self((NumberDataHolder &) *node.data);
            case JsonType::BOOL:
                return self((BoolDataHolder &) *node.data);
            case JsonType::NULLPTR:
                return self((NullDataHolder &) *node.data);
            default:
                throw std::runtime_error("Invalid Json Format, cannot deduce the type of the token");
        }
    }

    SmartPrinter &operator()(this SmartPrinter &self, const ObjectDataHolder &dataHolder) {
        self << "{";
        {
            indentGuard guard{self};
            self.nextLine();
            size_t i = 0;
            const size_t lastIndex = dataHolder.getData().size() - 1;
            for (auto &&[K, V]: dataHolder.getData()) {
                (self << '"' << K << '"').colonSpace().autoAppend(V);
                if (i++ != lastIndex) self.commaNextLine();
            }
        }
        return self.nextLine() << '}';
    }

    SmartPrinter &operator()(this SmartPrinter &self, const ArrayDataHolder &dataHolder) {
        self << '[';
        {
            indentGuard guard{self};
            self.nextLine();
            for (size_t i = 0; i < dataHolder.getData().size() - 1; i++) {
                self.autoAppend(dataHolder.getData()[i]);
                self.commaNextLine();
            }
            self.autoAppend(dataHolder.getData().back());
        }

        return self.nextLine() << ']';
    }

    SmartPrinter &operator()(this SmartPrinter &self, const NumberDataHolder &dataHolder) {
        return self << dataHolder.getData();
    }

    SmartPrinter &operator()(this SmartPrinter &self, const BoolDataHolder &dataHolder) {
        self.ss << std::boolalpha << dataHolder.getData();
        return self;
    }

    SmartPrinter &operator()(this SmartPrinter &self, const NullDataHolder &dataHolder) {
        return self << "null";
    }

    SmartPrinter &operator()(this SmartPrinter &self, const StringDataHolder &dataHolder) {
        static const std::map<char, std::string> escapeMap = {
                {'\n', "\\n"},
                {'\r', "\\r"},
                {'\t', "\\t"},
                {'\b', "\\b"},
                {'\f', "\\f"},
                {'"',  "\\\""}
        };

        self << '"';
        for (size_t i = 0; i < dataHolder.getData().size(); i++) {
            if (escapeMap.find(dataHolder.getData()[i]) != escapeMap.end()) {
                self << escapeMap.at(dataHolder.getData()[i]);
            } else {
                self << dataHolder.getData()[i];
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
