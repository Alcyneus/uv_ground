#if !defined(_TEST_BIT_SYNC_HPP_)
#define _TEST_BIT_SYNC_HPP_

#include <unistd.h>

#include "bitSync.hpp"

int TEST_BIT_SYNC()
{
    
    BitSync *bitSync = (BitSync *)BitSync::getInstance();

    bitSync->init(2048, 2);

    uint8_t *master = (uint8_t *)calloc(2048, sizeof(uint8_t));
    uint8_t *head = (uint8_t *)calloc(2, sizeof(uint8_t));

    memset(master, 0x55, 2048);
    head[0] = 0xdd;
    head[1] = 0x5c;



    master[0] = 0xdd;
    master[1] = 0xd5;
    master[2] = 0xcd;



    while (1)
    {
        // bitSync->run(master, head);

        sleep(2);
    }
    
}


#endif // _TEST_BIT_SYNC_HPP_
