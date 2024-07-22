#pragma once

#include <variant>
#include "JsonForwardHeader.hpp"

namespace Json {
    class Json {
    private:
        std::variant<String, Object, Array, Number, Bool, NullPtr> data;

    public:
        template<class T>
        [[maybe_unused]] decltype(auto) get(this auto &&self) {
            return std::get<T>(self.data);
        }

        decltype(auto) visit(this auto &&self, auto &&visitor) {
            return std::visit(visitor, self.data);
        } // using this is recommended

        [[nodiscard, maybe_unused]] std::variant<String, Object, Array, Number,
                Bool, NullPtr> &getData() {
            return data;
        }

        [[nodiscard, maybe_unused]] DataType what() const {
            constexpr struct {
                DataType operator()(const String &) const { return DataType::STRING; }

                DataType operator()(const Object &) const { return DataType::OBJECT; }

                DataType operator()(const Array &) const { return DataType::ARRAY; }

                DataType operator()(const Number &) const { return DataType::NUMBER; }

                DataType operator()(const Bool &) const { return DataType::BOOL; }

                DataType operator()(const NullPtr &) const { return DataType::NULLPTR; }
            } visitor;

            return std::visit(visitor, data);
        }

        Json() : data(nullptr) {};

        // Now declare the common constructors, we want copy and move constructors, also assign operators
        Json(const Json &other) = default;

        Json(Json &&other) noexcept = default;

        Json &operator=(const Json &other) {
            data = Json{other}.data;
            return *this;
        }

        Json &operator=(Json &&other) noexcept = default;

        Json copy(this const Json self) {
            return self;
        }

        ~Json() = default;

        // Now declare the constructors, we want copy and move constructors for non-trivial types

        Json(const std::string &str) : data(str) {}

        Json(std::string &&str) : data(std::move(str)) {}

        Json(const char *str) : data(std::string(str)) {}

        Json(const Object &obj) : data(obj) {}

        Json(Object &&obj) : data(std::move(obj)) {}

        Json(const Array &arr) : data(arr) {}

        Json(Array &&arr) : data(std::move(arr)) {}

        Json(const Number num) : data(num) {}

        Json(const Bool b) : data(b) {}

        [[nodiscard]] std::string toString();

        operator std::string() {
            return this->toString();
        }
    };

    template<typename T>
    class [[maybe_unused]] IJsonClass {
    public:
        virtual T serialize(const Json &) = 0;

        virtual Json deserialize() = 0;

        virtual ~IJsonClass() = default;
    };
}

#define JsonClass(T) \
                     \
public:              \
T(const Json::Json& json) {\
    this->serialize(json);\
}