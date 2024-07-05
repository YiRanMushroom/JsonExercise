#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include <variant>
#include <iostream>
#include "BuilderHelper.hpp"

namespace n_Json {

    class JsonNode;

    using Map = std::map<std::string, std::shared_ptr<JsonNode>>;
    using Array = std::vector<std::shared_ptr<JsonNode>>;
    using Number = double;
    using String = std::string;
    using NullPtr = nullptr_t;
    using Bool = bool;

    using DataVariant = std::variant<String, Map, Array, Number, Bool, NullPtr>;

    enum class JsonType {
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOL,
        NULLPTR,
        UNINITIALIZED
    };

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

/*    class JsonContexts {
    private:
        JsonContexts() = default;

        const JsonContext objectContext = JsonContext{JsonType::OBJECT};
        const JsonContext arrayContext = JsonContext{JsonType::ARRAY};
        const JsonContext stringContext = JsonContext{JsonType::STRING};
        const JsonContext numberContext = JsonContext{JsonType::NUMBER};
        const JsonContext boolContext = JsonContext{JsonType::BOOL};
        const JsonContext nullptrContext = JsonContext{JsonType::NULLPTR};

    public:
        static JsonContexts& getInstance() {
            static JsonContexts instance;
            return instance;
        }

        [[nodiscard]] const JsonContext& getObjectContext() const {
            return objectContext;
        }

        [[nodiscard]] const JsonContext& getArrayContext() const {
            return arrayContext;
        }

        [[nodiscard]] const JsonContext& getStringContext() const {
            return stringContext;
        }

        [[nodiscard]] const JsonContext& getNumberContext() const {
            return numberContext;
        }

        [[nodiscard]] const JsonContext& getBoolContext() const {
            return boolContext;
        }

        [[nodiscard]] const JsonContext& getNullptrContext() const {
            return nullptrContext;
        }
    };*/

    class JsonNode {
    protected:
        DataVariant data;

    public:
        template<typename T>
        decltype(auto) getData(this auto &&self) {
            return std::get<T>(self.data);
        }

        JsonNode(const JsonNode &) = default;

        JsonNode &operator=(const JsonNode &) = default;

        JsonNode(JsonNode &&) = default;

        JsonNode &operator=(JsonNode &&) = default;

        JsonNode() = default;

        virtual JsonContext getContext() const {
            std::unreachable();
        }

        virtual ~JsonNode() = default;

        [[nodiscard]] std::string toString() const;
    };

    class ObjectNode : public JsonNode {
    public:
        JsonContext getContext() const override {
            return JsonContext{JsonType::OBJECT};
        }

        explicit ObjectNode(const Map &data) {
            this->data = data;
        }

        explicit ObjectNode(Map &&data) {
            this->data = std::move(data);
        }
    };

    class ArrayNode : public JsonNode {
    public:
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
        JsonContext getContext() const override {
            return JsonContext{JsonType::NUMBER};
        }

        explicit NumberNode(const Number data) {
            this->data = data;
        }
    };

    class BoolNode : public JsonNode {
    public:
        JsonContext getContext() const override {
            return JsonContext{JsonType::BOOL};
        }

        explicit BoolNode(const Bool data) {
            this->data = data;
        }
    };

    class NullNode : public JsonNode {
    public:
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

        std::shared_ptr<JsonNode> nextObject();

        std::shared_ptr<JsonNode> nextArray();
    };
}