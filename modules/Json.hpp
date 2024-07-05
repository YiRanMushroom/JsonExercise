#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include "BuilderHelper.hpp"

class JsonNode;

using Map = std::map<std::string, JsonNode>;

enum class JsonType {
    OBJECT,
    ARRAY,
    STRING,
    NUMBER,
    BOOL,
    NULLPTR,
    UNINITIALIZED
};

class DataHolder {
};

class JsonNode {
public:
    JsonNode() : type(JsonType::UNINITIALIZED), data(nullptr) {}

    JsonType type;
    std::shared_ptr<DataHolder> data;

    JsonNode(JsonType type, const std::shared_ptr<DataHolder> &data) : type(type), data(data) {}

    JsonNode(JsonType type, std::shared_ptr<DataHolder> &&data) : type(type), data(std::move(data)) {}

    [[nodiscard]] std::string toString() const;
};

class StringDataHolder : public DataHolder {
private:
    std::string data;
public:
    decltype(auto) getData(this auto &&self) {
        return self.data;
    };

    explicit StringDataHolder(const std::string &data) : data(data) {}

    explicit StringDataHolder(std::string &&data) : data(std::move(data)) {}
};

class NumberDataHolder : public DataHolder {
private:
    double data;
public:
    decltype(auto) getData(this auto &&self) {
        return self.data;
    };

    explicit NumberDataHolder(double data) : data(data) {}
};

class BoolDataHolder : public DataHolder {
private:
    bool data;
public:
    decltype(auto) getData(this auto &&self) {
        return self.data;
    };

    explicit BoolDataHolder(bool data) : data(data) {}
};

class NullDataHolder : public DataHolder {
public:
    [[nodiscard]] static nullptr_t getData() {
        return nullptr;
    };

    NullDataHolder() = default;
};

class ArrayDataHolder : public DataHolder {
private:
    std::vector<JsonNode> data;
public:
    decltype(auto) getData(this auto &&self) {
        return self.data;
    }

    explicit ArrayDataHolder(const std::vector<JsonNode> &data) : data(data) {}

    explicit ArrayDataHolder(std::vector<JsonNode> &&data) : data(std::move(data)) {}
};

class ObjectDataHolder : public DataHolder {
private:
    Map data;
public:
    [[nodiscard]] decltype(auto) getData(this auto &&self) {
        return self.data;
    };

    explicit ObjectDataHolder(const Map &data) : data(data) {}

    explicit ObjectDataHolder(Map &&data) : data(std::move(data)) {}
};

class JsonBuilder {
private:
    std::unique_ptr<BuilderHelper> helper;
public:
    JsonBuilder() = default;

    JsonBuilder &setString(const std::string &string);

    JsonBuilder &setString(std::string &&string);

    JsonNode build();

    JsonNode nextObject();

    JsonNode nextArray();
};