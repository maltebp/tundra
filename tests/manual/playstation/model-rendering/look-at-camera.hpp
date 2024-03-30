#pragma once

#include <psxgte.h>

class LookAtCamera {
public:

    void set_target(VECTOR target);

    void set_position(VECTOR position);

    const VECTOR& get_position() const { return position; }

    MATRIX compute_matrix() const;    

private:

    VECTOR position = {};
    
    VECTOR target = {0, 0, ONE};

};