#if !defined(_TEST_IIO_BASEBAND_BASEBAND_HPP_)
#define _TEST_IIO_BASEBAND_BASEBAND_HPP_

#include "baseband.hpp"

int TEST_IIO_BASEBAND()
{
    Baseband *baseband =  (Baseband *)Baseband::getInstance();

    baseband->init();
}


#endif // _TEST
