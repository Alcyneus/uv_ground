#if !defined(_COMMON_GPIO_GPIO_HPP_)
#define _COMMON_GPIO_GPIO_HPP_

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string>

#include "debug.hpp"                   


#if _GPIO_DEBUG_  
#define GPIO_MSG(...) printf(__VA_ARGS__)
#define GPIO_DEBUG(...) printf(__VA_ARGS__)
#else
#define GPIO_MSG(...)
#define GPIO_DEBUG(...)  
#endif



#define GPIO_LED0               906         //led0(测试)
#define GPIO_LED1               915         //led1(发射)
#define GPIO_LED2               959         //led2(接收)
#define GPIO_DMA_DBUG           977         //dma_debug
#define GPIO_PL_RESET           978         //pl复位
#define GPIO_PL_DATA_VALID      979         //bram_pl_tx, pl_data_valid
#define GPIO_PS_READ_START      980         //bram_pl_tx, ps_rd_start
#define GPIO_PS_READ_END        981         //bram_pl_tx, ps_rd_end
#define PS_WRITE_BRAM_BEGIN     982         // PS notifies PL to start writing BRAM
#define PS_WRITE_BRAM_FINISH    983         // PS notifies PL that write BRAM is complete
#define PL_READ_BRAM_READY      984         // PL is ready to read BRAM
#define GPIO_DMA_FIFO_FULL      985         //dma_fifo_full
#define GPIO_DMA_FIFO_EMPTY     986         //dma_fifo_empty
#define GPIO_ADC_DEBUG_SWITCH   987         //adc_debug_switch


#define GPIO_TYPE_OUT   "out"
#define GPIO_TYPE_IN    "in"

class GPIO
{
    private:
    static volatile GPIO *instance;
    static pthread_mutex_t mtx;

    public:
        GPIO(/* args */);
        ~GPIO();

    static volatile GPIO *getInstance(); 

    void init();


    int exported(char *exportPath, int gpioId);
    int direction(int gpioId, char *gpioType);       /*设置GPIO模式*/

    int set(int gpioId, bool gpioValue);
    int get(int gpioId);
};


volatile GPIO * GPIO::instance = NULL;
pthread_mutex_t GPIO::mtx;

GPIO::GPIO(/* args */)
{
}

GPIO::~GPIO()
{
}


volatile GPIO *GPIO::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new GPIO();
        }
        pthread_mutex_unlock(&mtx);
    }
    GPIO_DEBUG("GPIO_DEBUG: Get the %sGPIO instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}
void GPIO::init()
{
    char *exportPath = "/sys/class/gpio/export";
    
    exported(exportPath, GPIO_LED0);                  //led0(测试)
    exported(exportPath, GPIO_LED1);                  //led1(发射)
    exported(exportPath, GPIO_LED2);                  //led2(接收)
    exported(exportPath, GPIO_DMA_DBUG);              //dma_debug
    exported(exportPath, GPIO_PL_RESET);              //pl复位
    exported(exportPath, GPIO_PL_DATA_VALID);         //bram_pl_tx, pl_data_valid
    exported(exportPath, GPIO_PS_READ_START);         //bram_pl_tx, ps_rd_start
    exported(exportPath, GPIO_PS_READ_END);           //bram_pl_tx, ps_rd_end
    exported(exportPath, PS_WRITE_BRAM_BEGIN);        //bram_pl_rx, ps_wr_start
    exported(exportPath, PS_WRITE_BRAM_FINISH);        //bram_pl_rx, ps_wr_valid
    exported(exportPath, PL_READ_BRAM_READY);         //bram_pl_rx, pl_rd_rdy
    exported(exportPath, GPIO_DMA_FIFO_FULL);         //dma_fifo_full
    exported(exportPath, GPIO_DMA_FIFO_EMPTY);        //dma_fifo_empty
    exported(exportPath, GPIO_ADC_DEBUG_SWITCH);      //adc_debug_switch

    /*设置GPIO输入输出类型*/
    direction(GPIO_LED0, GPIO_TYPE_OUT);            //led0(测试)
    direction(GPIO_LED1, GPIO_TYPE_OUT);            //led1(发射)
    direction(GPIO_LED2, GPIO_TYPE_OUT);            //led2(发射)
    direction(GPIO_DMA_DBUG, GPIO_TYPE_OUT);        //dma debug
    direction(GPIO_PL_RESET, GPIO_TYPE_OUT);        //pl复位

    direction(GPIO_PL_DATA_VALID, GPIO_TYPE_IN);    //bram pl tx, pl data valid     IN

    direction(GPIO_PS_READ_START, GPIO_TYPE_OUT);   //bram pl tx, ps rd start
    direction(GPIO_PS_READ_END, GPIO_TYPE_OUT);     //bram pl tx, ps rd end
    direction(PS_WRITE_BRAM_BEGIN, GPIO_TYPE_OUT);  //bram pl rx, ps wr start
    direction(PS_WRITE_BRAM_FINISH, GPIO_TYPE_OUT);  //bram pl rx, ps wr valid

    direction(PL_READ_BRAM_READY, GPIO_TYPE_IN);    //bram pl rx, pl rd rdy         IN
    direction(GPIO_DMA_FIFO_FULL, GPIO_TYPE_IN);    //dma fifo full                 IN
    direction(GPIO_DMA_FIFO_EMPTY, GPIO_TYPE_IN);   //dma fifo empty                IN

    direction(GPIO_ADC_DEBUG_SWITCH, GPIO_TYPE_OUT);//adc_debug_switch  
}
/** @brief GPIO拓展
 *  @param exportPath	GPIO文件路径："/sys/class/gpio/export"			
 *  @param gpioId		gpio 编号	
 *  @return	            exportfd.
 **/
int GPIO::exported(char * exportPath, int gpioId)
{
    char gpioName[10];

    snprintf(gpioName, sizeof(gpioName), "%d", gpioId);

    int exportfd = open(exportPath, O_WRONLY);

    if(exportfd < 0)
    {
        GPIO_DEBUG("GPIO_DEBUG: %sGPIO:%d export filed%s\n", RED, gpioId, NONE);
    }else
    {
        write(exportfd, gpioName, 4);  
        close(exportfd);
        GPIO_DEBUG("GPIO_DEBUG: GPIO:%d export successful\n", gpioId);
    }

    return exportfd;
}

/** @brief 设置GPIO输入输出类型。设置GPIO模式之前需要通过exported()来拓展GPIO引脚。
 *  @param gpioId	    gpio ID.		
 *  @param gpioType    "out":输出； "in":输入.	
 *  @return	            successful:1; failed:-1.
 **/
int GPIO::direction(int gpioId, char *gpioType)
{


    static std::string pathBefore = "/sys/class/gpio/gpio";
    static std::string pathAfter = "/direction";

    std::string path = pathBefore + std::to_string(gpioId) + pathAfter;

    int directionfd = open(path.c_str(), O_RDWR);
    if (directionfd < 0)
    {
        GPIO_DEBUG("GPIO_DEBUG: %sGPIO:%d direction filed%s\n", RED, gpioId, NONE);
        return directionfd;
    }else
    {
        write(directionfd, gpioType, 4);
        close(directionfd);
        GPIO_DEBUG("GPIO_DEBUG: GPIO:%d direction successful\n", gpioId);
        return directionfd;
    }
}

/** @brief 写GPIO.  写数据之前需要通过gpioDirection()将GPIO模式设置位输出，不然会有写入失败的可能。
 *  @param gpioId	    gpio ID.		
 *  @param gpioValue    true:High; false:Low.	
 *  @return	            successful:1; failed:-1.
 **/
int GPIO::set(int gpioId, bool gpioValue)
{
    std::string pathBefore = "/sys/class/gpio/gpio";
    std::string pathAfter = "/value";

    std::string path = pathBefore + std::to_string(gpioId) + pathAfter;

    int gpioValueFd = open(path.c_str(), O_RDWR);

    if (gpioValueFd < 0)
    {
        GPIO_DEBUG("GPIO_DEBUG: %sGPIO:%d set filed%s\n", RED, gpioId, NONE);
        return gpioValueFd;  
    }else
    {
        write(gpioValueFd, std::to_string(gpioValue).c_str(), 2);

        GPIO_DEBUG("GPIO_DEBUG: GPIO:%d set %d successful\n", gpioId, gpioValue);

        close(gpioValueFd);
        return gpioValueFd;
    } 
}


int GPIO::get(int gpioId)
{
    std::string pathBefore = "/sys/class/gpio/gpio";
    std::string pathAfter = "/value";

    std::string path = pathBefore + std::to_string(gpioId) + pathAfter;

    int gpioValueFd = open(path.c_str(), O_RDWR);
    if (gpioValueFd < 0)
    {
        GPIO_DEBUG("GPIO_DEBUG: %sGPIO:%d file open filed%s\n", RED, gpioId, NONE);
        
        return gpioValueFd;  
    }else
    {
        char gpioValue;
        if(read(gpioValueFd, &gpioValue, 2) == -1)
        {
            GPIO_DEBUG("GPIO_DEBUG: %sGPIO:%d get filed%s\n", RED, gpioId, NONE);
            return -1;  
        }else
        {
            char *value = "0";

            GPIO_DEBUG("GPIO_DEBUG: GPIO:%d get %d successful\n", gpioId, gpioValue-*value);
            close(gpioValueFd);

            return gpioValue-*value;
        }   
    }
}
#endif // _COMMON_GPIO_GPIO_HPP_
