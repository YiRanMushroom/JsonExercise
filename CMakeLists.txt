cmake_minimum_required(VERSION 3.28)
project(JsonExercise)

set(CMAKE_CXX_STANDARD 23)

add_executable(JsonExercise main.cpp
        modules/BuilderHelper.cpp
        modules/BuilderHelper.hpp
        modules/Json.cpp
        modules/Json.hpp
        modules/JsonApi.hpp)
