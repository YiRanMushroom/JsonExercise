#include <iostream>
#include "modules/Json.hpp"

int main() {
    using namespace n_Json;
    using namespace n_BuilderHelper;
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

    std::shared_ptr<JsonNode> node;

    node = builder.build();

    node->getData<Object>()["Hello"]->set(node);

    ObjectNode objectNode(*node);

    std::cout << objectNode.toString() << std::endl;
}
