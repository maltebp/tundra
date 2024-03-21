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

        void reserve(td::uint32 capacity_to_reserve);

        void add(const T& element);

        void add(T&& element);

        // Removes first element matching type - returns true if any elements was found
        bool remove(const T& element);

        void remove_at(uint32 index);

        // Destroy all elements and set size to 0 (does not deallocate memory)
        void clear();
        
        // Returns the index of the first matching element uint32::max if it was
        // not found
        [[nodiscard]] uint32 index_of(const T& element) const;

        [[nodiscard]] td::uint32 get_size() const;

        [[nodiscard]] T& get_last();

        [[nodiscard]] const T& get_last() const;

        List<T>& operator=(const List& other);

        [[nodiscard]] T& operator[](uint32 i);

        [[nodiscard]] const T& operator[](uint32 i) const;

    private:

        void reallocate(uint32 new_capacity);

        T* elements = nullptr;

        // Size of allocated memory
        td::uint32 capacity = 0;

        td::uint32 size = 0;

    };

}