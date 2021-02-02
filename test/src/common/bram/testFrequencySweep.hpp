#if !defined(_TEST_FREQUENCY_SWEEP_HPP_)
#define _TEST_FREQUENCY_SWEEP_HPP_

#include "bram.hpp"
#include "iio.hpp"


/*4.3.3 发送遥控配置 BPSK*/


void TEST_FREQUENCY_SWEEP_init()
{
    GPIO *gpio = (GPIO *)GPIO::getInstance();
    gpio->init();

    BRAM *bram = (BRAM *)BRAM::getInstance();
    bram->init(gpio);
    
    IIO *iioTX = (IIO *)IIO::getInstanceTX();

    Baseband::getInstance()->init();

    iioTX->init(149.51, 0);
}

ConfigSendRemoteControlT        configSendRemoteCtrlToBram;         // 发送调制配置

int TEST_FREQUENCY_SWEEP()
{   
    TEST_FREQUENCY_SWEEP_init();

    BRAM *bram = (BRAM *)BRAM::getInstance();


    static uint32_t frequencyOffset = 0;            /*预置频偏*/
    static uint32_t sweepRange = 10000;                 /*扫描范围*/
    static uint32_t sweepVelocity = 10000;              /*扫描速度*/
    static uint32_t sampleFrequency = (uint32_t)1e7;/*样本频率*/

    double dopplerT = 0;
    uint64_t dopplerPinc1 = 0;
    uint64_t dopplerPinc2 = 0;
    uint64_t dopplerPinc3 = 0;

    dopplerT = (double)4 * sweepRange / sweepVelocity;

    dopplerPinc1 = round((double)sweepVelocity * pow(2, 32) / sampleFrequency * pow(2, 48) / sampleFrequency);
        
    dopplerPinc2 = round((double)2 * sweepVelocity / dopplerT * pow(2, 32) / 
                            sampleFrequency * pow(2, 32) / sampleFrequency / sampleFrequency * pow(2, 40));

    dopplerPinc3 = round((double)0.5 / dopplerT * pow(2,64) / sampleFrequency);

    configSendRemoteCtrlToBram.dopplerPinc1H = dopplerPinc1>>32;
    configSendRemoteCtrlToBram.dopplerPinc1L = dopplerPinc1&0x00000000ffffffff;

    configSendRemoteCtrlToBram.dopplerPinc2H = dopplerPinc2>>32;
    configSendRemoteCtrlToBram.dopplerPinc2L = dopplerPinc2&0x00000000ffffffff;

    configSendRemoteCtrlToBram.dopplerPinc3H = dopplerPinc3>>32;
    configSendRemoteCtrlToBram.dopplerPinc3L = dopplerPinc3&0x00000000ffffffff;

    bram->writeBegin();


    return 0;
}



#endif // _TEST_FREQUENCY_SWEEP_HPP_
