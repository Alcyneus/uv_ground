#if !defined(_SERVICE_TCP_SERCICE_HPP_)
#define _SERVICE_TCP_SERCICE_HPP_

#include <stdlib.h>
#include <pthread.h>

#include "debug.hpp"


#include "tcp.hpp"
#include "captureTestTimeBean.hpp"
#include "threadManage.hpp"

#define PLUTO_IP    "127.0.0.1"
#define PLUTO_PORT  8080



#define _TCP_SERVER_DEBUG_     1

#if _TCP_DEBUG_
#define TCP_SERVER_MSG(...) printf(__VA_ARGS__)
#define TCP_SERVER_DEBUG(...) printf(__VA_ARGS__)
#else
#define TCP_SERVER_MSG(...)
#define TCP_SERVER_DEBUG(...)  
#endif


void *waitingForConnection(void* arg);
void* tcpServerHandle(void *arg);

class TcpServier
{
private:
    TCP *tcp = NULL;
    
    int dataSize;

    ThreadManage *threadManage;
public:

    

    TcpServier(/* args */);
    ~TcpServier();

    bool init(int dataSize);
    void threadJoin();

};


TcpServier::TcpServier(/* args */)
{
}

TcpServier::~TcpServier()
{
    if (NULL != tcp){ delete tcp; tcp = NULL;}   
}


bool TcpServier::init(int dataSize)
{
    this->tcp = (TCP*)TCP::getInstance();
    this->tcp->init(PLUTO_IP, PLUTO_PORT);
    
    this->dataSize = dataSize;

    static pthread_t waitingForPCconnectionId;

    pthread_create(&waitingForPCconnectionId, NULL, waitingForConnection, (void*)&(this->dataSize));
    
    threadManage = (ThreadManage *)ThreadManage::getInstance();
    threadManage->add("waitingForPCconnectionId", &waitingForPCconnectionId);
}
void TcpServier::threadJoin()
{
    TCP_SERVER_DEBUG("TCP_SERVER_DEBUG:%s\n",threadManage->toString().c_str());
    pthread_join(threadManage->get("tcpServerHandleId"), NULL);
    pthread_join(threadManage->get("waitingForPCconnectionId"), NULL);
}


/**
 * @brief 等待连接
 * 
 * @param arg data Size
 * @return void* 
 */
void *waitingForConnection(void* arg)
{
    int *dataSize = (int*)arg;

    TCP *tcp = (TCP *)TCP::getInstance();

    static pthread_t tcpServerHandleId;

    ThreadManage *threadManage = (ThreadManage *)ThreadManage::getInstance();

    while(true)
    {
        tcp->waitConnect();
        pthread_create(&tcpServerHandleId, NULL, tcpServerHandle, (void*)dataSize);
        threadManage->add("tcpServerHandleId", &tcpServerHandleId);
    }
}

void freeData(uint8_t *data);

void* tcpServerHandle(void *arg)
{
    int *dataSize = (int*)arg;
    
    uint8_t *sendData = NULL; 
    uint8_t *recvData = NULL;

    sendData = (uint8_t *)calloc(*dataSize, sizeof(uint8_t));
    recvData = (uint8_t *)calloc(*dataSize, sizeof(uint8_t));
    TCP_SERVER_DEBUG("TCP_SERVER_DEBUG: request memory sendData:%d\n", *dataSize);
    TCP_SERVER_DEBUG("TCP_SERVER_DEBUG: request memory recvData:%d\n", *dataSize);

    TCP *tcp = (TCP *)TCP::getInstance();
    ThreadManage *threadManage = (ThreadManage *)ThreadManage::getInstance();
    TCP_SERVER_DEBUG("TCP_SERVER_DEBUG:%s%s%s\n",YELLOW, threadManage->toString().c_str(),NONE);

    bool run = true;

    while (run)
    {
        if(1 > tcp->receive(recvData, *dataSize))
        {
            run = false;         
            TCP_SERVER_DEBUG("TCP_SERVER_DEBUG: %sThe connection is broken, waiting to be reconnected!%s\n", RED, NONE);
            threadManage->del("tcpServerHandleId");
        }else
        {
            CaptureTestTimeBean *captureTestTimeBean = (CaptureTestTimeBean *)CaptureTestTimeBean::getInstance();

            captureTestTimeBean->setData(recvData);

            TCP_SERVER_DEBUG("TCP_SERVER_DEBUG: \n%s%s%s;\n",YELLOW ,captureTestTimeBean->toString().c_str() ,NONE);
        }   
    }

    freeData(sendData);
    freeData(recvData);
    TCP_SERVER_DEBUG("TCP_SERVER_DEBUG: freeData(sendData);\n");
    TCP_SERVER_DEBUG("TCP_SERVER_DEBUG: freeData(recvData);\n");
}



void freeData(uint8_t *data)
{
    if(NULL != data)
    {
        free(data);
        data = NULL;
    }
}


#endif // _SERVICE_TCP_SERCICE_HPP_
