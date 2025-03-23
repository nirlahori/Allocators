#include "pool_allocator.hpp"
#include <array>
#include <algorithm>

#define PRINT(fmt, str) std::printf(fmt, str);
#define PRINTSTR(str) std::printf(str);

#define PRINT_ALLOCATED_CHUNKS(obj) PRINT("Allocated chunks: %lu\n", obj.allocated_chunks());
#define PRINT_AVAILABLE_CHUNKS(obj) PRINT("Available chunks: %lu\n", obj.available_chunks());



int main(){

    constexpr std::size_t req_size {8};
    constexpr std::size_t req_align {8};

    pool_allocator<req_size, req_align> p1(req_size * 100);

    std::array<double*, 100> darray;

    PRINTSTR("===============Before allocations==================\n");
    PRINT_ALLOCATED_CHUNKS(p1);
    PRINT_AVAILABLE_CHUNKS(p1);

    std::for_each(darray.begin(), darray.end(), [&p1](double*& d){
        d = reinterpret_cast<double*>(p1.allocate());
    });

    PRINTSTR("===============After allocating all chunks====================\n");
    PRINT_ALLOCATED_CHUNKS(p1);
    PRINT_AVAILABLE_CHUNKS(p1);

    std::for_each(darray.begin(), std::next(darray.begin(), darray.size() / 2), [&p1](double* d){
        p1.deallocate(reinterpret_cast<std::byte*>(d));
    });


    PRINTSTR("=================After deallocating some chunks===================\n");
    PRINT_ALLOCATED_CHUNKS(p1);
    PRINT_AVAILABLE_CHUNKS(p1);

    std::for_each(std::next(darray.rbegin(), darray.size() / 2), darray.rend(), [&p1](double*& d){
        d = reinterpret_cast<double*>(p1.allocate());
    });

    PRINTSTR("===============After some more allocations====================\n");
    PRINT_ALLOCATED_CHUNKS(p1);
    PRINT_AVAILABLE_CHUNKS(p1);


    std::for_each(darray.begin(), darray.end(), [&p1](double* d){
        p1.deallocate(reinterpret_cast<std::byte*>(d));
    });

    PRINTSTR("=================After deallocating all chunks===================\n");

    PRINT_ALLOCATED_CHUNKS(p1);
    PRINT_AVAILABLE_CHUNKS(p1);

    return 0;
}
