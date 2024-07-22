#include "../modules/Json.hpp"

class Student : public Json::IJsonClass<Student> {
JsonClass(Student)

    std::string name;
    unsigned int age;
    Json::Json additionalInformation;

    Student(std::string_view name, unsigned int age, Json::Json &&additionalInformation)
            : name(name), age(age), additionalInformation(std::move(additionalInformation)) {}

    Student(std::string_view name, unsigned int age, const Json::Json &additionalInformation)
            : Student(name, age, additionalInformation.copy()) {}

    Student() = default;

    Student(std::string_view name, unsigned int age) : name(name), age(age) {}

    Student serialize(const Json::Json &json) override;

    Json::Json deserialize() override;
};

