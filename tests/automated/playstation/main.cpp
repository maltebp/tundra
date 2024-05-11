#include <tundra/test-framework/test-runner.hpp>

#include <tundra/gte/initialize.hpp>

#include <psxgpu.h>

#include "base-tests.hpp"

#include "tests/playstation-time.tests.hpp"

#include "tests/gte/operations.tests.hpp"
#include "tests/gte/compute-transform.tests.hpp"
#include "tests/rendering/camera.tests.hpp"




int main() {
    TD_ASSERT(false, "This should not run");

    ResetGraph(0);

    td::gte::initialize(); 

    return td::TestRunner::run();

    return 0;
}