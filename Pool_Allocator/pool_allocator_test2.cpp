#include "pool_allocator.hpp"
#include <algorithm>


#define PRINT(fmt, str) std::printf(fmt, str);
#define PRINTSTR(str) std::printf(str);

#define PRINT_ALLOCATED_CHUNKS(obj) PRINT("Allocated chunks: %lu\n", obj.allocated_chunks());
#define PRINT_AVAILABLE_CHUNKS(obj) PRINT("Available chunks: %lu\n", obj.available_chunks());


struct student{
    long contact;
    int age;
    char* name;
};

using student_record_t =  student*;

int main(){

    constexpr auto req_size  {sizeof(student)};
    constexpr auto req_align {alignof(student)};
    constexpr auto ar_size {500};
    constexpr auto buffer_size {req_size * ar_size};

    pool_allocator<req_size, req_align> stud_list_alloc(buffer_size);
    student_record_t ptrs[ar_size];

    PRINT("Total Size: %lu\n", buffer_size);
    PRINT("Total Chunks Available for Allocation: %lu\n", stud_list_alloc.available_chunks());

    PRINT_ALLOCATED_CHUNKS(stud_list_alloc);
    PRINT_AVAILABLE_CHUNKS(stud_list_alloc);

    std::for_each(std::begin(ptrs), std::end(ptrs), [&](auto& elem){
        elem = reinterpret_cast<student_record_t>(stud_list_alloc.allocate());
    });

    PRINTSTR("After Allocating All Chunks\n");

    PRINT_ALLOCATED_CHUNKS(stud_list_alloc);
    PRINT_AVAILABLE_CHUNKS(stud_list_alloc);

    std::for_each(std::begin(ptrs), std::end(ptrs), [&](auto& elem){
        stud_list_alloc.deallocate(reinterpret_cast<std::byte*>(elem));
    });

    PRINTSTR("After Deallocating All Chunks\n");

    PRINT_ALLOCATED_CHUNKS(stud_list_alloc);
    PRINT_AVAILABLE_CHUNKS(stud_list_alloc);

    std::for_each(std::begin(ptrs), std::next(std::begin(ptrs), 100), [&](auto& elem){
        elem = reinterpret_cast<student_record_t>(stud_list_alloc.allocate());
    });

    PRINTSTR("After Allocating 100 Chunks\n");

    PRINT_ALLOCATED_CHUNKS(stud_list_alloc);
    PRINT_AVAILABLE_CHUNKS(stud_list_alloc);

    std::for_each(std::rbegin(ptrs), std::next(std::rbegin(ptrs), 250), [&](auto& elem){
        elem = reinterpret_cast<student_record_t>(stud_list_alloc.allocate());
    });

    PRINTSTR("After Allocating 250 more Chunks\n");

    PRINT_ALLOCATED_CHUNKS(stud_list_alloc);
    PRINT_AVAILABLE_CHUNKS(stud_list_alloc);

    std::for_each(std::rbegin(ptrs), std::next(std::rbegin(ptrs), 250), [&](auto& elem){
        stud_list_alloc.deallocate(reinterpret_cast<std::byte*>(elem));
    });

    PRINTSTR("After Deallocating 250 Chunks\n");

    PRINT_ALLOCATED_CHUNKS(stud_list_alloc);
    PRINT_AVAILABLE_CHUNKS(stud_list_alloc);

    std::for_each(std::begin(ptrs), std::next(std::begin(ptrs), 30), [&](auto& elem){
        stud_list_alloc.deallocate(reinterpret_cast<std::byte*>(elem));
    });

    PRINTSTR("After Deallocating 30 more Chunks\n");

    PRINT_ALLOCATED_CHUNKS(stud_list_alloc);
    PRINT_AVAILABLE_CHUNKS(stud_list_alloc);

    std::for_each(std::next(std::begin(ptrs), 30), std::next(std::begin(ptrs), 60), [&](auto& elem){
        stud_list_alloc.deallocate(reinterpret_cast<std::byte*>(elem));
    });

    PRINTSTR("After Deallocating 30 more Chunks\n");

    PRINT_ALLOCATED_CHUNKS(stud_list_alloc);
    PRINT_AVAILABLE_CHUNKS(stud_list_alloc);

    std::for_each(std::next(std::begin(ptrs), 60), std::next(std::begin(ptrs), 90), [&](auto& elem){
        stud_list_alloc.deallocate(reinterpret_cast<std::byte*>(elem));
    });

    PRINTSTR("After Deallocating 30 more Chunks\n");

    PRINT_ALLOCATED_CHUNKS(stud_list_alloc);
    PRINT_AVAILABLE_CHUNKS(stud_list_alloc);

    std::for_each(std::next(std::begin(ptrs), 90), std::next(std::begin(ptrs), 100), [&](auto& elem){
        stud_list_alloc.deallocate(reinterpret_cast<std::byte*>(elem));
    });

    PRINTSTR("After Deallocating final 10 Chunks\n");

    PRINT_ALLOCATED_CHUNKS(stud_list_alloc);
    PRINT_AVAILABLE_CHUNKS(stud_list_alloc);

    return 0;
}
