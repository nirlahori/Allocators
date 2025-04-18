#ifndef STATIC_BUFFER_ARENA_HPP
#define STATIC_BUFFER_ARENA_HPP


/**
 * Arena class represents the memory arena. Users need to specify the size of the arena as template
 * parameter. Allocators can use this arena for allocating and deallocating arbitrarily sized memory
 * blocks. Arena maintains the list of memory blocks which are in use as well as free memory blocks.
 */



#include <cstddef>
#include <list>
#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <memory>

/**
 * @brief The block_info class
 * Internal data structure used by arena to maintain information about the allocated and deallocated
 * memory blocks.
 */
struct block_info{
    std::byte* ptr;
    std::size_t aligned_by;
    std::size_t count;
    std::size_t bytes_for_align;

    bool operator== (const block_info& other) const{
        return (this->ptr == other.ptr &&
                this->count == other.count &&
                this->aligned_by == other.aligned_by &&
                this->bytes_for_align == other.bytes_for_align);
    }

    bool operator!= (const block_info& other) const{
        return !(*this == other);
    }
};



template<std::size_t bytes>
class arena{
    std::byte buffer[bytes];

    std::list<block_info> uselist;
    std::list<block_info> freelist;

    using iter_type = std::list<block_info>::iterator;

    std::size_t available_bytes {bytes};
    std::size_t occupied_bytes {0};
    std::byte* curr_byte {static_cast<std::byte*>(buffer)};
    std::byte* end_byte {static_cast<std::byte*>(buffer + bytes)};

public:

    arena() = default;

    // Arena cannot be copied
    arena(const arena&) = delete;
	arena& operator= (const arena&) = delete;

    // Arena cannot be moved
    arena(arena&&) = delete;
    arena& operator= (arena&&) = delete;


    /**
     * @brief allocate Allocates the storage from arena
     * @param count Size of block to allocate
     * @param align Alignment of the block to allocate
     * @return Address of the allocated block
     */
	[[gnu::alloc_align(3), gnu::alloc_size(2), gnu::malloc, gnu::returns_nonnull]] [[nodiscard]]
    std::byte* allocate(std::size_t count, std::size_t align = alignof(std::max_align_t)){

        if(available_bytes < count)
            throw std::bad_alloc();


        if(!std::all_of(freelist.begin(), freelist.end(), [count](const block_info& block){ return block.count < count; })){

            void* ptr {nullptr};
            std::size_t bytes_for_align {0};

            iter_type matched_block { std::find_if(freelist.begin(), freelist.end(), [count, align, &ptr, &bytes_for_align](block_info& block){

                if(block.count < count)
                    return false;
                ptr = static_cast<void*>(block.ptr);
                bytes_for_align = block.count;
                return (std::align(align, count, ptr, bytes_for_align)) ? true : false;
            })};

            if(matched_block != freelist.end()){

                block_info new_block {static_cast<std::byte*>(ptr), align, count, matched_block->count - bytes_for_align};
                uselist.push_back(new_block);
                matched_block->ptr += count;
                matched_block->count -= count;
                available_bytes -= count;
                occupied_bytes += count;
                return new_block.ptr;
            }
        }

        void* ptr {static_cast<void*>(curr_byte)};

        std::size_t bytes_for_align{available_bytes};
        ptr = std::align(align, count, ptr, bytes_for_align);

        if(ptr){

            std::size_t used_bytes {available_bytes - bytes_for_align};
            std::byte* next_addr {static_cast<std::byte*>(ptr)};
            available_bytes -= (count + used_bytes);
            occupied_bytes += (count + used_bytes);
            curr_byte = next_addr + count;
            uselist.push_back(block_info{next_addr, align, count, used_bytes});
            return next_addr;
        }

        throw std::bad_alloc();
    }

    /**
     * @brief deallocate Deallocates the storage
     * @param ptr Address of the block to deallocate
     */
	[[gnu::nonnull]]
    void deallocate(std::byte* ptr){

        const iter_type block_pos {std::find_if(uselist.begin(), uselist.end(), [&](const block_info& block) -> bool {
            return block.ptr == ptr;
        })};

        if(block_pos == uselist.end())
            return; // Handle the situation appropriately
        available_bytes += (block_pos->count + block_pos->bytes_for_align);
        occupied_bytes -= (block_pos->count + block_pos->bytes_for_align);


        if(block_pos->ptr + block_pos->count == curr_byte){
            curr_byte -= (block_pos->count + block_pos->bytes_for_align);
        }
        else{
            block_info freeblock{block_pos->ptr - block_pos->bytes_for_align, 0, block_pos->count + block_pos->bytes_for_align, 0};
            freelist.push_back(std::move(freeblock));
        }
        uselist.remove(*block_pos);
    }

#ifdef ARENA_BYTE_INFO
    std::size_t get_available_bytes() const {
        return available_bytes;
    }

    std::size_t get_occupied_bytes() const {
        return occupied_bytes;
    }
#endif
};


#endif // STATIC_BUFFER_ARENA_HPP
