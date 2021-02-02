#if !defined(_IIO_BASEBAND_BASEBAND_HPP_)
#define _IIO_BASEBAND_BASEBAND_HPP_

#include "uv_config.h"


#include <stdio.h>


#include "debug.hpp" 


#ifndef _BASEBAND_DEBUG_
#define BASEBAND_MSG(...) printf(__VA_ARGS__)
#define BASEBAND_DEBUG(...) printf(__VA_ARGS__)
#else
#define BASEBAND_MSG(...)
#define BASEBAND_DEBUG(...)  
#endif


class Baseband
{
    private:  
        static volatile Baseband *instance;
        static pthread_mutex_t mtx;
    public:
        Baseband(/* args */);
        ~Baseband();
        static volatile Baseband *getInstance(); 
        static volatile void init();
};

volatile Baseband * Baseband::instance = NULL;
pthread_mutex_t Baseband::mtx;

Baseband::Baseband(/* args */)
{
}

Baseband::~Baseband()
{
}

volatile Baseband *Baseband::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new Baseband();
        }
        pthread_mutex_unlock(&mtx);
    }
    BASEBAND_DEBUG("BASEBAND_DEBUG: Get the %sCan instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}

volatile void Baseband::init()
{
    uv_configure_init_thread();
}


#endif // _IIO_BASEBAND_BASEBAND_HPP_
