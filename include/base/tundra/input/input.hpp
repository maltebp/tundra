#pragma once

#include <tundra/input/icontroller.hpp>

namespace td {

    class Input {
    public:

        Input(IController& controller_1, IController& controller_2)
            : controller_1(controller_1), controller_2(controller_2)
        { }

        IController& controller_1;
        IController& controller_2;
        
    };

}