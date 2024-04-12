#include <cstdio>

#include <psxgpu.h>

#include <tundra/core/types.hpp>
#include <tundra/core/log.hpp>

#include <tundra/input/button.hpp>
#include <tundra/input/input.hpp>
#include <tundra/input/icontroller.hpp>
#include <tundra/input/playstation-input.hpp>
#include <tundra/input/playstation-controller.hpp>

#include <tundra/gte/initialize.hpp>


const td::uint32 PRINT_FREQUENCY = 150; // Frames


void print_controller_if_change(td::String name, td::PlayStationController& controller, bool previously_active) {

    if( controller.is_active() != previously_active ) {
        TD_DEBUG_LOG("%s: %s", name, (controller.is_active() ? "Activated" : "Deactivated"));
    }

    if(!controller.is_active() ) return;

    bool any_button_changed = controller.current_button_states != controller.previous_button_states;
    if( any_button_changed ) {
        uint16_t current_bit = 1;
        for( td::uint32 i = 0; i < 16; i++, current_bit <<= 1 ) {
            td::Button button = (td::Button)current_bit; 

            if( controller.is_pressed_this_frame(button ) ) {
                TD_DEBUG_LOG("%s: %s was pressed", name, button);
            }

            if( controller.is_released_this_frame(button ) ) {
                TD_DEBUG_LOG("%s: %s was released", name, button);
            }
        }
    }   
}

void print_controller_status(td::IController& controller_1, td::IController& controller_2) {
    auto print_with_padding = [](const td::String& s, td::uint32 length) {
        std::printf("%s", s.get_c_string());

        td::int32 padding = ((td::int32)length) - (td::int32)s.get_size();
        for( td::int32 i = 0; i < padding; i++ ) {
            std::printf(" ");
        }
    };

    std::printf("\n");
    print_with_padding("", 10);
    print_with_padding("Controller 1", 14);
    print_with_padding("Controller 2", 0);
    std::printf("\n");

    uint16_t current_bit = 1;
    for( td::uint32 i = 0; i < 16; i++, current_bit <<= 1 ) {
        td::Button button = (td::Button)(current_bit);

        print_with_padding(td::to_string(button), 10);
        print_with_padding((controller_1.is_pressed(button) ? "Pressed" : (controller_1.is_released(button) ? "Released" : "Inactive")), 14);
        print_with_padding((controller_2.is_pressed(button) ? "Pressed" : (controller_2.is_released(button) ? "Released" : "Inactive")), 0);
        std::printf("\n");
    }
    
    std::printf("\n");
}

int main() {

    TD_DEBUG_LOG("Initializing input"); 

    ResetGraph(0);
    td::gte::initialize();   

    td::playstation_input::initialize();
    
    td::PlayStationController controller_1;
    td::PlayStationController controller_2;
    td::playstation_input::update_controllers(controller_1, controller_2);

    td::Input input{controller_1, controller_2};

    td::uint32 print_cooldown = PRINT_FREQUENCY;
    
    bool controller_1_was_active_last_frame = false;
    bool controller_2_was_active_last_frame = false;

    while(true) {

        td::playstation_input::update_controllers(controller_1, controller_2);

        print_controller_if_change("Controller 1", controller_1, controller_1_was_active_last_frame);
        print_controller_if_change("Controller 2", controller_2, controller_2_was_active_last_frame);

        controller_1_was_active_last_frame = controller_1.is_active();
        controller_2_was_active_last_frame = controller_2.is_active();

        print_cooldown--;
        if( print_cooldown == 0 ) {
            print_cooldown = PRINT_FREQUENCY;

            print_controller_status(controller_1, controller_2);
        }

        VSync(0);
    }

    return 0;
}