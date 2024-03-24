#include <tundra/test-framework/test-runner.hpp>

#include <tundra/gte/initialize.hpp>

#include "base-tests.hpp"

#include "tests/gte/operations.tests.hpp"
#include "tests/gte/compute-transform.tests.hpp"


int main() {

    td::gte::initialize();

    return td::TestRunner::run();
}