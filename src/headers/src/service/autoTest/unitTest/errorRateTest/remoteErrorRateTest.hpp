#if !defined(_REMOTE_ERROR_RATE_TEST_HPP_)
#define _REMOTE_ERROR_RATE_TEST_HPP_


#include <unistd.h>

#include "iio.hpp"
#include "satelliteSimulation.hpp"


#ifndef _REMOTE_ERROR_RATE_DEBUG_
#define R_E_RATE_MSG(...) printf(__VA_ARGS__)
#define R_E_RATE_DEBUG(...) printf(__VA_ARGS__)
#else
#define R_E_RATE_MSG(...)
#define R_E_RATE_DEBUG(...)  
#endif


class RemoteErrorRateTest
{
private:
    uint8_t *buffer = NULL;
    int bufferSize;

    uint8_t *compareData = NULL;
    int compareDataSize;

    IIO *iioTx = NULL;
    SatelliteSimulation * simulation = NULL;

    static volatile RemoteErrorRateTest *instance;
    static pthread_mutex_t mtx;

public:
    RemoteErrorRateTest(/* args */);
    ~RemoteErrorRateTest();

    static volatile RemoteErrorRateTest *getInstance();

    bool init(IIO *iioTx, SatelliteSimulation * simulation);
    
    int setSendData(uint8_t *data, int size);
    int setCompareData(uint8_t *data, int size);

    int run();

    int dataContrast(uint8_t *recvData, int offSet);
    int bitContrast(uint8_t data1, uint8_t data2);
};

volatile RemoteErrorRateTest * RemoteErrorRateTest::instance = NULL;
pthread_mutex_t RemoteErrorRateTest::mtx;

RemoteErrorRateTest::RemoteErrorRateTest(/* args */)
{
}

RemoteErrorRateTest::~RemoteErrorRateTest()
{
}

volatile RemoteErrorRateTest *RemoteErrorRateTest::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new RemoteErrorRateTest();
        }
        pthread_mutex_unlock(&mtx);
    }
    R_E_RATE_DEBUG("R_E_RATE_DEBUG: Get the %sRemoteErrorRateTest instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}

bool RemoteErrorRateTest::init(IIO *iioTx, SatelliteSimulation * simulation)
{
    this->iioTx = iioTx;
    this->simulation = simulation;

    this->bufferSize = 2048;
    this->buffer = (uint8_t *)calloc(this->bufferSize, sizeof(uint8_t));
}


int RemoteErrorRateTest::setSendData(uint8_t *data, int size)
{
    memset(this->buffer, 0x55, this->bufferSize);

    memcpy(this->buffer, data, size);

    return 1;
}

int RemoteErrorRateTest::setCompareData(uint8_t *data, int size)
{

    this->compareDataSize = size;
    this->compareData = data;

    return 1;
}   

int RemoteErrorRateTest::dataContrast(uint8_t *recvData, int offSet)
{
    int count = 0;

    for (int i = 0; i < this->compareDataSize; i++)
    {
        count += bitContrast(this->compareData[i], recvData[i+5]);
    }
    
    return count;
}

int RemoteErrorRateTest::bitContrast(uint8_t data1, uint8_t data2)
{
    static uint8_t xorData;
    xorData = data1 ^ data2;

    int count = 0;

    while(xorData){
        count++;  //只要n不为0则其至少有一个1
        xorData = xorData & (xorData - 1);
    }

    return count;
}

string dataToString(uint8_t *data, int dataLength);

int RemoteErrorRateTest::run()
{
    static int count = 0;

    this->iioTx->send(buffer);

    sleep(1);

    uint8_t * recvData = simulation->send(RemotePoll::commandName);

    R_E_RATE_DEBUG("R_E_RATE_DEBUG: data: %s", dataToString(recvData, 100).c_str());

    count = dataContrast(recvData, 5);

    R_E_RATE_DEBUG("R_E_RATE_DEBUG: error bits: %d\n", count);

    return count;
}

string dataToString(uint8_t *data, int dataLength)
{
    std::stringstream sstream;

    int columns = 8;
    int rows = floor(dataLength / columns); 

    sstream << "data:\n";
    for (int row = 0; row < rows; row++)
    {
        sstream << row <<":\t";
        for (int column = 0; column< columns; column++)
        {
            sstream << "0x" << hex << (int)data[column+row*columns] << ", ";
        }
        sstream << "\n";
    }
    return sstream.str();
    
}



#endif // Remote error rate testing
