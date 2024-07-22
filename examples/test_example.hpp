#include "student.hpp"

inline void testWrite() {
    Student student{"Yiran", 19,
                    Json::Object{
                            {
                                    "who",         "Best CPEN Student on Earth"
                            },
                            {
                                    "github_page", "https://github.com/YiRanMushroom"
                            },
                            {
                                    "code_in",
                                                   Json::Array
                                                           {
                                                                   "C++",
                                                                   "Java",
                                                                   "Kotlin"
                                                           }
                            }}};
    std::cout << student.deserialize().toString();
}