#if !defined(_UNIT_TEST_CAPTURE_TEST_HPP_)
#define _UNIT_TEST_CAPTURE_TEST_HPP_

#include "satelliteSimulation.hpp"

#include "tcp.hpp"
#include "iio.hpp"
#include "bram.hpp"

#include "controlInformationBean.hpp"
#include "captureTestTimeBean.hpp"

#ifndef _CAPTURE_TEST_DEBUG_
#define CAPTURE_TEST_MSG(...) printf(__VA_ARGS__)
#define CAPTURE_TEST_DEBUG(...) printf(__VA_ARGS__)
#else
#define CAPTURE_TEST_MSG(...)
#define CAPTURE_TEST_DEBUG(...)  
#endif



#define SIGNAL_LOCK         1
#define SIGNAL_LOST_LOCK    0  

class CaptureTest
{
private:

    IIO *iioTX = NULL;
    BRAM *bram = NULL;
    TCP *tcp = NULL;

    SatelliteSimulation * simulation = NULL;

    ControlInformationBean * controlInfo = NULL;

    CaptureTestTimeBean * captureTestTime = NULL;

    CaptureTestTimeT *recordTime;


    bool locked = false;

    static volatile CaptureTest *instance;
    static pthread_mutex_t mtx;
public:
    CaptureTest(/* args */);
    ~CaptureTest();

    static volatile CaptureTest *getInstance();

    void init(IIO *iioTX, BRAM *bram, TCP *tcp, SatelliteSimulation * simulation);
    int issuedOrder(uint32_t controlSwitch);

    int closeChannel();
    int openChannel();
    int judgeSignalLock();
    int remainCaptureTime();

    int send();

    int run();

    
    
};

volatile CaptureTest * CaptureTest::instance = NULL;
pthread_mutex_t CaptureTest::mtx;

CaptureTest::CaptureTest(/* args */)
{
}

CaptureTest::~CaptureTest()
{
}

volatile CaptureTest *CaptureTest::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new CaptureTest();
        }
        pthread_mutex_unlock(&mtx);
    }
    CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: Get the %sCaptureTest instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}

void CaptureTest::init(IIO *iioTX, BRAM *bram, TCP *tcp, SatelliteSimulation * simulation)
{
    this->iioTX = iioTX;
    this->bram  = bram;
    this->tcp = tcp;

    this->simulation = simulation;

    this->controlInfo = (ControlInformationBean *)ControlInformationBean::getInstance();

    this->captureTestTime = (CaptureTestTimeBean *)CaptureTestTimeBean::getInstance();
    this->recordTime = new CaptureTestTimeT();
}

int CaptureTest::issuedOrder(uint32_t controlSwitch)
{
    if(NULL == this->bram)
    {
        CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: %sCaptureTest instance: %d bram uninitialized !%s\n",RED, instance,NONE);
        return 0;
    }

    static uint32_t phyaddr = BRAM_MAP_ADDR_TX + 0x0004;

    bram->writeBegin();
    bram->write(phyaddr, 1);
    bram->write(phyaddr+4, controlSwitch);
    bram->write(phyaddr+4*2, 0);
    bram->writeFinish();

    return 1;
}

int CaptureTest::run()
{
    closeChannel();
    sleep(this->controlInfo->getOffSignalTime() / 1e3);

    openChannel();

    while (remainCaptureTime())
    {
        while((SIGNAL_LOCK == judgeSignalLock()) && remainCaptureTime())
        {
            usleep(5000);
            CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: lock.\n");
        }

        while (SIGNAL_LOST_LOCK == judgeSignalLock() && remainCaptureTime()) 
        {
            usleep(5000);
            CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: lost lock.\n");
        }
    }

    send();
    this->locked = false;
    sleep(5);
}

int CaptureTest::closeChannel()
{
    uint32_t controlSwitch = this->controlInfo->getData()->controlSwitch;
    // controlSwitch &= 0xfffd;

    controlSwitch = 0x0d;
    int revalue = issuedOrder(controlSwitch);

    if(revalue)
    {
        gettimeofday(&(this->recordTime->channelCloseTime),NULL);

        
        CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: close channel.\n");
        CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: time: %ds, %dus\n",
                            this->recordTime->channelCloseTime.tv_sec, this->recordTime->channelCloseTime.tv_usec);
    }
    
    return revalue; 
}

int CaptureTest::openChannel()
{
    uint32_t controlSwitch = this->controlInfo->getData()->controlSwitch;
    // controlSwitch |= 0x0002;

    controlSwitch = 0x0f;
    int revalue = issuedOrder(controlSwitch);

    if(revalue)
    {
        gettimeofday(&(this->recordTime->channelOpenTime),NULL);
        CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: open channel.\n");
        CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: time: %ds, %dus\n",
                            this->recordTime->channelOpenTime.tv_sec, this->recordTime->channelOpenTime.tv_usec);
    }
    
    return revalue; 
}

int CaptureTest::judgeSignalLock()
{
    static uint16_t dataLength = 0x26;
    uint8_t *telemetryData = NULL;

    telemetryData = this->simulation->send(TelemetryPoll::commandName);

    if(!telemetryData)
    {
        CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: %sno telemetry data!%s\n",RED ,NONE);
        return -1;
    }

    uint16_t recvDataLength = telemetryData[UV_COMMUNICAT_DATA_LENGTH_BIT];
    recvDataLength = recvDataLength << 8 |telemetryData[UV_COMMUNICAT_DATA_LENGTH_BIT+1];

    if (dataLength != recvDataLength)
    {
        CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: %sreceive data length=%d  != 0x26!%s\n",RED,recvDataLength ,NONE);
        return -1;
    }

    uint8_t lockState = (telemetryData[9] >> 5) & 0x01;

    if(lockState)
    {
        if(!locked)
        {
            gettimeofday(&(this->recordTime->lockTime),NULL);
            CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: %s Signal to lock [%x] %s\n", YELLOW, lockState, NONE);
            CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: time: %ds, %dus\n",
                            this->recordTime->lockTime.tv_sec, this->recordTime->lockTime.tv_usec);
        }

        locked = true;
    }else
    {

        if(locked)
        {
            gettimeofday(&(this->recordTime->lostLockTime),NULL);
            CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: %s Signal loss of lock [%x] %s\n", RED, lockState, NONE);
            CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: time: %ds, %dus\n",
                            this->recordTime->lostLockTime.tv_sec, this->recordTime->lostLockTime.tv_usec);
        }   
    }

    return lockState; 
}

int CaptureTest::remainCaptureTime()
{
    static timeval currentTime;
    gettimeofday(&currentTime,NULL);

    double pastTime = (double)(currentTime.tv_sec - this->recordTime->channelOpenTime.tv_sec)
                    +(currentTime.tv_usec - this->recordTime->channelOpenTime.tv_usec)/1e6;
    double captureWaitTime = (double)this->controlInfo->getCaptureWaitTime() / 1e3;

    if((captureWaitTime-pastTime) > 0)
    {
        CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG:time remaining: %f s\n", captureWaitTime-pastTime);
    }

    return ((captureWaitTime-pastTime) > 0);
}

int CaptureTest::send()
{
    gettimeofday(&(this->recordTime->sendTime), NULL);
    CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: %s send time.%s\n", YELLOW, NONE);
    CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG: time: %ds, %dus\n",
                            this->recordTime->sendTime.tv_sec, this->recordTime->sendTime.tv_usec);

    captureTestTime->setData(this->recordTime);


    tcp->upload(captureTestTime);
    CAPTURE_TEST_DEBUG("CAPTURE_TEST_DEBUG:%s times: %s %s\n", YELLOW, this->captureTestTime->toString().c_str(), NONE);
}
#endif // _UNIT_TEST_CAPTURE_TEST_HPP_
