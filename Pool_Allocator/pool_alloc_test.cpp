#include "pool_allocator.hpp"



int main(){


    pool<100, 4> p1;

    std::byte* b[25];

    for(int i=0; i<25; i++){
        b[i] = p1.allocate();
        int* ptr = reinterpret_cast<int*>(b[i]);
        *ptr = i + 100;
    }

    for(int i=0; i<25; i++){
        int* ptr = reinterpret_cast<int*>(b[i]);
        std::printf("%d\n", *ptr);
    }


    for(int i=0; i<25; i++)
        p1.deallocate(b[i]);

    //p1.print_free_blocks();
    //p1.print();
    return 0;
}
