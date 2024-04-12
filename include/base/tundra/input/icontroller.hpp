#pragma once

#include <tundra/input/button.hpp>

namespace td {

    class IController {
    public:

        [[nodiscard]] virtual bool is_active() const;
    
        // True if button is pressed (no matter when).
        // Returns false if controller is inactive
        [[nodiscard]] virtual bool is_pressed(Button button) const;

        // True if button is pressed this frame but not last frame.
        // Returns false if controller is inactive
        [[nodiscard]] virtual bool is_pressed_this_frame(Button button) const;

        // True if button is pressed (no matter when).
        // Returns false if controller is inactive
        [[nodiscard]] virtual bool is_released(Button button) const;

        // True if button is released this frame but not last frame.
        // Returns false if controller is inactive
        [[nodiscard]] virtual bool is_released_this_frame(Button button) const;

    };

}