#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <functional>
#include "JsonForwardDeclarations.hpp"

namespace n_BuilderHelper {
    class BuilderHelper {
    public:
        explicit BuilderHelper(const std::string &string) : shardString(std::make_shared<std::string>(string)) {}

        explicit BuilderHelper(std::string &&string) : shardString(std::make_shared<std::string>(std::move(string))) {}

        ExpectType expect();

        template<typename T>
        void next(const std::function<void(std::shared_ptr<n_Json::JsonNode>&&)> &provider) {
            std::unreachable();
        }

        void skip();

        static char getEscape(char c);

        void ready();

        void nextChar();

        std::string readString();

        [[nodiscard]] const char &now() const;

        std::string &getString() {
            return *shardString;
        }

    private:
        std::shared_ptr<std::string> shardString;

        size_t at = 0;

    public:
        BuilderHelper() = delete;

        BuilderHelper(BuilderHelper &) = default;

    };

    template<>
    void BuilderHelper::next<double>(const std::function<void(std::shared_ptr<n_Json::JsonNode>&&)> &provider);

    template<>
    void BuilderHelper::next<std::string>(const std::function<void(std::shared_ptr<n_Json::JsonNode>&&)> &provider);

    template<>
    void BuilderHelper::next<bool>(const std::function<void(std::shared_ptr<n_Json::JsonNode>&&)> &provider);

    template<>
    void BuilderHelper::next<n_Json::Object>(const std::function<void(std::shared_ptr<n_Json::JsonNode>&&)> &provider);

    template<>
    void BuilderHelper::next<n_Json::Array>(const std::function<void(std::shared_ptr<n_Json::JsonNode>&&)> &provider);

    template<>
    void BuilderHelper::next<std::nullptr_t>(const std::function<void(std::shared_ptr<n_Json::JsonNode>&&)> &provider);

    std::string readFileIntoString(const std::string &filename);
}