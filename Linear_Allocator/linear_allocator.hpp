#ifndef LINEAR_ALLOC_HPP
#define LINEAR_ALLOC_HPP


/**
 *  The linear_contiguous_allocator manages the memory arena of the fixed size. It can allocate and
 *  deallocate the memory blocks of arbitrary sizes and alignments. The allocator takes the size of the
 *  memory arena as template parameter and creates the arena with that size.
 */



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

    /**
     * @brief linear_contiguous_allocator Converting constructor
     * @param _ar User-provided memory arena to use
     */
    linear_contiguous_allocator(arena<N>& _ar) : ar{_ar} {}

    /**
     * @brief linear_contiguous_allocator Copy constructor
     * @param other
     *
     * The copy constructor will create another instance of allocator but will not create another
     * arena. So there will be multiple allocators which will operate on that arena. Use some kind of
     * external synchronization if multiple allocators are going to operate on same arena.
     */
    linear_contiguous_allocator(const linear_contiguous_allocator&  other) : ar{other.ar} {}

    /**
     * @brief operator = Copy Assignment operator has been deleted
     * @param other
     * @return
     */
    linear_contiguous_allocator<N> & operator= (const linear_contiguous_allocator<N>&  other) = delete;

    /**
     * @brief allocate Allocates the storage for required size and alignment
     * @param count Size of the block in bytes
     * @param align Alignment of the block
     * @return Address of the allocated block
     */
    std::byte* allocate(std::size_t count, std::size_t align = alignof(std::max_align_t)){

        if(std::byte* ptr {ar.allocate(count, align)}; ptr)
            return ptr;
        throw std::bad_alloc();
    }

    /**
     * @brief deallocate Deallocates the storage
     * @param ptr Address of the block which has to be deallocated
     */
    void deallocate(std::byte* ptr){
        if(ptr)
            ar.deallocate(ptr);
    }

#ifdef ARENA_BYTE_INFO
    std::size_t get_available_bytes() const {
        return ar.get_available_bytes();
    }

    std::size_t get_occupied_bytes() const {
        return ar.get_occupied_bytes();
    }
#endif
};


#endif // LINEAR_ALLOC_HPP
