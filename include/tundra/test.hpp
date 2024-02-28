#pragma once

namespace td {
    void hello_world(const char* sender_name);

    class Test {
    public:
        void hello_world(const char* sender_name) const;
    };
}