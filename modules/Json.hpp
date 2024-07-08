#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include <variant>
#include <iostream>
#include "JsonForwardDeclarations.hpp"
#include "BuilderHelper.hpp"

namespace n_Json {

    struct JsonContext {
    private:
        JsonType type;

    public:
        JsonType getType() const {
            return type;
        }

        JsonContext() : type(JsonType::UNINITIALIZED) {}

        explicit JsonContext(const JsonType type) : type(type) {}
    };

    class JsonNode {
    protected:
        DataVariant data;

    public:
        using dataType = void;

        template<typename T>
        decltype(auto) getData(this auto &&self) {
            return std::get<T>(self.data);
        }

        JsonNode(const JsonNode &) = default;

        JsonNode &operator=(const JsonNode &) = default;

        JsonNode(JsonNode &&) = default;

        JsonNode &operator=(JsonNode &&) = default;

        virtual JsonContext getContext() const = 0;

        virtual ~JsonNode() = default;

        [[nodiscard]] std::string toString() const;

    protected:
        JsonNode() = default;
    };

    class ObjectNode : public JsonNode {
    public:
        using dataType = Object;

        JsonContext getContext() const override {
            return JsonContext{JsonType::OBJECT};
        }

        explicit ObjectNode(const Object &data) {
            this->data = data;
        }

        explicit ObjectNode(Object &&data) {
            this->data = std::move(data);
        }
    };

    class ArrayNode : public JsonNode {
    public:
        using dataType = Array;

        JsonContext getContext() const override {
            return JsonContext{JsonType::ARRAY};
        }

        explicit ArrayNode(const Array &data) {
            this->data = data;
        }

        explicit ArrayNode(Array &&data) {
            this->data = std::move(data);
        }
    };

    class StringNode : public JsonNode {
    public:
        using dataType = std::string;

        JsonContext getContext() const override {
            return JsonContext{JsonType::STRING};
        }

        explicit StringNode(const String &data) {
            this->data = data;
        }

        explicit StringNode(String &&data) {
            this->data = std::move(data);
        }
    };

    class NumberNode : public JsonNode {
    public:
        using dataType = Number;

        JsonContext getContext() const override {
            return JsonContext{JsonType::NUMBER};
        }

        explicit NumberNode(const Number data) {
            this->data = data;
        }
    };

    class BoolNode : public JsonNode {
    public:
        using dataType = Bool;

        JsonContext getContext() const override {
            return JsonContext{JsonType::BOOL};
        }

        explicit BoolNode(const Bool data) {
            this->data = data;
        }
    };

    class NullNode : public JsonNode {
    public:
        using dataType = NullPtr;

        JsonContext getContext() const override {
            return JsonContext{JsonType::NULLPTR};
        }

        explicit NullNode(const NullPtr data) {
            this->data = data;
        }

        NullNode() {
            this->data = nullptr;
        }
    };


    class JsonBuilder {
    private:
        std::unique_ptr<n_BuilderHelper::BuilderHelper> helper;
    public:
        JsonBuilder() = default;

        JsonBuilder &setString(const std::string &string);

        JsonBuilder &setString(std::string &&string);

        std::shared_ptr<JsonNode> build();
    };
}