#if !defined(_COMMON_BRAM_BRAM_HPP_)
#define _COMMON_BRAM_BRAM_HPP_

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string>
#include <sys/mman.h>
#include <string.h>
#include "gpio.hpp"

#include "debug.hpp"     

#if _BRAM_DEBUG_
#define BRAM_MSG(...) printf(__VA_ARGS__)
#define BRAM_DEBUG(...) printf(__VA_ARGS__)
#else
#define BRAM_MSG(...)
#define BRAM_DEBUG(...)  
#endif


#define BRAM_MAP_SIZE 4096UL
#define BRAM_MAP_MASK (BRAM_MAP_SIZE -1)
#define BRAM_MAP_ADDR_TX 0x40000000
#define BRAM_MAP_ADDR_RX 0x42000000
#define BRAM_BRAM_FILE "/dev/mem"

class BRAM
{
private:

    GPIO *gpio;

    static volatile BRAM *instance;
    static pthread_mutex_t mtx;
public:
    BRAM(/* args */);
    ~BRAM();

    static volatile BRAM *getInstance(); 

    void init(GPIO *gpio);
    void writeBegin();
    void writeFinish();

    void write(uint32_t phyaddr , uint32_t data);
    void write(uint32_t phyaddr , uint32_t data, uint32_t mapAddr);

    uint32_t read(uint32_t phyaddr);
    uint32_t read(uint32_t phyaddr, uint32_t mapAddr);
};

volatile BRAM * BRAM::instance = NULL;
pthread_mutex_t BRAM::mtx;

BRAM::BRAM(/* args */)
{
}

BRAM::~BRAM()
{
}


volatile BRAM *BRAM::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new BRAM();
        }
        pthread_mutex_unlock(&mtx);
    }
    BRAM_DEBUG("BRAM_DEBUG: Get the %sBRAM instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}


void BRAM::init(GPIO *gpio)
{
    this->gpio = gpio;
}


/**
 * @brief Notify PL that it is starting to write BRAMs
 * 
 */
void BRAM::writeBegin()
{
    /*Notify PL that it is starting to write BRAM*/
    while (gpio->get(PL_READ_BRAM_READY) == 1);
    gpio->set(PS_WRITE_BRAM_BEGIN, 1);
    gpio->set(PS_WRITE_BRAM_BEGIN, 0);
    while (gpio->get(PL_READ_BRAM_READY) == 0);
}

void BRAM::writeFinish()
{
    gpio->set(PS_WRITE_BRAM_FINISH, 1);
    gpio->set(PS_WRITE_BRAM_FINISH, 0);
}

void BRAM::write(uint32_t phyaddr , uint32_t data)
{
    write(phyaddr, data, BRAM_MAP_ADDR_TX);
}

/** @brief BRAM 写数据
 *  @param sendData	    遥控数据结构体
 *  @param sendBuffer	发送缓冲	
 *  @return	            
 **/
void BRAM::write(uint32_t phyaddr , uint32_t data, uint32_t mapAddr)
{

    int bramFileDescriptor;             /*BRAM file descriptor*/  
    uint32_t AddressOffset;             /*Physical address offset*/
    volatile uint8_t *mapBaseAddress;   /*bram write address(mapping address)*/

    AddressOffset = (phyaddr & (BRAM_MAP_MASK));

    /*open BRAM file descriptor*/ 
    bramFileDescriptor = open(BRAM_BRAM_FILE,O_RDWR | O_SYNC);

    /*file open failed?*/
    if(-1 == open(BRAM_BRAM_FILE, O_RDWR | O_SYNC))
    {
        BRAM_DEBUG("BRAM_DEBUG: %s bram write: open [%s] failed.%s\n", RED, BRAM_BRAM_FILE, NONE);
        return ;
    }

    /*mapping address*/
    mapBaseAddress = (volatile uint8_t *)mmap(NULL,BRAM_MAP_SIZE,PROT_READ | PROT_WRITE ,MAP_SHARED,bramFileDescriptor,mapAddr);

    /*mapping address failed?*/
    if(mapBaseAddress < 0) {
        BRAM_DEBUG("BRAM_DEBUG: %s bram write: mmap failed.%s\n",RED, NONE);
        close(bramFileDescriptor);
        return ;
    }
    BRAM_DEBUG("BRAM_DEBUG: map base address: 0x%x\n", (volatile unsigned int *)(mapBaseAddress + AddressOffset));

    /*write data*/
    memcpy((void *)(mapBaseAddress + AddressOffset), &data, sizeof(data));
    /*Pl is slow, write twice*/
    usleep(1);
    memcpy((void *)(mapBaseAddress + AddressOffset), &data, sizeof(data));

    BRAM_DEBUG("BRAM_DEBUG: write %s[phyaddr:0x%x], [data:0x%x]%s successful!\n", YELLOW, phyaddr, data, NONE);

    /*unmapping address*/
    munmap((void *)mapBaseAddress,BRAM_MAP_SIZE);
    close(bramFileDescriptor);
}


uint32_t BRAM::read(uint32_t phyaddr)
{
    return read(phyaddr, BRAM_MAP_ADDR_RX);
}

uint32_t BRAM::read(uint32_t phyaddr, uint32_t mapAddr)
{

    int bramFileDescriptor; /*BRAM file descriptor*/  
    uint32_t data;          /*read data*/
    void *mapBaseAddress;   /*bram write address(mapping address)*/

    uint32_t AddressOffset = phyaddr & (BRAM_MAP_MASK);

    /*open BRAM file descriptor*/
    bramFileDescriptor = open(BRAM_BRAM_FILE,O_RDWR | O_SYNC);

    /*file open failed?*/
    if( -1 == bramFileDescriptor){
        BRAM_DEBUG("BRAM_DEBUG: %s bram read: open [%s] failed.%s\n", RED, BRAM_BRAM_FILE, NONE);
        return -1;
    }

    /*mapping address*/
    mapBaseAddress = mmap(NULL,BRAM_MAP_SIZE,PROT_READ | PROT_WRITE ,MAP_SHARED,bramFileDescriptor,mapAddr);

    /*mapping address failed?*/
    if(mapBaseAddress < 0) 
    {
        BRAM_DEBUG("BRAM_DEBUG: read error [mmap failed].\n");
        return -1;
    }

    memcpy(&data, (void *)(mapBaseAddress + AddressOffset), sizeof(data));
    usleep(1);
    memcpy(&data, (void *)(mapBaseAddress + AddressOffset), sizeof(data));

    BRAM_DEBUG("BRAM_DEBUG: map base address: 0x%x\n", (volatile unsigned int *)(mapBaseAddress + AddressOffset));

    munmap((void *)mapBaseAddress,BRAM_MAP_SIZE);
    close(bramFileDescriptor);

    BRAM_DEBUG("BRAM_DEBUG: read %s[phyaddr:0x%x], [data:0x%x]%s successful!\n", YELLOW, phyaddr, data, NONE);
    return data;
}
#endif // _COMMON_BRAM_BRAM_HPP_
