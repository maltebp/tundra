#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/string.hpp>

namespace td {

    enum class Button : uint16 {

        // The values matches those used by hardware
        
        Triangle	= 1 << 12,
	    Circle		= 1 << 13,
	    Cross		= 1 << 14,
	    Square		= 1 << 15,
	    
        Up			= 1 << 4,
	    Right		= 1 << 5,
	    Down		= 1 << 6,
	    Left		= 1 << 7,

	    L1			= 1 << 10,
	    R1			= 1 << 11,
        L2			= 1 << 8,
	    R2			= 1 << 9,
        L3			= 1 << 1,
	    R3			= 1 << 2,

        Select		= 1 << 0,
	    Start		= 1 << 3

    }; 

    template<>
    String to_string<Button>(const Button& button);

}