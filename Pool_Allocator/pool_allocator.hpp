#ifndef POOL_ALLOCATOR_HPP
#define POOL_ALLOCATOR_HPP

#include <memory>
#include <cstddef>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>


struct mem_chunk{
    mem_chunk* next;
};


template<std::size_t chk_size, std::size_t chk_align = 8>
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

    std::byte* allocate_chunk(){
        chunk_type* node {head};
        head = head->next;
        return reinterpret_cast<std::byte*>(std::memset(node, '\0', chk_size));
    }

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


    pool_allocator() :
        pool_allocator(sysconf(_SC_PAGE_SIZE)) {}

    explicit pool_allocator(const std::size_t& buffer_size) :
        pool_allocator(mmap(nullptr, buffer_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,  0), buffer_size) {}

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

    void deallocate(std::byte* ptr){
        deallocate_chunk(ptr);
    }

    std::byte* allocate() noexcept {
        return allocate_chunk();
    }

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

    std::size_t allocated_chunks() const noexcept {
        return mem_buffer_size / chk_size - available_chunks();
    }
};


#endif // POOL_ALLOCATOR_HPP
