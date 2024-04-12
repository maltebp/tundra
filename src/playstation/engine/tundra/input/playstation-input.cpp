#include "tundra/input/playstation-controller.hpp"
#include <tundra/input/playstation-input.hpp>

// Disabling warning for anonymous structs (they are a GNU extension, but we always compile this
// with GCC, so it shouldn't be a problem)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <psxpad.h>
#include <psxapi.h>
#pragma GCC diagnostic pop

#include <tundra/core/types.hpp>
#include <tundra/core/assert.hpp>

namespace td {

    namespace playstation_input::internal {
        // I do not know why it has to be 34 specifically, but the examples are doing it
        // The object that it seems to hold (PADTYPE) is only 19 bytes
        const uint32 BUFFER_LENGTH = 34;

        static byte pad_buffer_1[BUFFER_LENGTH];
        static byte pad_buffer_2[BUFFER_LENGTH];

        static bool is_initialized = false;

        bool pad_is_active_controller(const PADTYPE* pad) {
            if( pad->stat != 0 ) return false;
            return
                    pad->type == PadTypeID::PAD_ID_DIGITAL 
                ||  pad->type == PadTypeID::PAD_ID_ANALOG 
                ||  pad->type == PadTypeID::PAD_ID_ANALOG_STICK;
        }

        void update_controller(const byte* pad_buffer, td::PlayStationController& controller) {

            const PADTYPE* pad_type = reinterpret_cast<const PADTYPE*>(pad_buffer);

            controller.previous_button_states = controller.current_button_states;
            
            if( pad_is_active_controller(pad_type) ) {
                controller.m_is_active = true;
                controller.current_button_states = pad_type->btn;
            }
            else {
                controller.m_is_active = false;
                controller.current_button_states = 0xFFFF;                
            }
        };

    }

    void playstation_input::initialize() {
        TD_ASSERT(!internal::is_initialized, "PlayStation input is already initialized");
        internal::is_initialized = true;
    
        InitPAD(internal::pad_buffer_1, internal::BUFFER_LENGTH, internal::pad_buffer_2, internal::BUFFER_LENGTH);
        StartPAD();

        // Make input driver not acknowledge V-Blank IRQ
        ChangeClearPAD(0); 
    }

    bool playstation_input::is_initialized() {
        return internal::is_initialized;
    }

    void playstation_input::update_controllers(td::PlayStationController& controller_1, PlayStationController& controller_2) {
        TD_ASSERT(internal::is_initialized, "PlayStation input is not initialized");
        internal::update_controller(internal::pad_buffer_1, controller_1);
        internal::update_controller(internal::pad_buffer_2, controller_2);
    }

}