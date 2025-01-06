#ifndef POOL_ALLOCATOR_HPP
#define POOL_ALLOCATOR_HPP


#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>


template<std::size_t block_size, std::size_t chunk_size>
class pool{


    std::byte* head{nullptr};
    constexpr static std::size_t total_chunk_size{sizeof(std::byte*) + chunk_size};
    constexpr static std::size_t no_of_blocks{block_size / chunk_size};
    constexpr static std::size_t buffer_size{no_of_blocks * total_chunk_size};
    std::byte buffer[buffer_size];


public:

    pool(){

        std::size_t no_of_chunks {block_size / chunk_size};
        std::printf("buffer size %lu \n", buffer_size);

        std::memset(static_cast<void*>(buffer), 0, block_size);

        head = &buffer[0];
        std::size_t index{0};
        for(std::size_t i{0}; i<buffer_size-1; i+=total_chunk_size){
            //buffer[i] = static_cast<std::byte>(reinterpret_cast<std::uintptr_t>(&buffer[index + chunk_size]));
            //index += chunk_size;
            auto ptr {reinterpret_cast<std::byte**>(&buffer[i])};
            *ptr = &buffer[index + total_chunk_size];
            index += total_chunk_size;
        }
        auto ptr {reinterpret_cast<std::byte**>(&buffer[index-total_chunk_size])};
        *ptr = nullptr;
        std::printf("ptr : %p\n", *ptr);


        std::printf("index : %lu\n", index);

        std::printf("head -> %p\n", head);

        for(std::size_t i{0}; i<buffer_size-1; i+=total_chunk_size){
            auto ptr {reinterpret_cast<std::byte**>(&buffer[i])};
            std::printf("val -> %p, addr -> %p, \n", *ptr, &buffer[i]);
        }
    }


    std::byte* allocate(){

        std::byte* ptr {head};

        if(head == nullptr)
            throw std::bad_alloc();
        std::byte** next_ptr {reinterpret_cast<std::byte**>(&(*head))};
        if(*next_ptr != nullptr)
            head = ptr + total_chunk_size;
        else
            head = nullptr;
        std::byte* data_ptr {ptr + sizeof(std::byte*)};
        return data_ptr;
    }


    void deallocate(std::byte* ptr){

        std::printf("curr head -> %p\n", head);

        std::byte* start_offset {ptr - sizeof(std::byte*)};
        std::byte** head_ptr {reinterpret_cast<std::byte**>(start_offset)};
        *head_ptr = head;
        head = start_offset;
        std::printf("head ->  %p\n", head);
    }

    void print_free_blocks(){

        std::printf("head ->  %p\n", head);
        if(head == nullptr)
            return;
        auto* curr_ptr {head};
        while(*reinterpret_cast<std::byte**>((&(*curr_ptr))) != nullptr){
            std::printf("%p\n", *reinterpret_cast<std::byte**>(&(*curr_ptr)));
            curr_ptr = *reinterpret_cast<std::byte**>(&(*curr_ptr));
        }
    }

    void print(){
        std::printf("head -> %p\n", head);
        for(std::size_t i{0}; i<buffer_size-1; i+=total_chunk_size){
            std::printf("addr -> %p, val -> %p\n", &buffer[i], *reinterpret_cast<std::byte**>(&buffer[i]));
        }
    }
};


#endif // POOL_ALLOCATOR_HPP
