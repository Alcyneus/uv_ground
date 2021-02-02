#if !defined(_TEST_COMMON_IIO_IIO_HPP_)
#define _TEST_COMMON_IIO_IIO_HPP_

#include "iio.hpp"
#include "bitSync.hpp"

#include "synchronous.h"

#include "sync.hpp"

static uint8_t tm_info_t_buff[256]= {	
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


#define TEST_DATA_S 301
static uint8_t testDatas[TEST_DATA_S] = {0};

void *sendHandler(void *);

int TEST_COMMON_IIO()
{
    GPIO *gpio = (GPIO *)GPIO::getInstance();
    gpio->init();
    
    IIO *iioTX = (IIO *)IIO::getInstanceTX();
    IIO *iioRX = (IIO *)IIO::getInstanceRX();

    Baseband::getInstance()->init();

   
    iioTX->init(149.51, 0);
    iioRX->init(149.51, 0);

    uint8_t *sendData = (uint8_t *)calloc(2048, sizeof(uint8_t)); 
    uint8_t *recvData = (uint8_t *)calloc(2048, sizeof(uint8_t)); 
    if (1)
    {
        memset(sendData, 0x00, 2048);
    }else
    {
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
    }

    IIO_DEBUG("TEST_IIO: INPUT GAIN:\n>>");
    double gain;
    cin >> gain;
    iioTX->setTXGain(gain);

    while (1)
    {
        while (0 == gpio->get(985))
        {
            iioTX->send(sendData);
            // iioRX->recv(recvData);
        }
    }
    

    
    

    return 0;
}


int TEST_COMMON_IIO_GAIN()
{
    GPIO *gpio = (GPIO *)GPIO::getInstance();
    gpio->init();
    
    IIO *iioTX = (IIO *)IIO::getInstanceTX();
    IIO *iioRX = (IIO *)IIO::getInstanceRX();

    Baseband::getInstance()->init();

   
    iioTX->init(149.51, 0);
    iioRX->init(149.51, 0);

    uint8_t *sendData = (uint8_t *)calloc(2048, sizeof(uint8_t)); 
    uint8_t *recvData = (uint8_t *)calloc(2048, sizeof(uint8_t)); 

    IIO_DEBUG("TEST_IIO: DATA TYPE: 0:0 1:RANDAM\n>>");
    int dataType;
    cin >> dataType;

    if (dataType)
    {
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
    }else
    {
        memset(sendData, 0x00, 2048);
    }

    IIO_DEBUG("TEST_IIO: INPUT GAIN:\n>>");
    double gain;
    cin >> gain;
    iioTX->setTXGain(gain);

    while (1)
    {
        static int sendCounter = 0;

        while (0 == gpio->get(985))
        {
            iioTX->send(sendData);
        }
    }
    
    return 0;
}


int TEST_IIO_RECV()
{

    BitSync *bitSync = (BitSync *)BitSync::getInstance();

    bitSync->init(2048, 2);

    uint8_t *head = (uint8_t *)calloc(2, sizeof(uint8_t));
    head[0] = 0xdd;
    head[1] = 0x5c;


    GPIO *gpio = (GPIO *)GPIO::getInstance();
    gpio->init();
    
    IIO *iioTX = (IIO *)IIO::getInstanceTX();
    IIO *iioRX = (IIO *)IIO::getInstanceRX();

    Baseband::getInstance()->init();

    iioTX->init(149.51, 0);
    iioRX->init(401.69, 0);

    uint8_t *recvData = (uint8_t *)calloc(2048, sizeof(uint8_t)); 

    pthread_t sendHandl;

    pthread_create(&sendHandl, NULL, sendHandler, NULL);

    while (1)
    {
        iioRX->recv(recvData);
        IIO_DEBUG("data[%d] = 0x%x\n", 10, recvData[10]);
        
        // for (int i = 0; i < 2048; i++)
        // {
        //     if((recvData[i] != 0x00) && (recvData[i] != 0xff) && (recvData[i] != 0x55))
        //     {
        //         IIO_DEBUG("data[%d] = 0x%x, ", i, recvData[i]);
        //     }
            
        // }

        // sleep(3);
        

        // IIO_DEBUG("TEST_IIO: head: 0xdd5c in ");
        // head[0] = 0xdd;
        // head[1] = 0x5c;
        // bitSync->run(recvData, head);

        // IIO_DEBUG("TEST_IIO: head: 0xdd5c in ");
        // head[0] = 0x22;
        // head[1] = 0xa3;
        // bitSync->run(recvData, head);
    }



    
    return 0;
}


void *sendHandler(void *)
{
    IIO *iioTX = (IIO *)IIO::getInstanceTX();

    uint8_t *sendData = (uint8_t *)calloc(2048, sizeof(uint8_t));  

    memset(sendData, 0x55, 2048);

    for (int i = 0; i < 100; i++)
    {
        testDatas[i] = i;
    }
    for (int i = 0; i < 100; i++)
    {
        testDatas[i+100] = i;
    }

    for (int i = 0; i < 101; i++)
    {
        testDatas[i+200] = i;
    }

    if (1)
    {
        static int count = 0;

        /*frame head*/
        sendData[400] = 0xeb;
        sendData[401] = 0x90;
        sendData[402] = 0x10;
        sendData[403] = 0x06;
        sendData[404] = 0x01;
        sendData[405] = 0x2c;
        sendData[406] = 0x01;
        
        /*frame data*/
        // memset(&(sendData[407]), 0x12, 100);
        // memset(&(sendData[307]), 0x12, 100);
        // memset(&(sendData[407]), 0x12, 100);
        // sendData[508] = 0xdd;

        memcpy(&(sendData[207]), testDatas, TEST_DATA_S);

        /*frame head*/
        sendData[800] = 0xeb;
        sendData[801] = 0x90;
        sendData[802] = 0x10;
        sendData[803] = 0x06;
        sendData[804] = 0x01;
        sendData[805] = 0x2c;
        sendData[806] = 0x02;
        
        /*frame data*/

        // memset(&(sendData[607]), 0x12, 100);
        // memset(&(sendData[707]), 0x12, 100);
        // memset(&(sendData[807]), 0x12, 100);
        // sendData[908] = 0xee;
        memcpy(&(sendData[807]), testDatas, TEST_DATA_S);
        /*frame head*/
        int offset = 50;
        sendData[1200] = 0xeb;
        sendData[1201] = 0x90;
        sendData[1202] = 0x12;
        sendData[1203] = 0x06;
        sendData[1204] = 0x01;
        sendData[1205] = 0x2c;
        sendData[1206] = 0x03;
        
        /*frame data*/
        // memset(&(sendData[1007]), 0xcc, 100);
        // memset(&(sendData[1107]), 0xcc, 100);
        // memset(&(sendData[1207]), 0xcc, 100);
        // sendData[1308] = 0xff;
        memcpy(&(sendData[1207]), testDatas, TEST_DATA_S);
    }else
    {
        memset(sendData, 0x00, 2048);
    }

    int i = 1;
    while (1)
    {

        while (1)
        {
            sendData[406] = i++;
            sendData[806] = i++;
            sendData[1206] = i++;
            iioTX->send(sendData);
            sleep(4);
        }
    }
    
}

string RemoteDataToString(uint8_t *data, int dataLength);

int TEST_IIO_BIT_SYNC()
{
    

    uint8_t *head = (uint8_t *)calloc(2, sizeof(uint8_t));
    head[0] = 0xdd;
    head[1] = 0x5c;


    GPIO *gpio = (GPIO *)GPIO::getInstance();
    gpio->init();
    
    IIO *iioTX = (IIO *)IIO::getInstanceTX();
    IIO *iioRX = (IIO *)IIO::getInstanceRX();

    Baseband::getInstance()->init();

    iioTX->init(149.51, 0);
    iioRX->init(401.69, 0);


    uint8_t *recvdata = (uint8_t *)calloc(2048, sizeof(uint8_t)); 

    uint8_t *frameCount = (uint8_t *)calloc(200, sizeof(uint8_t)); 

    pthread_t sendHandl;

    pthread_create(&sendHandl, NULL, sendHandler, NULL);


    BitSync *bitSync = (BitSync *)BitSync::getInstance();
    bitSync->init(2048, 2);
    
    int index = 0;

    int loop = 1;

    SYNC *sync = new SYNC();
    sync->init(2048, 2);


    head[0] = 0xeb;
    head[1] = 0x90;
    sync->setHead(head);

    
    while (1)
    {
        sync->run(recvdata);
        BIT_SYNC_DEBUG("%s%s%s\n", RED, RemoteDataToString(recvdata, 2048).c_str(), NONE);

        // BIT_SYNC_DEBUG("%s%s%s\n", RED, RemoteDataToString(recvdata, 308+7).c_str(), NONE);
        // BIT_SYNC_DEBUG("%s%s%s\n", RED, RemoteDataToString(&(recvdata[308]), 308+7).c_str(), NONE);
        // BIT_SYNC_DEBUG("%s%s%s\n", RED, RemoteDataToString(&(recvdata[308*2]), 308+7).c_str(), NONE);

        // BIT_SYNC_DEBUG("%sframe1:%d, frame2:%d, frame3:%d%s\n",LUE, recvdata[6], recvdata[314], recvdata[622], NONE);

        // sleep(2);
        SYNC_DEBUG("TEST_SYNC: %sloop: %d\n%s", RED, loop++, NONE);

        if(0 != recvdata[6]){frameCount[index++] = recvdata[6];}
        if(0 != recvdata[314]){frameCount[index++] = recvdata[314];}
        if(0 != recvdata[622]){frameCount[index++] = recvdata[622];}

        if(index > 100)
        {
            BIT_SYNC_DEBUG("%s\n", RemoteDataToString(frameCount, 100).c_str());
            break;
        }
    }
    


    while (0)
    {
        iioRX->recv(recvdata);

        bitSync->setData(recvdata);

        BIT_SYNC_DEBUG("TEST_IIO: head: 0x146f in ");
        head[0] = 0x14;
        head[1] = 0x6f;
        bitSync->setFrameHead(head);

        head[0] = 0xef;
        head[1] = 0xf9;
        bitSync->setFrameType(head);

        index = bitSync->run(recvdata);

        if(index != -1)
        {
            uint16_t dataLength = recvdata[4];
            dataLength = dataLength << 8 | recvdata[5];
            dataLength = 65535 - dataLength + 8;

            BIT_SYNC_DEBUG("%s\n", RemoteDataToString(recvdata, 32).c_str());
            BIT_SYNC_DEBUG("%s\n", RemoteDataToString(&(recvdata[dataLength-7]), 32).c_str());
            BIT_SYNC_DEBUG("%s\n", RemoteDataToString(&(recvdata[(dataLength-7)*2]), 32).c_str());
        }


        BIT_SYNC_DEBUG("TEST_IIO: head: 0xeb90 in ");
        head[0] = 0xeb;
        head[1] = 0x90;
        bitSync->setFrameHead(head);

        head[0] = 0x10;
        head[1] = 0x06;
        bitSync->setFrameType(head);
        index = bitSync->run(recvdata);

        if(index != -1)
        {
            uint16_t dataLength = recvdata[4];
            dataLength = dataLength << 8 | recvdata[5] + 8;
            BIT_SYNC_DEBUG("%s\n", RemoteDataToString(recvdata, 32).c_str());
            BIT_SYNC_DEBUG("%s\n", RemoteDataToString(&(recvdata[dataLength-7]), 32).c_str());
            BIT_SYNC_DEBUG("%s\n", RemoteDataToString(&(recvdata[(dataLength-7)*2]), 32).c_str());
        }


        BIT_SYNC_DEBUG("TEST_SYNC: loop: %d\n", loop++);
        sleep(2);
    }

    return 0;
}

string RemoteDataToString(uint8_t *data, int dataLength)
{
    std::stringstream sstream;

    int columns = 16;
    int rows = ceil(dataLength / columns)+1; 

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

#endif // _TEST_COMMON_IIO_IIO_HPP_
