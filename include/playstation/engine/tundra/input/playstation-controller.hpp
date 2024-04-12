#pragma once

#include <tundra/input/button.hpp>
#include <tundra/input/icontroller.hpp>

namespace td {

    class PlayStationController : public IController {
    public:

        virtual bool is_active() const override {
            return m_is_active;
        }
    
        // True if button is pressed (no matter when)
        virtual bool is_pressed(Button button) const override {
            return m_is_active && !(current_button_states & (uint16)button);
        }

        // True if button is pressed this frame but not last frame
        virtual bool is_pressed_this_frame(Button button) const override {
            return m_is_active && !(current_button_states & (uint16)button) && (previous_button_states & (uint16)button);
        }

        // True if button is pressed (no matter when)
        virtual bool is_released(Button button) const override {
            return m_is_active && (current_button_states & (uint16)button);
        }

        // True if button is released this frame but not last frame
        virtual bool is_released_this_frame(Button button) const override {
            return m_is_active && current_button_states & (uint16)button && !(previous_button_states & (uint16)button);
        }

        bool m_is_active = false;

        uint16 current_button_states = 0xFFFF;
        uint16 previous_button_states = 0xFFFF;
    };

}