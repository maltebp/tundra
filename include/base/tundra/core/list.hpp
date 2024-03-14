#pragma once

#include <cstdlib>

#include <tundra/core/types.hpp>
#include <tundra/core/assert.hpp>
#include <tundra/core/utility.hpp>

// For some reason, it cannot find the placement new
void* operator new(size_t count, void* address) noexcept;

namespace td {  

    template<typename T>
    class List {
    public:

        List() = default;

        List(const List& list) {

        }

        ~List() {
            if( elements != nullptr ) {
                for( uint32 i = 0; i < size; i++ ) {
                    elements[i].~T();
                }
                ::operator delete ((void*)elements);
            }
        }

        void add(const T& element) {
            if( size == available_size ) {
                uint32 new_size = available_size == 0 ? 2 : available_size * 2;
                reallocate(new_size);
            }

            new (elements + size) T(element);
            size++;
        }

        void remove(uint32 index) {
            TD_ASSERT(index < size, "List index out of bounds (was %d but size is %d)", index, size);
            elements[index].~T();

            for( uint32 i = index + 1; i < size; i++ ) {
                new(elements + i - 1) T(td::move(elements[i]));
                elements[i].~T();
            }

            size--;
        }

        void clear() {
            if( elements == nullptr ) return;

            for( uint32 i = 0; i < size; i++ ) {
                elements[i].~T();
            }            

            size = 0;
        }

        td::uint32 get_size() const { return size; }

        T& operator[](uint32 i) {
            TD_ASSERT(i < size, "List index out of bounds (was %d but size is %d)", i, size);
            return elements[i];
        }

        const T& operator[](uint32 i) const {
            TD_ASSERT(i < size, "List index out of bounds (was %d but size is %d)", i, size);
            return elements[i];
        }

    private:

        void reallocate(uint32 new_available_size) {
            TD_ASSERT(new_available_size != 0, "Cannot allocate 0 elements (mplementation bug in td::List)");
            TD_ASSERT(new_available_size > available_size, "Cannot reallocate to smaller size (current size is %d, new size %d) (implementation bug in td::List )", available_size, new_available_size);

            T* new_elements = (T*) ::operator new(sizeof(T) * new_available_size);
            TD_ASSERT(new_elements != nullptr, "Failed at allocated buffer of size %d", new_available_size * sizeof(T));

            if( elements != nullptr ) {
                for( uint32 i = 0; i < size; i++ ) {
                    new (new_elements + i) T(td::move(elements[i]));
                    elements[i].~T();
                }

                ::operator delete ((void*)elements);    
            }
            
            elements = new_elements;
            available_size = new_available_size;
        }

        T* elements = nullptr;

        td::uint32 available_size = 0;

        td::uint32 size = 0;

    };

}