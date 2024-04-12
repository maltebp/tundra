#pragma once

#include <tundra/input/playstation-controller.hpp>

namespace td::playstation_input {

    void initialize();

    bool is_initialized();

    void update_controllers(td::PlayStationController& controller_1, PlayStationController& controller_2);

}