#include <tundra/test-framework/test-runner.hpp>

#include <tundra/gte/initialize.hpp>

#include <psxgpu.h>

#include "base-tests.hpp"
#include "tests/gte/operations.tests.hpp"
#include "tests/gte/compute-transform.tests.hpp"

#include "tests/playstation-time.tests.hpp"

#include "tundra/core/fixed.hpp"
#include "tundra/core/log.hpp"
#include "tundra/playstation-time.hpp"


int main() {

    ResetGraph(0);

    td::gte::initialize();

    return td::TestRunner::run();

    return 0;
}