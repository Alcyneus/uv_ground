#if !defined(_TELEMETRY_ERROR_RATE_TEST_HPP_)
#define _TELEMETRY_ERROR_RATE_TEST_HPP_

#include <unistd.h>

#include "iio.hpp"
#include "satelliteSimulation.hpp"
#include "bitSync.hpp"

#include "debug.hpp"

class TelemetryErrorRateTest
{
private:

    IIO *iioRx = NULL;
    SatelliteSimulation * simulation = NULL;

    BitSync *bitSync = NULL;

    static volatile TelemetryErrorRateTest *instance;
    static pthread_mutex_t mtx;
public:
    TelemetryErrorRateTest(/* args */);
    ~TelemetryErrorRateTest();

    static volatile TelemetryErrorRateTest *getInstance();

    void init(IIO *iioRx, SatelliteSimulation *simulation);
    void run(uint8_t data);
};

volatile TelemetryErrorRateTest * TelemetryErrorRateTest::instance = NULL;
pthread_mutex_t TelemetryErrorRateTest::mtx;

TelemetryErrorRateTest::TelemetryErrorRateTest(/* args */)
{
}

TelemetryErrorRateTest::~TelemetryErrorRateTest()
{
}

volatile TelemetryErrorRateTest *TelemetryErrorRateTest::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new TelemetryErrorRateTest();
        }
        pthread_mutex_unlock(&mtx);
    }
    T_L_RATE_DEBUG("T_L_RATE_DEBUG: Get the %sTelemetryErrorRateTest instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}

void TelemetryErrorRateTest::init(IIO *iioRx, SatelliteSimulation *simulation)
{
    this->iioRx = iioRx;
    this->simulation = simulation;

    this->bitSync = (BitSync *)BitSync::getInstance();
    bitSync->init(2048, 2);
}

void TelemetryErrorRateTest::run(uint8_t data)
{
    //1.模拟星务发送遥控数据
    //2.IIO 接收遥控数据
    //3.位同步
    
}
#endif // _TELEMETRY_
