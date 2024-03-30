
#include <psxgpu.h>

#include <tundra/core/log.hpp>
#include <tundra/gte/initialize.hpp>

int main() {

    ResetGraph(0);
    td::gte::initialize();

    TD_DEBUG_LOG("Hello, from Model Rendering test");

    return 0; 
}