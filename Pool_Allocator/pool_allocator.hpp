#ifndef POOL_ALLOCATOR_HPP
#define POOL_ALLOCATOR_HPP


/*
 *  Pool Allocator is a memory allocator that allocates the fixed sized memory blocks (chunks).
 *  The class takes required chunk size and chunk alignment as template parameters.
 */


#include <memory>
#include <cstddef>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

/**
 * @brief The mem_chunk class
 * The mem chunk type represents a single chunk. It stores the pointer to next free chunk. Pool Allocator
 * maintains the linked list of free memory chunks.
 */


struct mem_chunk{
    mem_chunk* next;
};


/**
 * @brief The pool_allocator class
 * Pool Allocator class manages the region of memory from which the allocator allocates and deallocates the
 * memory chunks. Pool Allocator imposes certain constraints on types for which pool allocator can be used.
 * The minimum size and minimum alignment to use pool allocator is the size and alignment of its internal chunk
 * type (mem_chunk). The Allocator maintains the linked list in the same memory region which is to be managed.
 */

template<std::size_t chk_size = sizeof(mem_chunk), std::size_t chk_align = alignof(mem_chunk)>
class pool_allocator{


    using chunk_type = mem_chunk;
    static_assert(chk_size >= sizeof(chunk_type), "Chunk size must be 8 bytes minimum");
    static_assert(chk_align >= alignof(chunk_type), "Chunk must atleast be 8-byte aligned");

    void* mem_buffer;
    std::size_t mem_buffer_size;
    chunk_type* head;

    const long page_size {sysconf(_SC_PAGE_SIZE)}; // Is system page size needed as a NSDM ?
    constexpr static std::size_t chunk_size = sizeof(chunk_type);

    void* buf_start;
    std::size_t buf_length;

private:

    /**
     * @brief allocate_chunk Removes the chunk at the beginning of the list.
     * @return Address of the removed chunk.
     * This is an internal function of the Allocator. Users are supposed to use
     * allocate() member function to allocate chunk.
     */
    [[gnu::malloc, gnu::returns_nonnull]] [[nodiscard]]
    std::byte* allocate_chunk(){
        chunk_type* node {head};
        head = head->next;
        return reinterpret_cast<std::byte*>(std::memset(node, '\0', chk_size));
    }


    /**
     * @brief deallocate_chunk Inserts the new chunk at appropriate place in the list.
     * @param ptr Starting address of the chunk to be deallocated
     * This is an internal function of the Allocator. Users are supposed to use
     * deallocate(std::byte*) member function to deallocate chunk.
     */
    [[gnu::nonnull]]
    void deallocate_chunk(std::byte* ptr){
        if(!(ptr >= buf_start && ptr < (static_cast<std::byte*>(buf_start) + buf_length)))
            throw std::logic_error("Invalid Address");
        chunk_type* chunk {reinterpret_cast<chunk_type*>(ptr)};
        if(head > chunk){
            chunk->next = head;
            head = chunk;
        }
        else{
            auto curr_node {head};
            auto prev_node {head};
            while(chunk > curr_node && curr_node != nullptr){
                prev_node = curr_node;
                curr_node = curr_node->next;
            }
            if(!prev_node){
                head = chunk;
                head->next = nullptr;
            }
            else{
                prev_node->next = chunk;
                chunk->next = curr_node;
            }
        }
    }

public:

    /**
     * @brief pool_allocator Default constructor
     * Manages the memory equivalent to system page size.
     */
    pool_allocator() :
        pool_allocator(sysconf(_SC_PAGE_SIZE)) {}

    /**
     * @brief pool_allocator Converting constructor
     * Manages the memory equivalent to user-provided buffer size.
     * @param buffer_size Size of the memory buffer in bytes. Allocator allocates
     * the storage to manage memory of the required size.
     */
    explicit pool_allocator(const std::size_t& buffer_size) :
        pool_allocator(mmap(nullptr, buffer_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,  0), buffer_size) {}

    /**
     * @brief pool_allocator Converting constructor
     * Manages the user-provided memory buffer.
     * @param buffer Starting address of memory buffer.
     * @param buffer_size Size of memory buffer in bytes.
     */
    pool_allocator(void* buffer, const std::size_t& buffer_size) :
        mem_buffer{buffer},
        mem_buffer_size{buffer_size},
        buf_start{mem_buffer},
        buf_length{mem_buffer_size} {

        void* init_buf {mem_buffer};
        std::size_t space {buffer_size};
        buf_start = std::align(chk_align, chk_size, init_buf, space);
        if(!buf_start){
            throw std::logic_error("");
        }
        buf_length = space;

        chunk_type* curr_chunk {reinterpret_cast<chunk_type*>(buf_start)};
        while(void* new_chk_start = std::align(chk_align, chk_size, init_buf, space)){
            curr_chunk->next = reinterpret_cast<chunk_type*>(new_chk_start);
            curr_chunk = curr_chunk->next;
            init_buf = static_cast<std::byte*>(init_buf) + chk_size;
            space -= chk_size;
        }
        head = reinterpret_cast<chunk_type*>(buf_start);
    }

    /**
     * @brief deallocate Takes chunk address as input and deallocates that chunk.
     * @param ptr Address of the chunk to deallocate
     */
    void deallocate(std::byte* ptr){
        deallocate_chunk(ptr);
    }

    /**
     * @brief allocate Allocates new chunk
     * @return Address of allocated memory chunk
     */
    std::byte* allocate() noexcept {
        return allocate_chunk();
    }

    /**
     * @brief available_chunks
     * @return Total number of available chunks
     */
    std::size_t available_chunks() const noexcept {
        if(head == nullptr)
            return 0;

        chunk_type* curr_chunk {head};
        std::size_t count{0};
        while(curr_chunk != nullptr){
            ++count;
            curr_chunk = curr_chunk->next;
        }
        return count;
    }

    /**
     * @brief allocated_chunks
     * @return Total number of allocated chunks
     */
    std::size_t allocated_chunks() const noexcept {
        return mem_buffer_size / chk_size - available_chunks();
    }
};


#endif // POOL_ALLOCATOR_HPP
