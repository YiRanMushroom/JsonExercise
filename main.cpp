#include <iostream>
#include "modules/Json.hpp"

int main() {
    std::string fileName;
    std::cout << "Enter the file name: ";
    std::cin >> fileName;
    Json::Json json = Json::parseJsonFromFile(fileName);
    std::cout << json.deserialize();
}
