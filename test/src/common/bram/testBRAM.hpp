#if !defined(_TEST_COMMON_BRAM_BRAM_HPP)
#define _TEST_COMMON_BRAM_BRAM_HPP

#include "bram.hpp"

int TEST_COMMON_BRAM()
{
    BRAM *bram = (BRAM *)BRAM::getInstance();
    GPIO *gpio = (GPIO *)GPIO::getInstance();

    gpio->init();
    bram->init(gpio);

    uint32_t phyaddr = BRAM_MAP_ADDR_TX + 0x0004;

    bram->writeBegin();
    bram->write(phyaddr, 2);
    bram->write(phyaddr+4, 1);
    bram->write(phyaddr+4*2, 2);
    bram->write(phyaddr+4*3, 3);
    bram->write(phyaddr+4*4, 4);
    bram->write(phyaddr+4*5, 5);
    bram->write(phyaddr+4*6, 6);
    bram->write(phyaddr+4*7, 7);
    bram->write(phyaddr+4*8, 8);
    bram->write(phyaddr+4*9, 9);
    bram->writeFinish();


    bram->writeBegin();
    bram->read(phyaddr, BRAM_MAP_ADDR_TX);
    bram->read(phyaddr+4, BRAM_MAP_ADDR_TX);
    bram->read(phyaddr+4*2, BRAM_MAP_ADDR_TX);
    bram->read(phyaddr+4*3, BRAM_MAP_ADDR_TX);
    bram->read(phyaddr+4*4, BRAM_MAP_ADDR_TX);
    bram->read(phyaddr+4*5, BRAM_MAP_ADDR_TX);
    bram->read(phyaddr+4*6, BRAM_MAP_ADDR_TX);
    bram->read(phyaddr+4*7, BRAM_MAP_ADDR_TX);
    bram->read(phyaddr+4*8, BRAM_MAP_ADDR_TX);
    bram->read(phyaddr+4*9, BRAM_MAP_ADDR_TX);
    bram->writeFinish();
}


#endif // _TEST_COMMON_BRAM_BRAM_HPP
