#include <tundra/test-framework/test-runner.hpp>

#include <tundra/gte/initialize.hpp>

#include <psxgpu.h>

#include "base-tests.hpp"

#include "tests/playstation-time.tests.hpp"

#include "tests/gte/operations.tests.hpp"
#include "tests/gte/compute-transform.tests.hpp"
#include "tests/rendering/camera.tests.hpp"
#include "tundra/engine/entity-system/internal/registry-block.dec.hpp"
#include "tundra/engine/transform.hpp"


int main() {
    ResetGraph(0);

    td::gte::initialize(); 
    td::TestRunner::run();

    TD_DEBUG_LOG("RegistryBlock size: %d\n", sizeof(td::internal::RegistryBlock<td::Transform>));

    return 0;
}