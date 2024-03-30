#include "initialize-prim.hpp"

#include <psxgpu.h>

template<>
void initialize_prim<POLY_F3>(POLY_F3* prim) {
    setPolyF3(prim);
}

template<>
void initialize_prim<POLY_G3>(POLY_G3* prim) {
    setPolyG3(prim);
}

template<>
void initialize_prim<POLY_F4>(POLY_F4* prim) {
    setPolyF4(prim);
}

template<>
void initialize_prim<POLY_FT4>(POLY_FT4* prim) {
    setPolyFT4(prim);
}