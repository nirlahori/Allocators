#include "stack_allocator.hpp"
#include <vector>
#include <iostream>
#include <iomanip>
#include <list>

int main(){


    stack_allocator<int, 80> stk_alloc;

    {
        std::vector<int, stack_allocator<int, 80>> vec {stk_alloc};
        for(int i=0; i<20; i++){
            vec.push_back(i);
        }

        for(int i : vec){
            std::cout << std::setw(2) << i << std::endl;
        }

        vec.resize(22, 76);

        for(int i : vec){
            std::cout << std::setw(2) << i << std::endl;
        }
    }

    {
        std::list<int, stack_allocator<int, 80>> lst{stk_alloc};
        for(int i=0; i<5; i++){
            lst.push_back(i);
        }
        lst.resize(7, 45);
        for(int i : lst){
            std::cout << std::setw(2) << i << std::endl;
        }
    }
    return 0;
}
