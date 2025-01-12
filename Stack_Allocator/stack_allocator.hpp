#ifndef GENERAL_ALLOC_HPP
#define GENERAL_ALLOC_HPP

#include <cstddef>
#include <cstdio>
#include <new>
#include "static_buffer_arena.hpp"

///
/// \brief The stack_allocator class
/// The stack allocator class is a basic stack allocator which can be used with STL containers and other
/// allocator-aware types in C++ Standard Library. The single allocator instance works only for a single
/// type. However, it can be used to allocate different types with a same instance if all the allocated
/// types have same alignment.
///



template<typename T, std::size_t elements>
class stack_allocator{

    static arena<elements * sizeof(T)> _ar;

public:

    using value_type = T;

    template<typename U>
    struct rebind{
        using other = stack_allocator<U, elements>;
    };

    stack_allocator() = default;
    stack_allocator(const stack_allocator&) = default;
    stack_allocator& operator=(const stack_allocator&) = default;

    template<typename U>
    stack_allocator(const U& alloc){}

    T* allocate(std::size_t count){
        std::byte* ptr{_ar.allocate(count * sizeof(T), alignof(T))};
        if(!ptr)
            throw std::bad_alloc();
        return reinterpret_cast<T*>(ptr);
    }


    void deallocate(T* ptr, std::size_t count){
        _ar.deallocate(reinterpret_cast<std::byte*>(ptr));
    }

    template<class ... Args>
    void construct(T* ptr, Args&& ... args){
        ::new(static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
    }

    void destroy(T* ptr){
        ptr->~T();
    }
};

template<class T, std::size_t count>
arena<count * sizeof(T)> stack_allocator<T, count>::_ar{};


#endif // GENERAL_ALLOC_HPP
