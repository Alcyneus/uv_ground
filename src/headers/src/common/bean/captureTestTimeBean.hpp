#if !defined(_CAPTURE_TEST_TIME_BEAN_H)
#define _CAPTURE_TEST_TIME_BEAN_H

/**
 * @file captureTestTimeBean.hpp
 * @author ls 
 * @brief  捕获时间数据对象
 * @version 0.1
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "bean.hpp"

#include <unistd.h>
#include <sys/time.h>

typedef struct CaptureTestTime
{
    timeval channelCloseTime;       
    timeval channelOpenTime;
    timeval lockTime;
    timeval lostLockTime;
    timeval sendTime;
}CaptureTestTimeT;

typedef struct CaptureTestTimeSourceData
{
    uint32_t frameHeader;           //01 帧头       0xEB90      2
    uint32_t frameType;             //02 帧类型     0x1002       2
    uint32_t frameLength;           //03 帧长度     0x0023      2
    uint32_t frameCount;            //04 帧计数     0-255       1
    uint32_t channelCloseTime;          
    uint32_t channelOpenTime;
    uint32_t lockTime;
    uint32_t lostLockTime;
    uint32_t sendTime;

}CaptureTestTimeData;


class CaptureTestTimeBean:public Bean
{
private:
    CaptureTestTimeData *data;

    static volatile CaptureTestTimeBean *instance;
    static pthread_mutex_t mtx;
public:
    CaptureTestTimeBean(/* args */);
    ~CaptureTestTimeBean();

    static volatile CaptureTestTimeBean *getInstance();

    CaptureTestTimeData *getData();

    void setData(CaptureTestTimeData *data);
    void setData(CaptureTestTimeT *data);
    void setData(uint8_t *data);

    int pack(uint8_t *buffer);
    int unpack(uint8_t *buffer);


    void timevalToUint32(CaptureTestTimeData* data, CaptureTestTimeT* time);
    string toString();
};

CaptureTestTimeBean::CaptureTestTimeBean(/* args */)
{
    data = new CaptureTestTimeData();
}

CaptureTestTimeBean::~CaptureTestTimeBean()
{
    delete data;
}

volatile CaptureTestTimeBean * CaptureTestTimeBean::instance = NULL;
pthread_mutex_t CaptureTestTimeBean::mtx;

volatile CaptureTestTimeBean *CaptureTestTimeBean::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new CaptureTestTimeBean();
        }
        pthread_mutex_unlock(&mtx);
    }
    
    return instance;
}

int CaptureTestTimeBean::pack(uint8_t *buffer)
{
    memcpy(buffer, this->data, sizeof(CaptureTestTimeData));
}

int CaptureTestTimeBean::unpack(uint8_t *buffer)
{
    
}

CaptureTestTimeData *CaptureTestTimeBean::getData()
{
    if(NULL != data)
    {
        return data;
    }
}
void CaptureTestTimeBean::setData(CaptureTestTimeData *data)
{   
    memcpy(this->data, data, sizeof(*(this->data)));
}
void CaptureTestTimeBean::setData(CaptureTestTimeT *data)
{
    timevalToUint32(this->data, data);
}

void CaptureTestTimeBean::setData(uint8_t *data)
{   
    memcpy(this->data, data, sizeof(*(this->data)));
}

void CaptureTestTimeBean::timevalToUint32(CaptureTestTimeData *data, CaptureTestTimeT *time)
{
    data->channelCloseTime = (time->channelCloseTime.tv_sec % 1000)*1e6 + time->channelCloseTime.tv_usec;
    data->channelOpenTime = (time->channelOpenTime.tv_sec % 1000)*1e6 + time->channelOpenTime.tv_usec;
    data->lockTime = (time->lockTime.tv_sec % 1000)*1e6 + time->lockTime.tv_usec;
    data->lostLockTime = (time->lostLockTime.tv_sec % 1000)*1e6 + time->lostLockTime.tv_usec;
    data->sendTime = (time->sendTime.tv_sec % 1000)*1e6 + time->sendTime.tv_usec;

}
string CaptureTestTimeBean::toString()
{
    stringstream sstream;

    sstream << "CaptureTestTimeBean{\n" 
            << "frameHeader=0x" << (long int)data->frameHeader
            << ", frameType=0x" << (long int)data->frameType
            << ", frameLength=0x" << (long int)data->frameLength
            << ", frameCount=0x" << (long int)data->frameCount
            << "\nchannelCloseTime=0x" << (long int)data->channelCloseTime
            << "us, channelOpenTime=0x" << (long int)data->channelOpenTime
            << "us\nlockTime=0x" << (long int)data->lockTime
            << "us, lostLockTime=0x" << (long int)data->lostLockTime
            << "us, sendTime=0x" << (long int)data->sendTime
            << "\n}";

    return sstream.str();
}
#endif // _CAPTURE_TEST_TIME_BEAN_H
