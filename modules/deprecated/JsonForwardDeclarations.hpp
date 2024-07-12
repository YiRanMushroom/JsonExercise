#pragma once

#include <map>
#include <variant>
#include <vector>

namespace n_Json {
    class JsonNode;

    class JsonBuilder;

    using Object = std::map<std::string, std::shared_ptr<JsonNode>>;
    using Array = std::vector<std::shared_ptr<JsonNode>>;
    using Number = double;
    using String = std::string;
    using NullPtr = nullptr_t;
    using Bool = bool;

    using DataVariant = std::variant<String, Object, Array, Number, Bool, NullPtr>;

    enum class JsonType {
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOL,
        NULLPTR,
        UNINITIALIZED
    };
}

namespace n_BuilderHelper {
    class BuilderHelper;

    enum class ExpectType {
        OBJECT_Start,
        OBJECT_End,
        ARRAY_Start,
        ARRAY_End,
        STRING,
        NUMBER,
        BOOL,
        NULLPTR
    };
}