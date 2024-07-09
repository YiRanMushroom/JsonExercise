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
        [[nodiscard]] JsonType getType() const {
            return type;
        }

        JsonContext() = delete;

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

        DataVariant &get() {
            return data;
        }

        [[nodiscard]] const DataVariant &get() const {
            return data;
        }

        virtual JsonContext getContext() const {
            throw std::logic_error("getContext() is not implemented for base class");
        }

        virtual ~JsonNode() {

        };

        [[nodiscard]] std::string toString() const;

        void set(const std::shared_ptr<JsonNode> &);

        void set(std::shared_ptr<JsonNode> &&);

        [[nodiscard]] virtual JsonNode deepCopy() const {
            throw std::logic_error("deepCopy() is not implemented for base class");
        }

        JsonNode() = default;

        JsonNode(const JsonNode &);

        JsonNode &operator=(const JsonNode &);

        JsonNode(JsonNode &&);

        JsonNode &operator=(JsonNode &&);

    protected:

    };

#define DeclareConsAndAssign(Type) \
    Type(const JsonNode& v) : JsonNode(v){} \
    Type(JsonNode&& v) : JsonNode(std::move(v)){} \
    JsonNode& operator= (const JsonNode& v){    \
        return (JsonNode&) (*this) = v;\
    }                              \
    JsonNode& operator= (JsonNode&& v){    \
        return (JsonNode&) (*this) = std::move(v); \
    }

    class ObjectNode : public JsonNode {
    public:
        DeclareConsAndAssign(ObjectNode)
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

        JsonNode deepCopy() const override;
    };

    class ArrayNode : public JsonNode {
    public:
        DeclareConsAndAssign(ArrayNode)
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

        JsonNode deepCopy() const override;
    };

    class StringNode : public JsonNode {
    public:
        DeclareConsAndAssign(StringNode)
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

        JsonNode deepCopy() const override;
    };

    class NumberNode : public JsonNode {
    public:
        DeclareConsAndAssign(NumberNode)
        using dataType = Number;

        JsonContext getContext() const override {
            return JsonContext{JsonType::NUMBER};
        }

        explicit NumberNode(const Number data) {
            this->data = data;
        }

        JsonNode deepCopy() const override;
    };

    class BoolNode : public JsonNode {
    public:
        DeclareConsAndAssign(BoolNode)
        using dataType = Bool;

        JsonContext getContext() const override {
            return JsonContext{JsonType::BOOL};
        }

        explicit BoolNode(const Bool data) {
            this->data = data;
        }

        JsonNode deepCopy() const override;
    };

    class NullNode : public JsonNode {
    public:
        DeclareConsAndAssign(NullNode)
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

        JsonNode deepCopy() const override;
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