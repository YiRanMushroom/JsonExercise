#pragma once

#include <variant>
#include "JsonForwardHeader.hpp"

namespace Json {
    class Json {
    private:
        std::variant<String, Object, Array, Number, Bool, NullPtr> data;

    public:
        template<class T>
        decltype(auto) get(this auto &&self) {
            return std::get<T>(self.data);
        }

        decltype(auto) visit(this auto &&self, auto&& visitor) {
            return std::visit(visitor, self.data);
        }

/*        [[nodiscard]] const std::variant<String, Object, Array, Number,
                Bool, NullPtr> &getData() const {
            return data;
        }*/

        [[nodiscard]] DataType what() const {
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

        Json(Json &&other) = default;

        Json &operator=(const Json &other) = default;

        Json &operator=(Json &&other) = default;

        // Now declare the constructors, we want copy and move constructors for non-trivial types

        explicit Json(const std::string &str) : data(str) {}

        explicit Json(std::string &&str) : data(std::move(str)) {}

        explicit Json(const Object &obj) : data(obj) {}

        explicit Json(Object &&obj) : data(std::move(obj)) {}

        explicit Json(const Array &arr) : data(arr) {}

        explicit Json(Array &&arr) : data(std::move(arr)) {}

        explicit Json(const Number num) : data(num) {}

        explicit Json(const Bool b) : data(b) {}

        [[nodiscard]] std::string deserialize();
    };
}
