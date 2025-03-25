#include "linear_allocator.hpp"
#include <iostream>

template<std::size_t N>
void print_info(std::size_t bytes, const linear_contiguous_allocator<N>& alloc){
    //std::cout << "Available bytes after alloc of " << bytes << " bytes -> " << alloc.get_available_bytes() << std::endl;
    //std::cout << "Occupied bytes after alloc of " << bytes << " bytes -> " << alloc.get_occupied_bytes() << std::endl;
}

int main(){


    arena<1024> ar;
    linear_contiguous_allocator<1024> alloc {ar};

    {
        std::byte* addr1 {alloc.allocate(64)};
        print_info<1024>(64, alloc);
        //alloc.deallocate(addr1);

        std::byte* addr2 {alloc.allocate(128)};
        print_info<1024>(128, alloc);
        alloc.deallocate(addr1);

        std::byte* addr3 {alloc.allocate(256)};
        print_info<1024>(256, alloc);
        alloc.deallocate(addr2);
        //alloc.deallocate(addr1);

        std::byte* addr4 {alloc.allocate(64)};
        print_info<1024>(64, alloc);

        std::byte* addr5 {alloc.allocate(64)};
        print_info<1024>(64, alloc);

        std::byte* addr6 {alloc.allocate(32)};
        print_info<1024>(32, alloc);


        std::byte* addr7 {alloc.allocate(256)};
        print_info<1024>(256, alloc);

        alloc.deallocate(addr3);
        alloc.deallocate(addr4);
        alloc.deallocate(addr5);
        alloc.deallocate(addr6);
        alloc.deallocate(addr7);
    }


    return 0;
}
