#if !defined(_TEST_COMMON_SOCKET_TCP_H_)
#define _TEST_COMMON_SOCKET_TCP_H_

#include <stdlib.h>

#include "tcp.hpp"

#define PLUTO_IP    "127.0.0.1"
#define PLUTO_PORT  8080


typedef struct PackageHead
{
    uint32_t frameHeader;           //01 帧头
    uint32_t frameType;             //02 帧类型
    uint32_t frameLength;           //03 帧长度
    uint32_t frameCount;            //04 帧计数
}PackageHeadT;

int TEST_COMMON_SOCKET_TCP()
{
    int size = 1024;
    uint8_t *data =  (uint8_t *)calloc(size, sizeof(uint8_t));

    TCP *tcp = new TCP();
    tcp->init(PLUTO_IP, PLUTO_PORT);
    
    tcp->waitConnect();

    int recvlength = 0;
    recvlength = tcp->receive(data, size);

    PackageHeadT *head = new PackageHeadT();

    memcpy(head, data, sizeof(PackageHeadT));

    std::cout << recvlength << std::endl;

    std::cout << (long)head->frameHeader << std::endl;
    std::cout << (long)head->frameType << std::endl;
    std::cout << (long)head->frameLength << std::endl;    
    std::cout << (long)head->frameCount << std::endl;

    return 0;
}




#endif // _TEST_COMMON_SOCKET_TCP_H_
