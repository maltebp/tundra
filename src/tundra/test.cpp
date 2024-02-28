#include <cstdio>

#include "tundra/test.hpp"


namespace td {

    void hello_world(const char* sender_name) {
        std::printf("Hello, world - from %s\n", sender_name);
    }

    void Test::hello_world(const char* sender_name) const {
        std::printf("Hello, world - from %s\n", sender_name);
    }

}