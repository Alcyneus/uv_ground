#if !defined(_TEST_UNIT_TEST_CAPTURE_TEST_HPP_)
#define _TEST_UNIT_TEST_CAPTURE_TEST_HPP_

#include <time.h>
#include <unistd.h>
#include "gpio.hpp"

#include "iio.hpp"
#include "captureTest.hpp"
#include "satelliteSimulation.hpp"

#define TEST_PLUTO_IP    "127.0.0.1"
#define TEST_PLUTO_PORT  8080


int CAPTURE_INIT()
{
    Baseband::getInstance()->init();
    IIO *iioTX = (IIO *)IIO::getInstanceTX();
    iioTX->init(149.51, 0);

    GPIO *gpio = (GPIO *)GPIO::getInstance();
    gpio->init();
    
    BRAM *bram = (BRAM *)BRAM::getInstance();
    bram->init(gpio);

    TCP *tcp = (TCP *)TCP::getInstance();
    tcp->init(TEST_PLUTO_IP, TEST_PLUTO_PORT);


    CAN *can0 = (CAN *)CAN::getInstance();
    can0->init("can0", 500000);

    TelemetryPoll *telemetryPoll = (TelemetryPoll *)TelemetryPoll::getInstance();
    telemetryPoll->init(can0);

    SatelliteSimulation *simulation = (SatelliteSimulation *)SatelliteSimulation::getInstance();
    simulation->addCommand(TelemetryPoll::commandName, telemetryPoll);

    CaptureTest *captureTest = (CaptureTest *)CaptureTest::getInstance();
    captureTest->init(iioTX, bram, tcp, simulation);
}



void *iioTXhandler(void *)
{
    IIO *iioTX = (IIO *)IIO::getInstanceTX();
    GPIO *gpio = (GPIO *)GPIO::getInstance();

    uint8_t *sendData = (uint8_t *)calloc(2048, sizeof(uint8_t)); 

    static int count = 0;

    sendData[200] = 0xeb;
    sendData[201] = 0x90;
    sendData[202] = count++;

    static int dataCounter = 1;

    for (size_t i = 203; i < 1904; i+=100)
    {
        memcpy(&(sendData[i]), tm_info_t_buff, 100);
        // data[i+4] = dataCounter++;
    }    

    while (1)
    {
        while (0 == gpio->get(985))
        {
            iioTX->send(sendData);
        }

    }
}

int TEST_CAPTURE()
{
    CAPTURE_INIT();
    pthread_t iiosend;

    pthread_create(&iiosend, NULL, iioTXhandler ,NULL);
    sleep(3);


    ControlInformationData * data = new ControlInformationData();

    data->captureTestH = 0x7d0; //2s wait time;
    data->captureTestL = 0x7d08000; // 2s close;

    data->controlSwitch = 0x0f;
    
    uint8_t *sendData = (uint8_t *)calloc(1024, sizeof(uint8_t));
    memcpy(sendData, data, sizeof(ControlInformationData));


    ControlInformationBean * controlInfo = (ControlInformationBean *)ControlInformationBean::getInstance();
    controlInfo->unpack(sendData);

    CaptureTest *captureTest = (CaptureTest *)CaptureTest::getInstance();


    while (1)
    {
        captureTest->run();
    }
    
    return 0;
}

int TEST_CAPTURE_judgeSignalLock()
{
    CAPTURE_INIT();

    pthread_t iiosend;

    pthread_create(&iiosend, NULL, iioTXhandler ,NULL);

    sleep(3);
    CaptureTest *captureTest = (CaptureTest *)CaptureTest::getInstance();

    // captureTest->closeChannel();
    // sleep(3);
    // captureTest->openChannel();

    while (1)
    {
        sleep(2);
        captureTest->judgeSignalLock();
    }
    
    pthread_join(iiosend, NULL);
}


int TEST_CAPTURE_TEST_issuedORder()
{
    

    CAPTURE_INIT();

    CaptureTest *captureTest = (CaptureTest *)CaptureTest::getInstance();

    ControlInformationBean *controlInfo = (ControlInformationBean *)ControlInformationBean::getInstance();
    ControlInformationData *data = controlInfo->getData();

    data->frameHeader = 0x1;      //01 帧头       0xEB90      2
    data->frameType = 0x2;        //02 帧类型     0x1001      2
    data->frameLength = 0x3;      //03 帧长度     0x000E      2
    data->frameCount = 0x4;       //04 帧计数     0-255       1

    data->controlSwitch = 0x5;    //0x 控制开关   bit32 详情见3.4.1控制信号->控制开关
    data->chanaelSwitch = 0x6;    //05 通道开关               1
    data->remoteSwitch = 0x7;     //06 遥控开关               1
    data->telemetrySwitch = 0x8;  //07 遥测开关               1
    data->testFunction = 0x9;     //08 测试功能控制               1

    data->captureTestH = 0xa;     //09 捕获时间测试 H(高32位，关闭信号时间[byte1~2], 捕获等待时间[byte3~4])    4
    data->captureTestL = 0xb;     //09 捕获时间测试 L(低32位，含测试开关[byte1~2[15]]，锁定稳定时间[byte1~2[14:0])    2

    data->workStatus = 0xc;       //10 工作状态                   1
    data->statusOut = 0xd;        //11 状态输出控制               2
    data->checkSum = 0xe;         //12 校验和  

    // 01111
    data->controlSwitch = 0x0f;

    captureTest->issuedOrder(data->controlSwitch);

    while (1)
    {
        sleep(3);
        data->controlSwitch = 0x0f;
        captureTest->issuedOrder(data->controlSwitch);

        sleep(3);    
        data->controlSwitch = 0xd;
        captureTest->issuedOrder(data->controlSwitch);
    }

}   


#endif // _TEST_UNIT_TEST_CAPTURE_TEST_HPP_
