#if !defined(_COMMANDS_POLLING_TELEMETRY_POOL_HPP_)
#define _COMMANDS_POLLING_TELEMETRY_POOL_HPP_

#include "commands.hpp"

class TelemetryPoll:public Commands
{
    private:
        uint8_t commandData[8] = {0x5a, 0x47, 0x00, 0x04, 0x00, 0x10, 0x01, 0x11};
        uint8_t *data = NULL;
        int length = 64;
        int dataCounter = 0;
        

        static volatile TelemetryPoll *instance;
        static pthread_mutex_t mtx;
    public:

        static std::string commandName;

        TelemetryPoll(/* args */);
        ~TelemetryPoll();

        static volatile TelemetryPoll *getInstance(); 

        void init(CAN *can);
        uint8_t* send();
        string toString();

        void clearData();
        void addData(uint8_t *data, int dataLength);
        void addData(CanFrame *frame);
};

std::string TelemetryPoll::commandName = "TelemetryPoll";

volatile TelemetryPoll * TelemetryPoll::instance = NULL;
pthread_mutex_t TelemetryPoll::mtx;

TelemetryPoll::TelemetryPoll(/* args */)
{
}

TelemetryPoll::~TelemetryPoll()
{
    if(NULL == data){free(data); data=NULL;}
    if(NULL == command){delete command; command=NULL;}
    if(NULL == recvFramd){delete recvFramd;recvFramd=NULL;}
}

volatile TelemetryPoll *TelemetryPoll::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new TelemetryPoll();
        }
        pthread_mutex_unlock(&mtx);
    }
    COMMAND_DEBUG("COMMAND_DEBUG: Get the %sTelemetryPoll instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}

void TelemetryPoll::init(CAN *can)
{
    this->data = (uint8_t *)calloc(this->length, sizeof(uint8_t));

    this->command = new CanFrame();
    this->recvFramd = new CanFrame();

    this->can = can;
    this->command->setCanFrameId(UV_COMMUNICATION_CAN_ID);
    this->command->setCanData(commandData,8);
}

uint8_t* TelemetryPoll::send()
{
    COMMAND_DEBUG("COMMAND_DEBUG: send telemetry poll commands.\n");

    clearData();

    this->can->sendCanFrame(this->command);
    this->can->recvCanFrame(this->recvFramd);
    addData(this->recvFramd);
    COMMAND_DEBUG("COMMAND_DEBUG: %s%s%s\n", YELLOW, this->recvFramd->toString().c_str(), NONE);

    uint16_t recvDataLength = this->recvFramd->getCanData()[UV_COMMUNICAT_DATA_LENGTH_BIT];
    recvDataLength = recvDataLength << 8 | this->recvFramd->getCanData()[UV_COMMUNICAT_DATA_LENGTH_BIT+1] + 4;


    int frames = ceil((double)recvDataLength / CAN_DATA_LENGTH);
    COMMAND_DEBUG("COMMAND_DEBUG: Telemetry Poll: %sdata length:%d, frames:%d%s\n", YELLOW, recvDataLength,frames, NONE);

    for (int frame = 0; frame < frames-1; frame++)
    {
        this->can->recvCanFrame(this->recvFramd);
        COMMAND_DEBUG("COMMAND_DEBUG: %s%s%s\n", YELLOW, this->recvFramd->toString().c_str(), NONE);
        addData(this->recvFramd);
    }
    
    return this->data;
}

void TelemetryPoll::clearData()
{
    memset(this->data, 0x00, this->length);
    this->dataCounter = 0;
}
void TelemetryPoll::addData(uint8_t *data, int dataLength)
{ 
    memcpy(&(this->data[dataCounter]), data, dataLength);

    this->dataCounter += dataLength;
}

void TelemetryPoll::addData(CanFrame *frame)
{ 
    addData(frame->getCanData(), frame->getFrameDataLength());
}

string TelemetryPoll::toString()
{   
    stringstream sstream;

    sstream << "Telemetry poll command:";
    sstream << "id:0x"<< hex << this->command->getCanFrameId() << ", data:[";
    for (int i = 0; i < this->command->getFrameDataLength(); i++)
    {
        sstream << "0x" << (int)this->command->getCanData()[i]<<",";
    }
    sstream << "]";


    return sstream.str();
}
#endif // _COMMANDS_POLLING_TELEMETRY_POOL_HPP_
