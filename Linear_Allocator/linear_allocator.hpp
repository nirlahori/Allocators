#ifndef LINEAR_ALLOC_HPP
#define LINEAR_ALLOC_HPP


#include <cstddef>
#include <new>
#include <memory>
#include <list>
#include <algorithm>
#include <cstdio>
#include "static_buffer_arena.hpp"

template<std::size_t N>
class linear_contiguous_allocator{


    arena<N>& ar;

public:


    linear_contiguous_allocator(arena<N>& _ar) : ar{_ar} {}
    linear_contiguous_allocator(const linear_contiguous_allocator&  other) : ar{other.ar} {}

    linear_contiguous_allocator<N> & operator= (const linear_contiguous_allocator<N>&  other) = delete;


    std::byte* allocate(std::size_t count, std::size_t align = alignof(std::max_align_t)){

        if(std::byte* ptr {ar.allocate(count, align)}; ptr)
            return ptr;
        throw std::bad_alloc();
    }


    void deallocate(std::byte* ptr){
        if(ptr)
            ar.deallocate(ptr);
    }

    /*std::size_t get_available_bytes() const {
        return ar.get_available_bytes();
    }

    std::size_t get_occupied_bytes() const {
        return ar.get_occupied_bytes();
    }

    void print_align(){
        ar.print();
    }*/

};





#endif // LINEAR_ALLOC_HPP
