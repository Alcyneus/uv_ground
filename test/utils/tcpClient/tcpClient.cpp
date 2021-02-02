#include <iostream>

#include "tcpClient.hpp"

#include "captureTestTimeBean.hpp"

#define PORT_NUM  8080

typedef struct PackageHead
{
    uint32_t frameHeader;           //01 帧头
    uint32_t frameType;             //02 帧类型
    uint32_t frameLength;           //03 帧长度
    uint32_t frameCount;            //04 帧计数
}PackageHeadT;

int main(int argc, char const *argv[])
{

    struct hostent *host;

    if(argc!=2)
    {
        fprintf(stderr, "Usage:%s hostname \a\n", argv[0]);
        exit(1);
    }

    if( (host=gethostbyname (argv[1])) == NULL)
    {
        fprintf(stderr, "Gethostname error\n") ;
        exit(1);
    }

    TcpClient *tcpClient = new TcpClient();

    tcpClient->init(PORT_NUM, host);


    int size = 1024;
    uint8_t *data =  (uint8_t *)calloc(size, sizeof(uint8_t));

    CaptureTestTimeData *captureTestTimeData = new CaptureTestTimeData();

    captureTestTimeData->frameHeader = 0xeb90;
    captureTestTimeData->frameType = 0x1001;
    captureTestTimeData->frameLength = 0x04;
    captureTestTimeData->frameCount = 0x01;
    captureTestTimeData->channelCloseTime = 0x01;
    captureTestTimeData->channelOpenTime = 0x02;
    captureTestTimeData->lockTime = 0x03;
    captureTestTimeData->lostLockTime = 0x04;
    captureTestTimeData->sendTime = 0x05;

    memcpy(data, captureTestTimeData, sizeof(CaptureTestTimeData));

    tcpClient->upload(data, size);

    std::cout << "/* send successful! */" << std::endl;

    return 0;
}
