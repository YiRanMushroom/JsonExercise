#include <iostream>
#include "modules/Json.hpp"
#include "examples/test_example.hpp"

int main() {
    testWrite();
    /*std::string fileName;
    std::cout << "Enter the file name: ";
    std::cin >> fileName;
    Json::Json json = Json::parseJsonFromFile(fileName);

    Json::Json cpy = json;

    json.get<Json::Object>()["Hello"] = json;

    std::cout << json.toString();*/
}
