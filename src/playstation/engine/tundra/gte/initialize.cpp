#include "tundra/gte/initialize.hpp"

#include <psxgte.h>

namespace {
    bool gte_is_initialized = false;
}

void td::gte::initialize() {
    if( !gte_is_initialized ) {
        gte_is_initialized = true;
        InitGeom();
    }
}

bool td::gte::is_initialized() {
    return gte_is_initialized;
}