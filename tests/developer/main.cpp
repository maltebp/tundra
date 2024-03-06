#include <tundra/test-framework/test-runner.hpp>

#include "base-tests.hpp"
#include "tests/test-test.hpp"

int main() {
    return td::TestRunner::run();
}