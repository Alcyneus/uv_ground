#if !defined(_BEAN_H_)
#define _BEAN_H_


#include <stdint.h>
#include <pthread.h>
#include <string>
#include <string.h>
#include <sstream>
#include <time.h>
#include <stdio.h>
 

#if _BEAN_DEBUG_
#define BEAN_MSG(...) printf(__VA_ARGS__)
#define BEAN_DEBUG(...) printf(__VA_ARGS__)
#else
#define BEAN_MSG(...)
#define BEAN_DEBUG(...)  
#endif

using namespace std;

class Bean
{
private:
    /* data */
public:
    Bean(/* args */);
    ~Bean();

    virtual int pack(uint8_t *buffer) = 0;
    virtual int unpack(uint8_t *buffer) = 0;

    virtual string toString() = 0;
};

Bean::Bean(/* args */)
{
}

Bean::~Bean()
{
}


#endif // _BEAN_H_
