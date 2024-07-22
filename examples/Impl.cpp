#include "student.hpp"

Student Student::serialize(const Json::Json &json) {
    const Json::Object &data = json.get<Json::Object>();
    this->name = data.at("name").get<Json::String>();
    this->age = data.at("age").get<Json::Number>();
    if (data.contains("additional_information"))
        this->additionalInformation = data.at("additional_information");
    else this->additionalInformation = Json::Json();
    return *this;
}

Json::Json Student::deserialize() {
    auto returnVal = Json::Json{
            Json::Object{
                    {
                            "name", this->name
                    },
                    {
                            "age",  (Json::Number) this->age
                    }
            }
    };

    if (!holds_alternative<Json::NullPtr>(this->additionalInformation.getData()))
        returnVal.get<Json::Object>()["additional_information"] = this->additionalInformation;

    return returnVal;
}
