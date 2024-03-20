#pragma once

#include <tundra/core/list.fwd.hpp>

#include <tundra/core/types.hpp>

namespace td {  

    template<typename T>
    class List {
    public:

        List() = default;

        // Add num_elements default initialized elements to the list
        List(td::uint32 num_elements);

        List(const List& other);

        List(List&& other);

        ~List();

        void add(const T& element);

        void remove(uint32 index);

        // Destroy all elements and set size to 0 (does not deallocate memory)
        void clear();

        td::uint32 get_size() const;

        List<T>& operator=(const List& other);

        T& operator[](uint32 i);

        const T& operator[](uint32 i) const;

    private:

        void reallocate(uint32 new_capacity);

        T* elements = nullptr;

        // Size of allocated memory
        td::uint32 capacity = 0;

        td::uint32 size = 0;

    };

}