#if !defined(_TCP_CLIENT_H_)
#define _TCP_CLIENT_H_

#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>

#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <math.h>
#include <unistd.h>

class TcpClient
{
private:
     int socketFileDescription;
public:
    TcpClient(/* args */);
    ~TcpClient();

    bool init(uint16_t port, struct hostent *host);
    int upload(uint8_t * data, int size);
    int receive(uint8_t * data, int size);

};

TcpClient::TcpClient(/* args */)
{
}

TcpClient::~TcpClient()
{
}

bool TcpClient::init(uint16_t port, struct hostent *host)
{
    if((this->socketFileDescription = socket(AF_INET,SOCK_STREAM,0))==-1)// AF_INET:Internet;socK_STREAM:TCP
    {
        fprintf(stderr, "Socket Error:%s \a\n", strerror(errno)) ;
        exit(1);
    }

    static struct sockaddr_in server_addr;

    bzero(&server_addr ,sizeof(server_addr));                       //初始化，置0
    server_addr.sin_family = AF_INET;                               //IPV4
    server_addr.sin_port = htons(port);                             //(将本机器上的short数据转化位网络上的short
    server_addr.sin_addr = *((struct in_addr *) host->h_addr);      // IP地址
    
    /*客户程序发起连接请求*/        
    if(connect(this->socketFileDescription, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Connect Error:%s \a\n", strerror(errno)) ;
        exit (1);
    }
}

/**
 * @brief Data upload
 * 
 * @param data data
 * @return int 
 */
int TcpClient::upload(uint8_t* data, int size)
{
    static int sendDataCount = 0;
    sendDataCount = send(this->socketFileDescription, data, size, 0);

    return sendDataCount;
}

/**
 * @brief Data receive
 * 
 * @param data data
 * @return int 
 */
int TcpClient::receive(uint8_t* data, int size)
{
    static int recvDataCount = 0;
    recvDataCount = recv(this->socketFileDescription, data, size, 0);

    return recvDataCount;
}



#endif // _TCP_CLIENT_H_
