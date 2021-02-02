#if !defined(TEST_REMOTE_ERROR_RATE_TEST_HPP_)
#define TEST_REMOTE_ERROR_RATE_TEST_HPP_

#include "remoteErrorRateTest.hpp"


static uint8_t testData[256]= {	
	0xDD,0x5C,0x69,0xFE,0x4A,0xD,0xC4,0x9F,
	0xB,0x77,0xB4,0x87,0x26,0x98,0xA1,0xAA,
	0xB9,0x49,0xDE,0x4B,0x85,0x6E,0xD8,0x27,
	0xB4,0xA8,0x26,0x13,0xA,0xEA,0x94,0x89,
	0xD3,0xF5,0x91,0xDF,0xB2,0x3E,0xA5,0x11,
	0x3C,0xC6,0x51,0x35,0x73,0xC,0x77,0x62,
	0x40,0xA2,0xAC,0x84,0x1C,0xC8,0xAC,0x50,
	0x73,0x49,0x44,0x2D,0x1B,0xD8,0x2D,0x14,
	0xA5,0xEA,0x23,0x3,0x37
};

static uint8_t compareData[256]= {	 
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
	0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,
    0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
	0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,
    0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
	0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,
    0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
	0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,
    0x2b
    
};

int REMOTE_ERROR_RATE_INIT()
{

    GPIO *gpio = (GPIO *)GPIO::getInstance();
    gpio->init();
    
    CAN *can0 = (CAN *)CAN::getInstance();
    can0->init("can0", 500000);

    RemotePoll *remotePoll = (RemotePoll *)RemotePoll::getInstance();
    remotePoll->init(can0);

    SatelliteSimulation *simulation = (SatelliteSimulation *)SatelliteSimulation::getInstance();
    simulation->addCommand(RemotePoll::commandName, remotePoll);

    Baseband::getInstance()->init();
    IIO *iioTx = (IIO *)IIO::getInstanceTX();
    iioTx->init(149.51, 0);

    RemoteErrorRateTest *remoteTest = (RemoteErrorRateTest *)RemoteErrorRateTest::getInstance();
    remoteTest->init(iioTx, simulation);
}


int REMOTE_ERROR_RATE_TEST()
{
    REMOTE_ERROR_RATE_INIT();

    int size = 2048;

    uint8_t *data = (uint8_t *)calloc(size, sizeof(uint8_t));

    RemoteErrorRateTest *remoteTest = (RemoteErrorRateTest *)RemoteErrorRateTest::getInstance();
    IIO *iioTx = (IIO *)IIO::getInstanceTX();
    GPIO *gpio = (GPIO *)GPIO::getInstance();
    if (0)
    {
        memset(data, 0x00, size);
    }else
    {
        R_E_RATE_DEBUG("for (size_t i = 203; i < 1904; i+=100)");
        static int count = 0;

        data[200] = 0xeb;
        data[201] = 0x90;
        data[202] = count++;

        static int dataCounter = 1;

        memcpy(&(data[203]), testData, 100);

        // for (size_t i = 203; i < 1904; i+=100)
        // {
        //     memcpy(&(data[i]), testData, 100);
        //     // data[i+4] = dataCounter++;
        // }
    }


    R_E_RATE_DEBUG("remoteErrorRateTest::getInstance();\n");
    remoteTest->setSendData(data, 2048);


    remoteTest->setCompareData(compareData, 65);

    R_E_RATE_DEBUG("remoteTest->setData(data, 2048);\n");

    int bitCount = 0;
    int loopCount = 0;
    double errorRate = 0;
    while (1)
    {
        bitCount += remoteTest->run();
        loopCount++;

        errorRate = bitCount / loopCount *65*8;
        
        R_E_RATE_DEBUG("%sbitCount: %d, errorRate: %f, loopCount: %d %s\n",
                         YELLOW, bitCount, errorRate, loopCount, NONE);
    }

}



#endif // TEST_REMOTE_ERROR_RATE_TEST_HPP_
