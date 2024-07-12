#pragma once

#include <string>
#include <map>
#include <vector>
#include <typeinfo>

namespace Json {
    class Json;

    using String = std::string;
    using Object = std::map<String, Json>;
    using Array = std::vector<Json>;
    using Number = double;
    using Bool = bool;
    using NullPtr = std::nullptr_t;

    enum class DataType {
        STRING,
        OBJECT,
        ARRAY,
        NUMBER,
        BOOL,
        NULLPTR
    };

    Json parseJson(const std::string &str);
    Json parseJsonFromFile(const std::string &fileName);
}