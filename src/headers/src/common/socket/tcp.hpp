#if !defined(_COMMON_SOCKET_TCP_)
#define _COMMON_SOCKET_TCP_
#include <iostream>

#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>


#include "debug.hpp"
#include "captureTestTimeBean.hpp"

#if _TCP_DEBUG_
#define TCP_MSG(...) printf(__VA_ARGS__)
#define TCP_DEBUG(...) printf(__VA_ARGS__)
#else
#define TCP_MSG(...)
#define TCP_BRAM_DEBUG(...)  
#endif

class TCP
{
    private:
        int socketFileDescription;
        int connectFileDescription;

        static volatile TCP *instance;
        static pthread_mutex_t mtx;
    public:
        TCP(/* args */);
        ~TCP();

        static volatile TCP *getInstance();

        bool init(const char *ip, uint16_t port);
        int waitConnect();

        int upload(Bean * bean);

        int upload(uint8_t * data, int size);
        int receive(uint8_t * data, int size);
};

volatile TCP * TCP::instance = NULL;
pthread_mutex_t TCP::mtx;

TCP::TCP(/* args */)
{
}

TCP::~TCP()
{
}

volatile TCP *TCP::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new TCP();
        }
        pthread_mutex_unlock(&mtx);

        TCP_DEBUG("TCP_DEBUG: Get the %sGPIO instance: %d%s\n",GREEN, instance,NONE);
    }
    
    return instance;
}
/**
 * @brief TCP initialization
 * 
 * @param ip    local IP
 * @param port  Port used
 * @return true 
 * @return false 
 */
bool TCP::init(const char *ip, uint16_t port)
{

    int returnValue = 0;
    static struct sockaddr_in serverAddr;
    this->socketFileDescription = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serverAddr, sizeof(struct sockaddr_in));             //初始化，置0
    serverAddr.sin_family = AF_INET;                            //设置为ipv4

    serverAddr.sin_addr.s_addr=inet_addr(ip);             //绑定IP
    TCP_DEBUG("TCP_DEBUG: IP binding: %s\n", ip);

    serverAddr.sin_port = htons(port);                    //绑定端口
    TCP_DEBUG("TCP_DEBUG: Port binding: %d\n", port);

    int portReuse = 1;
    returnValue = setsockopt(this->socketFileDescription,SOL_SOCKET,SO_REUSEADDR,&portReuse,sizeof(portReuse));  //设置允许地址重用
    TCP_DEBUG("TCP_DEBUG: setsockopt return: %d\n", returnValue);

    returnValue = bind(this->socketFileDescription, (struct sockaddr*)(&serverAddr), sizeof(struct sockaddr));
    TCP_DEBUG("TCP_DEBUG: bind return: %d\n", returnValue);

    returnValue = listen(this->socketFileDescription, 5);
    TCP_DEBUG("TCP_DEBUG: listen return: %d\n", returnValue);

    return true;
}

/**
 * @brief Wait for the client to connect
 * 
 * @return int 
 */
int TCP::waitConnect()
{
    static struct sockaddr_in clientAddr; 
    socklen_t sinSize = sizeof(struct sockaddr_in) ;
    this->connectFileDescription = accept(this->socketFileDescription, (struct sockaddr*)&clientAddr, &sinSize);
    TCP_DEBUG("TCP_DEBUG: A new connection: %d\n", this->connectFileDescription);
}


int TCP::upload(Bean * bean)
{
    uint8_t * sendData = (uint8_t *)calloc(2048, sizeof(uint8_t));
    bean->pack(sendData);

    return upload(sendData, 2048);
}

/**
 * @brief Data upload
 * 
 * @param data data
 * @return int 
 */
int TCP::upload(uint8_t* data, int size)
{
    static int sendDataCount = 0;
    sendDataCount = send(this->connectFileDescription, data, size, 0);
    TCP_DEBUG("TCP_DEBUG: %s Number of bytes sent: %d %s\n",YELLOW ,sendDataCount, NONE);

    return sendDataCount;
}

/**
 * @brief Data receive
 * 
 * @param data data
 * @return int 
 */
int TCP::receive(uint8_t* data, int size)
{
    static int recvDataCount = 0;
    recvDataCount = recv(this->connectFileDescription, data, size, 0);
    TCP_DEBUG("TCP_DEBUG: Number of bytes receive: %d\n", recvDataCount);

    return recvDataCount;
}


#endif // _COMMON_SOCKET_TCP_
