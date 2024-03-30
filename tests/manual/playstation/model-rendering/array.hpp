#pragma once

#include <stddef.h>

#include <tundra/core/assert.hpp>


template<typename T>
class Array {
public:

    Array(size_t size) 
        :   elements(new T[size]),
            size(size)
    { 
        TD_ASSERT(elements != nullptr, "Failed to allocate array");
    }

    ~Array() {
        delete[] elements;
    }

    Array(const Array&) = delete;

    Array(const Array&&) = delete;

    void default_initialize() {
        for( int i = 0; i < size; i++ ) {
            elements[i] = {};
        }
    }

    size_t get_size() const { return size; }

    T* get_data() { return elements; }

    T* get_data_end() { return elements + size; }

    T& get_last() { return elements[size - 1]; }

    T& get_first() { return elements[0]; }

    T& operator [](size_t index) {
        TD_ASSERT(index < size, "Array index must be less than its size (was %d)", index);
        return elements[index];
    }

    const T& operator [](size_t index) const {
        TD_ASSERT(index < size, "Array index must be less than its size (was %d)", index);
        return elements[index];
    }

private:

    T* const elements;

    const size_t size;

};