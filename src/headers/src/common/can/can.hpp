#if !defined(_COMMON_CAN_CAN_HPP_)
#define _COMMON_CAN_CAN_HPP_

#include "canFrame.hpp"

#include "debug.hpp"                   

#include <errno.h>
#include <time.h>
#include <math.h>


#define CAN_DATA_LENGTH                 CAN_MAX_DLEN

#define COMMNAD "/sbin/ip link set " 

#define UV_COMMUNICATION_CAN_ID         (0x13400508 | CAN_EFF_FLAG)     //UV通信机 发送 ID
#define UV_COMMUNICATION_CAN_MASK       CAN_EFF_MASK


#if     _CAN_DEBUG_ 
#define CAN_MSG(...) printf(__VA_ARGS__)
#define CAN_DEBUG(...) printf(__VA_ARGS__)
#else
#define CAN_MSG(...)
#define CAN_DEBUG(...)  
#endif


class CAN
{
private:
    int socketCanFileDescription;;
    string interface;
    struct ifreq ifr;
    struct sockaddr_can addr;
    struct can_filter filter; 


    static volatile CAN *instance;
    static pthread_mutex_t mtx;

public:
    CAN(/* args */);
    ~CAN();

    static volatile CAN *getInstance(); 

    void init(string interface, uint32_t bitRate);



    int command(string commands);
    int up();
    int down();
    int setBitRate(uint32_t bitRate);

    int sendCanFrame(CanFrame* frame);
    int recvCanFrame(CanFrame* frame);
    void setRecvTimeout(uint64_t us);


    int Socket(int domain, int type, int protocol);
    int Ioctl(int socketFd, unsigned long request, struct ifreq *ifr);
    int Bind(int socketFd, struct sockaddr_can *addr , socklen_t len);
    int Filter(int socketFd, can_filter *filter);
    void Optdaemon(int optdaemon);
    void Output(char * optout);
};

volatile CAN * CAN::instance = NULL;
pthread_mutex_t CAN::mtx;

CAN::CAN(/* args */)
{
}

CAN::~CAN()
{
}

volatile CAN *CAN::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new CAN();
        }
        pthread_mutex_unlock(&mtx);
    }
    CAN_DEBUG("CAN_DEBUG: Get the %sCAN instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}

void CAN::init(string interface, uint32_t bitRate)
{

    this->interface = interface;

    setBitRate(bitRate);

    this->socketCanFileDescription = Socket(PF_CAN, SOCK_RAW, CAN_RAW);

    strcpy(ifr.ifr_name, interface.c_str());
    Ioctl(this->socketCanFileDescription,SIOCGIFINDEX, &ifr);

    /*set time out*/ 
    // struct timeval timeout={0, (__suseconds_t)1e5};
    // setsockopt(socketFd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));

    
    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    Bind(this->socketCanFileDescription, &addr, sizeof(addr));


    /*set filter for UV Communication machine*/
    filter.can_id = UV_COMMUNICATION_CAN_ID;   //UV extended can id 
    filter.can_mask = UV_COMMUNICATION_CAN_MASK;              //UV extended can frame mask
    // setsockopt(this->socketCanFileDescription, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));//set
    Filter(this->socketCanFileDescription, NULL);
 
    Optdaemon(0);
    Output(NULL);
}

int CAN::sendCanFrame(CanFrame* frame)
{
    int nbytes = send(this->socketCanFileDescription, frame->getCanFrame(), sizeof(*(frame->getCanFrame())), 0);

    if(nbytes < 0)
    {
        fprintf(stderr, "send CanFrame Error:%s \a\n", strerror(errno)) ;
        exit(1);
    }
    CAN_DEBUG("CAN_DEBUG:  send byte %s [%d]\n", nbytes < 0 ? "failed" : "", nbytes);
    
    return nbytes;
}
int CAN::recvCanFrame(CanFrame* frame)
{
    int nbytes = recv(this->socketCanFileDescription, frame->getCanFrame(), sizeof(*(frame->getCanFrame())), 0);
    CAN_DEBUG("CAN_DEBUG:  recv byte %s [%d]\n", nbytes < 0 ? "failed" : "", nbytes);

    return nbytes;
}


void CAN::setRecvTimeout(uint64_t us)
{
    /*set time out*/ 
    struct timeval timeout;

    timeout.tv_sec = floor(us / 1e6);
    timeout.tv_usec = us % (int)1e6;

    setsockopt(this->socketCanFileDescription,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));

    if(0 == us)
    {
        CAN_DEBUG("-SOCKET_CAN_DEBUG: close timeout.\n");
    }else
    {
        CAN_DEBUG("-SOCKET_CAN_DEBUG: set timeout %ds,%dus\n", (int)timeout.tv_sec, (int)timeout.tv_usec);
    }
}
int CAN::command(string commands)
{
    // CAN_DEBUG("CAN_DEBUG: set command [%s]\n", commands.c_str());
    int returnValue = system(commands.c_str());

    CAN_DEBUG("CAN_DEBUG: set command %s[state:%d] [%s]\n", returnValue < 0 ? "failed" : "",returnValue , commands.c_str());

    return returnValue;
}

int CAN::up()
{
    string upCommand = COMMNAD + this->interface + " up";

    return command(upCommand);
}
int CAN::down()
{
    string downCommand = COMMNAD + this->interface + " down";

    return command(downCommand);
}
int CAN::setBitRate(uint32_t bitRate)
{
    string downCommand = COMMNAD + this->interface + " type can bitrate " + to_string(bitRate);

    down();
    int revale = command(downCommand);
    up();
    return revale;
}


int CAN::Socket(int domain, int type, int protocol)
{  
    int socketFd = socket(domain, type, protocol);
    CAN_DEBUG("CAN_DEBUG: socket: %d\n", socketFd);

    return socketFd;
}

int CAN::Ioctl(int socketFd, unsigned long request, struct ifreq *ifr)
{  
    int ioctlR = ioctl(socketFd, request, ifr);
    CAN_DEBUG("CAN_DEBUG: ioctl: %d\n", ioctlR);

    return ioctlR;
}

int CAN::Bind(int socketFd, struct sockaddr_can *addr , socklen_t len)
{
    int bindR = bind(socketFd, (struct sockaddr *)addr, len);
    CAN_DEBUG("CAN_DEBUG: bind: %d\n", bindR);

    return bindR;
}

int CAN::Filter(int socketFd, can_filter *filter)
{
    static int filterCount = 1;
    if(filter) 
    {
        int sockoptR;
        sockoptR = setsockopt(socketFd, SOL_CAN_RAW, CAN_RAW_FILTER, filter, filterCount * sizeof(struct can_filter));
        CAN_DEBUG("CAN_DEBUG: filter: id=0x%x, mask=0x%x\n", filter->can_id, filter->can_mask);
        return sockoptR;
	}else
    {
        CAN_DEBUG("CAN_DEBUGG: no filter to be set.\n");
    }

    return 0;
}

static void sigHandle(int signo)
{
    CAN_DEBUG("CAN_DEBUG: sig handle [%d]!\n", signo);
}

void CAN::Optdaemon(int optdaemon)
{
    if(optdaemon)
    {
        daemon(1, 0);
    }
    else
    {
        signal(SIGTERM, sigHandle);
		signal(SIGHUP, sigHandle);
    }
    
}

void CAN::Output(char * optout)
{ 
    if (optout) 
    {
        FILE *out = stdout;
        out = fopen(optout, "a");
        if (!out) 
        {
            perror("fopen");
            CAN_DEBUG("CAN_DEBUG: file open failed.\n");
        }
        CAN_DEBUG("CAN_DEBUG: file open successful.\n");
	}else
    {
        CAN_DEBUG("CAN_DEBUG: no file to be output.\n");
    }  
}

#endif // _COMMON_CAN_CAN_HPP_
