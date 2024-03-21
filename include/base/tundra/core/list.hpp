#pragma once

#include <tundra/core/list.dec.hpp>

#include <cstdlib>

#include <tundra/core/types.hpp>
#include <tundra/core/limits.hpp>
#include <tundra/core/assert.hpp>
#include <tundra/core/log.hpp>
#include <tundra/core/utility.hpp>


// For some reason, it cannot find the placement new
void* operator new(size_t count, void* address) noexcept;

template<typename T>
td::List<T>::List(td::uint32 num_elements) 
    :   elements(num_elements == 0 ? nullptr : (T*)::operator new(sizeof(T) * num_elements)),
        capacity(num_elements),
        size(num_elements)
{
    for( td::uint32 i = 0; i < num_elements; i++ ) {
        new(elements + i) T();
    }
}

template<typename T>
td::List<T>::List(const List& other) 
    :   elements(other.size == 0 ? nullptr : (T*)::operator new(sizeof(T) * other.capacity)),
        capacity(other.capacity),
        size(other.size)
{
    for( td::uint32 i = 0; i < size; i++ ) {
        new(elements + i) T(other.elements[i]);
    }
}

template<typename T>
td::List<T>::List(List&& other) 
    :   elements(other.elements),
        capacity(other.capacity),
        size(other.size)
{
    other.elements = nullptr;
    other.capacity = 0;
    other.size = 0;
}

namespace td {
    template<typename T>
    List<T>::~List() {
        if( elements != nullptr ) {
            for( uint32 i = 0; i < size; i++ ) {
                elements[i].~T();
            }
            ::operator delete ((void*)elements);
        }
    }
}

template<typename T>
void td::List<T>::reserve(td::uint32 capacity_to_reserve) {
    if( capacity_to_reserve < capacity ) return;
    reallocate(capacity_to_reserve);
}

template<typename T>
void td::List<T>::add(const T& element) {
    if( size == capacity ) {
        uint32 new_size = capacity == 0 ? 2 : capacity * 2;
        reallocate(new_size);
    }

    new (elements + size) T(element);
    size++;
}

template<typename T>
void td::List<T>::add(T&& element) {
    if( size == capacity ) {
        uint32 new_size = capacity == 0 ? 2 : capacity * 2;
        reallocate(new_size);
    }

    new (elements + size) T(td::move(element));
    size++;
}

template<typename T>
bool td::List<T>::remove(const T& element) {
    for( uint32 i = 0; i < size; i++ ) {
        if( (*this)[i] == element ) {
            remove_at(i);
            return true;
        }
    }

    return false;
}

template<typename T>
void td::List<T>::remove_at(uint32 index) {
    TD_ASSERT(index < size, "List index out of bounds (was %d but size is %d)", index, size);
    elements[index].~T();

    for( uint32 i = index + 1; i < size; i++ ) {
        new(elements + i - 1) T(td::move(elements[i]));
        elements[i].~T();
    }

    size--;
}

template<typename T>
void td::List<T>::clear() {
    if( elements == nullptr ) return;

    for( uint32 i = 0; i < size; i++ ) {
        elements[i].~T();
    }            

    size = 0;
}

template<typename T>
[[nodiscard]] td::uint32 td::List<T>::index_of(const T& element) const {
    for( uint32 i = 0; i < size; i++ ) {
        if( elements[i] == element ) return i;
    }

    return td::limits::numeric_limits<td::uint32>::max;
}

template<typename T>
td::uint32 td::List<T>::get_size() const { return size; }

template<typename T>
T& td::List<T>::get_last() {
    TD_ASSERT(size > 0, "List is empty when trying to get last");
    return elements[size - 1];
}

template<typename T>
const T& td::List<T>::get_last() const {
    TD_ASSERT(size > 0, "List is empty when trying to get last");
    return elements[size - 1];
}

template<typename T>
td::List<T>& td::List<T>::operator=(const td::List<T>& other) {

    clear();

    if( this->capacity < other.capacity ) {
        reallocate(other.capacity);
    }

    for( td::uint32 i = 0; i < other.size; i++ ) {
        new(elements + i) T(other.elements[i]);
    }

    this->size = other.size;

    return *this;
}

template<typename T>
T& td::List<T>::operator[](uint32 i) {
    TD_ASSERT(i < size, "List index out of bounds (was %d but size is %d)", i, size);
    return elements[i];
}

template<typename T>
const T& td::List<T>::operator[](uint32 i) const {
    TD_ASSERT(i < size, "List index out of bounds (was %d but size is %d)", i, size);
    return elements[i];
}

template<typename T>
void td::List<T>::reallocate(uint32 new_capacity) {
    TD_ASSERT(new_capacity != 0, "Cannot allocate 0 elements (mplementation bug in td::List)");
    TD_ASSERT(new_capacity > capacity, "Cannot reallocate to smaller size (current size is %d, new size %d) (implementation bug in td::List )", capacity, new_capacity);

    T* new_elements = (T*) ::operator new(sizeof(T) * new_capacity);
    TD_ASSERT(new_elements != nullptr, "Failed at allocated buffer of size %d", new_capacity * sizeof(T));

    if( elements != nullptr ) {
        for( uint32 i = 0; i < size; i++ ) {
            new (new_elements + i) T(td::move(elements[i]));
            elements[i].~T();
        }

        ::operator delete ((void*)elements);    
    }
    
    elements = new_elements;
    capacity = new_capacity;
}