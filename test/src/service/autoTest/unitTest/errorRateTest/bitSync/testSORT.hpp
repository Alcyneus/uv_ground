#if !defined(_TEST_SORT_HPP_)
#define _TEST_SORT_HPP_


#include "sort.hpp"

#include <iostream>


using namespace std;

int TEST_SORT()
{
    int master[16] = {65535, 65535, 344, 455, 566, 677,788, 899,
                      900, 901, 902, 903, 904, 905, 906, 907  };

    bubbleSort(master, 16);


    for (int i = 0; i < 16; i++)
    {
        std::cout << "master"<< i << "=" << master[i] << std::endl;
    }
    

}


#endif // _TEST_SORT_HPP_
