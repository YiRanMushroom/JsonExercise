#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <functional>
#include "JsonForwardDeclarations.hpp"

namespace n_BuilderHelper {
    class BuilderHelper {
        friend class n_Json::JsonBuilder;

    public:
        explicit BuilderHelper(const std::string &string) : shardString(std::make_shared<std::string>(string)) {}

        explicit BuilderHelper(std::string &&string) : shardString(std::make_shared<std::string>(std::move(string))) {}

    private:
        ExpectType expect();

        template<typename T>
        T read() {
            throw std::exception("Type not supported");
        };

        template<typename NodeType = void>
        void next(const std::function<void(std::shared_ptr<n_Json::JsonNode> &&)> &provider) {
            provider(std::make_shared<NodeType>(this->read<typename NodeType::dataType>()));
        }

        void skip();

        static char getEscape(char c);

        void ready();

        void nextChar();

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
    double BuilderHelper::read<double>();

    template<>
    bool BuilderHelper::read<bool>();

    template<>
    nullptr_t BuilderHelper::read<nullptr_t>();

    template<>
    std::string BuilderHelper::read<std::string>();

    template<>
    n_Json::Object BuilderHelper::read<n_Json::Object>();

    template<>
    n_Json::Array BuilderHelper::read<n_Json::Array>();

    template<>
    void n_BuilderHelper::BuilderHelper::next<void>(
            const std::function<void(std::shared_ptr<n_Json::JsonNode> &&)> &provider);

    std::string readFileIntoString(const std::string &filename);
}