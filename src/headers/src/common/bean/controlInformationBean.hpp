#if !defined(_CONTROL_INFORMATION_BEAN_HPP_)
#define _CONTROL_INFORMATION_BEAN_HPP_
/**
 * @file controlInformationBean.hpp
 * @author ls 
 * @brief  控制信息数据对象
 * @version 0.1
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "bean.hpp"
#include "debug.hpp"

/*4.2.1 控制信息*/
// typedef struct ControlInformationSourceData
// {
//     uint32_t frameHeader;           //01 帧头       0xEB90      2
//     uint32_t frameType;             //02 帧类型     0x1001      2
//     uint32_t frameLength;           //03 帧长度     0x000E      2
//     uint32_t frameCount;            //04 帧计数     0-255       1
//     uint32_t chanaelSwitch;         //05 通道开关               1
//     uint32_t remoteSwitch;          //06 遥控开关               1
//     uint32_t telemetrySwitch;       //07 遥测开关               1
//     uint32_t testFunction;          //08 测试功能控制               1
//     uint32_t captureTestH;          //09 捕获时间测试 H(高32位，关闭信号时间[byte1~2], 捕获等待时间[byte3~4])    4
//     uint32_t captureTestL;          //09 捕获时间测试 L(低32位，含测试开关[byte1~2[15]]，锁定稳定时间[byte1~2[14:0])    2
//     uint32_t workStatus;            //10 工作状态                   1
//     uint32_t statusOut;             //11 状态输出控制               2
//     uint32_t checkSum;              //12 校验和                     1
// }ControlInformationData;

typedef struct ControlInformationSourceData
{
    uint32_t frameHeader;           //01 帧头       0xEB90      2
    uint32_t frameType;             //02 帧类型     0x1001      2
    uint32_t frameLength;           //03 帧长度     0x000E      2
    uint32_t frameCount;            //04 帧计数     0-255       1

    uint32_t controlSwitch;         //0x 控制开关   bit32 详情见3.4.1控制信号->控制开关
    uint32_t chanaelSwitch;         //05 通道开关               1
    uint32_t remoteSwitch;          //06 遥控开关               1
    uint32_t telemetrySwitch;       //07 遥测开关               1
    uint32_t testFunction;          //08 测试功能控制               1

    uint32_t captureTestH;          //09 捕获时间测试 H(高32位，关闭信号时间[byte1~2], 捕获等待时间[byte3~4])    4
    uint32_t captureTestL;          //09 捕获时间测试 L(低32位，含测试开关[byte1~2[15]]，锁定稳定时间[byte1~2[14:0])    2

    uint32_t workStatus;            //10 工作状态                   1
    uint32_t statusOut;             //11 状态输出控制               2
    uint32_t checkSum;              //12 校验和                     1
}ControlInformationData;



class ControlInformationBean
{
private:
    ControlInformationData *data;

    bool testSwitch;
    uint16_t stableLockTime;
    uint16_t offSignalTime;
    uint16_t captureWaitTime;

    static volatile ControlInformationBean *instance;
    static pthread_mutex_t mtx;
public:
    ControlInformationBean(/* args */);
    ~ControlInformationBean();

    static volatile ControlInformationBean *getInstance();
    ControlInformationData *getData();

    int unpack(uint8_t *data);

    bool getTestSwitch();
    uint16_t getStableLockTime();
    uint16_t getOffSignalTime();
    uint16_t getCaptureWaitTime();

    string toString();
};

ControlInformationBean::ControlInformationBean(/* args */)
{
    data = new ControlInformationData();
}

ControlInformationBean::~ControlInformationBean()
{
    delete data;
}
volatile ControlInformationBean * ControlInformationBean::instance = NULL;
pthread_mutex_t ControlInformationBean::mtx;

volatile ControlInformationBean *ControlInformationBean::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new ControlInformationBean();
        }
        pthread_mutex_unlock(&mtx);
        BEAN_DEBUG("BEAN_DEBUG: Get the %sControlInformationBean instance: %d%s\n",GREEN, instance,NONE);
    }
    
    return instance;
}

int ControlInformationBean::unpack(uint8_t *data)
{
    memcpy(this->data, data, sizeof(ControlInformationData));

    this->testSwitch = (this->data->captureTestL & 0x8000) >> 15;
    this->stableLockTime = this->data->captureTestL & 0x7fff;
    this->offSignalTime = this->data->captureTestL >> 16;
    this->captureWaitTime = this->data->captureTestH;

    BEAN_DEBUG("BEAN_DEBUG:%s testSwitch=%d %s \n",YELLOW, this->testSwitch,NONE );
    BEAN_DEBUG("BEAN_DEBUG:%s stableLockTime=%d %s \n",YELLOW, this->stableLockTime,NONE );
    BEAN_DEBUG("BEAN_DEBUG:%s offSignalTime=%d %s \n",YELLOW, this->offSignalTime,NONE );
    BEAN_DEBUG("BEAN_DEBUG:%s captureWaitTime=%d %s \n",YELLOW, this->captureWaitTime,NONE );
}

ControlInformationData *ControlInformationBean::getData()
{
    BEAN_DEBUG("BEAN_DEBUG: Get the %sControlInformationBean data: %d%s\n",GREEN, instance,NONE);
    return data;
}

bool ControlInformationBean::getTestSwitch()
{
    return this->testSwitch;
}
uint16_t ControlInformationBean::getStableLockTime()
{
    return this->stableLockTime;
}
uint16_t ControlInformationBean::getOffSignalTime()
{
    return this->offSignalTime;
}
uint16_t ControlInformationBean::getCaptureWaitTime()
{
    return this->captureWaitTime;
}


string ControlInformationBean::toString()
{
    stringstream sstream;

    sstream << "ControlInformationBean{\n" 
            << "frameHeader=0x" << hex << (long int)data->frameHeader
            << ", frameType=0x" << hex << (long int)data->frameType
            << ", frameLength=0x" << hex << (long int)data->frameLength
            << ", frameCount=0x" << hex << (long int)data->frameCount
            << "\n"
            << "controlSwitch=0x" << hex << (long int)data->controlSwitch
            << ", chanaelSwitch=0x" << hex << (long int)data->chanaelSwitch
            << ", remoteSwitch=0x" << hex << (long int)data->remoteSwitch
            << ", telemetrySwitch=0x" << hex << (long int)data->telemetrySwitch
            << "\n"
            << "testFunction=0x" << hex << (long int)data->testFunction
            << ", captureTestH=0x" << hex << (long int)data->captureTestH
            << ", captureTestL=0x" << hex << (long int)data->captureTestL
            << "\n"
            << "workStatus=0x" << hex << (long int)data->workStatus
            << ", statusOut=0x" << hex << (long int)data->statusOut
            << ", checkSum=0x" << hex << (long int)data->checkSum
            << "\n}";

    return sstream.str();
}

#endif // _CONTROL_INFORMATION_BEAN_HPP_
