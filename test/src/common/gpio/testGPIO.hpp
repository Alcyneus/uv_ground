#if !defined(_TEST_COMMON_GPIO_GPIO_HPP_)
#define _TEST_COMMON_GPIO_GPIO_HPP_



#include "gpio.hpp"


int TEST_COMMON_GPIO()
{
    GPIO *gpio = (GPIO *)GPIO::getInstance();
    gpio->init();

    gpio->set(GPIO_LED0, 0);            //led0(测试)
    gpio->set(GPIO_LED1, 0);            //led1(发射)
    gpio->set(GPIO_LED2, 0);            //led2(发射)
    gpio->set(GPIO_DMA_DBUG, 0);        //dma debug
    gpio->set(GPIO_PL_RESET, 0);        //pl复位
    gpio->set(GPIO_PL_DATA_VALID, 0);    //bram pl tx, pl data valid     IN
    gpio->set(GPIO_PS_READ_START, 0);   //bram pl tx, ps rd start
    gpio->set(GPIO_PS_READ_END, 0);     //bram pl tx, ps rd end
    gpio->set(PS_WRITE_BRAM_BEGIN, 0);  //bram pl rx, ps wr start
    gpio->set(PS_WRITE_BRAM_FINISH, 0);  //bram pl rx, ps wr valid
    gpio->set(PL_READ_BRAM_READY, 0);    //bram pl rx, pl rd rdy         IN
    gpio->set(GPIO_DMA_FIFO_FULL, 0);    //dma fifo full                 IN
    gpio->set(GPIO_DMA_FIFO_EMPTY, 0);   //dma fifo empty                IN
    gpio->set(GPIO_ADC_DEBUG_SWITCH, 0);//adc_debug_switch  

    gpio->get(GPIO_LED0);            //led0(测试)
    gpio->get(GPIO_LED1);            //led1(发射)
    gpio->get(GPIO_LED2);            //led2(发射)
    gpio->get(GPIO_DMA_DBUG);        //dma debug
    gpio->get(GPIO_PL_RESET);        //pl复位
    gpio->get(GPIO_PL_DATA_VALID);    //bram pl tx, pl data valid     IN
    gpio->get(GPIO_PS_READ_START);   //bram pl tx, ps rd start
    gpio->get(GPIO_PS_READ_END);     //bram pl tx, ps rd end
    gpio->get(PS_WRITE_BRAM_BEGIN);  //bram pl rx, ps wr start
    gpio->get(PS_WRITE_BRAM_FINISH);  //bram pl rx, ps wr valid
    gpio->get(PL_READ_BRAM_READY);    //bram pl rx, pl rd rdy         IN
    gpio->get(GPIO_DMA_FIFO_FULL);    //dma fifo full                 IN
    gpio->get(GPIO_DMA_FIFO_EMPTY);   //dma fifo empty                IN
    gpio->get(GPIO_ADC_DEBUG_SWITCH);//adc_debug_switch 


    gpio->set(GPIO_LED0, 1);            //led0(测试)
    gpio->set(GPIO_LED1, 1);            //led1(发射)
    gpio->set(GPIO_LED2, 1);            //led2(发射)
    gpio->set(GPIO_DMA_DBUG, 1);        //dma debug
    gpio->set(GPIO_PL_RESET, 1);        //pl复位
    gpio->set(GPIO_PL_DATA_VALID, 1);    //bram pl tx, pl data valid     IN
    gpio->set(GPIO_PS_READ_START, 1);   //bram pl tx, ps rd start
    gpio->set(GPIO_PS_READ_END, 1);     //bram pl tx, ps rd end
    gpio->set(PS_WRITE_BRAM_BEGIN, 1);  //bram pl rx, ps wr start
    gpio->set(PS_WRITE_BRAM_FINISH, 1);  //bram pl rx, ps wr valid
    gpio->set(PL_READ_BRAM_READY, 1);    //bram pl rx, pl rd rdy         IN
    gpio->set(GPIO_DMA_FIFO_FULL, 1);    //dma fifo full                 IN
    gpio->set(GPIO_DMA_FIFO_EMPTY, 1);   //dma fifo empty                IN
    gpio->set(GPIO_ADC_DEBUG_SWITCH, 1);//adc_debug_switch  

    gpio->get(GPIO_LED0);            //led0(测试)
    gpio->get(GPIO_LED1);            //led1(发射)
    gpio->get(GPIO_LED2);            //led2(发射)
    gpio->get(GPIO_DMA_DBUG);        //dma debug
    gpio->get(GPIO_PL_RESET);        //pl复位
    gpio->get(GPIO_PL_DATA_VALID);    //bram pl tx, pl data valid     IN
    gpio->get(GPIO_PS_READ_START);   //bram pl tx, ps rd start
    gpio->get(GPIO_PS_READ_END);     //bram pl tx, ps rd end
    gpio->get(PS_WRITE_BRAM_BEGIN);  //bram pl rx, ps wr start
    gpio->get(PS_WRITE_BRAM_FINISH);  //bram pl rx, ps wr valid
    gpio->get(PL_READ_BRAM_READY);    //bram pl rx, pl rd rdy         IN
    gpio->get(GPIO_DMA_FIFO_FULL);    //dma fifo full                 IN
    gpio->get(GPIO_DMA_FIFO_EMPTY);   //dma fifo empty                IN
    gpio->get(GPIO_ADC_DEBUG_SWITCH);//adc_debug_switch 
}


#endif // _TEST_COMMON_GPIO_GPIO_HPP_
