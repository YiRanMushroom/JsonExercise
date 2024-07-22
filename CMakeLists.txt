cmake_minimum_required(VERSION 3.28)
project(JsonExercise)

set(CMAKE_CXX_STANDARD 23)

add_executable(JsonExercise main.cpp
#        modules/deprecated/BuilderHelper.cpp
#        modules/deprecated/BuilderHelper.hpp
#        modules/deprecated/Json.cpp
#        modules/deprecated/Json.hpp
#        modules/deprecated/JsonApi.hpp
#        modules/deprecated/JsonForwardDeclarations.hpp
        modules/JsonForwardHeader.hpp
        modules/Json.cpp
        modules/Json.hpp
        modules/JsonImpl.cpp
        examples/student.hpp
        examples/Impl.cpp
        examples/test_example.hpp)
