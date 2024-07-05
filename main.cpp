#include <iostream>
#include "modules/Json.hpp"

int main() {
    std::cout << "Please enter the path to the file, relative to the current directory: ";
    std::string path;
    JsonBuilder builder;
    std::cin >> path;
    try {
        builder.setString(readFileIntoString(path));
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    JsonNode node;

    try {
        node = builder.build();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::cout << node.toString();
}