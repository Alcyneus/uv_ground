#if !defined(_DOWN_LINK_REMOTE_DATA_HPP)
#define _DOWN_LINK_REMOTE_DATA_HPP

#include "commands.hpp"


class DownlinkRemoteData:public Commands
{
private:
    uint8_t *buffer = NULL;
    int bufferSize = 300;

    uint16_t dataSize = 0;

    int framingCounter = 0;
    uint32_t canID;

    static volatile DownlinkRemoteData *instance;
    static pthread_mutex_t mtx;
public:
    DownlinkRemoteData(/* args */);
    ~DownlinkRemoteData();

    static std::string commandName;
    static volatile DownlinkRemoteData *getInstance(); 

    void init(CAN *can);
    void setData(uint8_t *data, int length);

    CanFrame *framing();

    void sendInit();
    uint8_t* send();

    string toString(){};

};

std::string DownlinkRemoteData::commandName = "DownlinkRemoteData";
volatile DownlinkRemoteData * DownlinkRemoteData::instance = NULL;
pthread_mutex_t DownlinkRemoteData::mtx;

DownlinkRemoteData::DownlinkRemoteData(/* args */)
{
}

DownlinkRemoteData::~DownlinkRemoteData()
{
    if(NULL == buffer){free(buffer); buffer=NULL;}
    if(NULL == command){delete command; command=NULL;}
    if(NULL == recvFramd){delete recvFramd;recvFramd=NULL;}
}

volatile DownlinkRemoteData *DownlinkRemoteData::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new DownlinkRemoteData();
        }
        pthread_mutex_unlock(&mtx);
    }
    COMMAND_DEBUG("COMMAND_DEBUG: Get the %sDownlinkRemoteData instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}

void DownlinkRemoteData::init(CAN *can)
{
    this->buffer = (uint8_t *)calloc(this->bufferSize, sizeof(uint8_t));

    this->command = new CanFrame();
    this->recvFramd = new CanFrame();

    this->can = can;
    this->command->setCanFrameId(UV_COMMUNICATION_CAN_ID);
    // this->command->setCanData(commandData,8);
}

void DownlinkRemoteData::setData(uint8_t *data, int length)
{
    memset(this->buffer, 0x00, this->bufferSize);
    memcpy(this->buffer, data, length);

    this->dataSize = this->buffer[2];
    this->dataSize = (this->dataSize << 8) | this->buffer[3];

    COMMAND_DEBUG("COMMAND_DEBUG: dataSize = %d\n", this->dataSize);
}

void DownlinkRemoteData::sendInit()
{
    this->canID = UV_COMMUNICATION_CAN_ID;
    this->framingCounter = 0;
}

uint8_t* DownlinkRemoteData::send()
{
    static int frames = 0;
    frames = this->dataSize / 8 + 2;

    COMMAND_DEBUG("COMMAND_DEBUG: frames = %d\n", frames);

    sendInit();

    for (int i = 0; i < frames; i++)
    {
        can->sendCanFrame(framing());
        usleep(100000);
    }
    
    // can->recvCanFrame(this->recvFramd);

    // return this->recvFramd->getCanData();
    
}

CanFrame *DownlinkRemoteData::framing()
{
    this->command->setCanFrameId(this->canID++);
    this->command->setCanData(&buffer[this->framingCounter],8);
    this->framingCounter += 8;

    return this->command;
}


#endif // _DOWN_LINK_REMOTE_DATA_HPP
