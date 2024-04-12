#include "tundra/core/assert.hpp"
#include <tundra/input/button.hpp>

namespace td {
    
    template<>
    String to_string<Button>(const Button& button) {

        switch(button) {
            case Button::Triangle: return "Triangle";
            case Button::Circle: return "Circle";
            case Button::Cross: return "Cross";
            case Button::Square: return "Square";
            case Button::Up: return "Up";
            case Button::Right: return "Right";
            case Button::Down: return "Down";
            case Button::Left: return "Left";
            case Button::L1: return "L1";
            case Button::R1: return "R1";
            case Button::L2: return "L2";
            case Button::R2: return "R2";
            case Button::L3: return "L3";
            case Button::R3: return "R3";
            case Button::Select: return "Select";
            case Button::Start: return "Start";
        }

        TD_ASSERT(false, "Unknown Button enum value %d (internal error)", (int)button);
        return "";
    }

}