#if !defined(_SIMULATE_COMMANDS_HPP_)
#define _SIMULATE_COMMANDS_HPP_

#include <cmath>
#include <sstream>
#include "can.hpp"




#if _COMMAND_DEBUG_
#define COMMAND_MSG(...) printf(__VA_ARGS__)
#define COMMAND_DEBUG(...) printf(__VA_ARGS__)
#else
#define COMMAND_MSG(...)
#define COMMAND_DEBUG(...)  
#endif




#define UV_COMMUNICATION_CAN_ID         (0x12028000 | CAN_EFF_FLAG)     //UV地检 发送 ID
#define UV_COMMUNICAT_DATA_LENGTH_BIT   2


class Commands
{
    protected:
        canid_t sendId;
        CAN *can;
        CanFrame *command = NULL;
        CanFrame *recvFramd = NULL;

    public:
        Commands(/* args */);
        ~Commands();

        virtual void init(CAN *can) = 0;

        virtual uint8_t* send() = 0;

        virtual string toString() = 0;
};

Commands::Commands(/* args */)
{
}

Commands::~Commands()
{
}



#endif // _SIMULATE_COMMANDS_HPP_
