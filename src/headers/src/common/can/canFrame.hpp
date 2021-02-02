#ifndef __LINUX_UV_ENTITY_CAN_FRAME_H__
#define __LINUX_UV_ENTITY_CAN_FRAME_H__



#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <sstream>
// #include <pthread.h>

#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <unistd.h>


using namespace std;

class CanFrame
{
    protected:
        struct can_frame *frame;
    public:
        CanFrame();
        ~CanFrame();

        void init(){};
        bool equal(CanFrame *canFame);
        bool equal(CanFrame *canFrame, int dataLength);

        string toString();
        can_frame *getCanFrame();
        void setCanFrame(can_frame canFrame);

        canid_t getCanFrameId();
        void setCanFrameId(canid_t id);

        __u8 *getCanData();
        void setCanData( __u8 *data, int dataLength);

        uint8_t getFrameDataLength();
        void setFrameDataLength(uint8_t length);

};
CanFrame::CanFrame()
{
    frame = new can_frame();
}

CanFrame::~CanFrame()
{
    delete frame;
}

bool CanFrame::equal(CanFrame *canFrame)
{
    can_frame *compare = canFrame->getCanFrame();

    bool equality = false;

    equality |= frame->can_id  ^ compare->can_id | frame->can_dlc ^ compare->can_dlc;

    for (int i = 0; i < CAN_MAX_DLEN; i++)
    {
        equality |= frame->data[i] ^ compare->data[i];
    }

    return !equality;
}

bool CanFrame::equal(CanFrame *canFrame, int dataLength)
{
    can_frame *compare = canFrame->getCanFrame();

    bool equality = false;

    equality |= frame->can_id  ^ compare->can_id | frame->can_dlc ^ compare->can_dlc;

    for (int i = 0; i < dataLength; i++)
    {
        equality |= frame->data[i] ^ compare->data[i];
    }

    return !equality;
}

can_frame *CanFrame::getCanFrame()
{
    return this->frame;
}
void CanFrame::setCanFrame(can_frame canFrame)
{
    this->frame = &(canFrame);
    // memcpy(this->canFrame, &canFrame, sizeof(canFrame));
}

canid_t CanFrame::getCanFrameId()
{
    return this->frame->can_id;
}
void CanFrame::setCanFrameId(canid_t id)
{
    this->frame->can_id = id;
}



uint8_t *CanFrame::getCanData()
{
    return this->frame->data;
}
void CanFrame::setCanData( __u8 *data, int dataLength)
{
    memcpy(this->frame->data, data, dataLength);
    this->frame->can_dlc = dataLength;
}

uint8_t CanFrame::getFrameDataLength()
{
    return this->frame->can_dlc;
}
void CanFrame::setFrameDataLength(uint8_t length)
{
    this->frame->can_dlc = length;
}


string CanFrame::toString()
{
    stringstream sstream;

    sstream     <<  "CanFrame{ " 
                <<  "can_id="       << "0x" << hex << (int)this->frame->can_id 
                <<  ", can_dlc="    << "0x" << hex << (int)this->frame->can_dlc
                <<  ", pad="        << "0x" << hex << (int)this->frame->__pad 
                <<  ", res0="       << "0x" << hex << (int)this->frame->__res0 
                <<  ", res1="       << "0x" << hex << (int)this->frame->__res1 
                <<  ", data=["      << "0x" << hex << (int)this->frame->data[0] << ", "
                                    << "0x" << hex << (int)this->frame->data[1] << ", " 
                                    << "0x" << hex << (int)this->frame->data[2] << ", " 
                                    << "0x" << hex << (int)this->frame->data[3] << ", " 
                                    << "0x" << hex << (int)this->frame->data[4] << ", " 
                                    << "0x" << hex << (int)this->frame->data[5] << ", " 
                                    << "0x" << hex << (int)this->frame->data[6] << ", " 
                                    << "0x" << hex << (int)this->frame->data[7] << "]"
                <<'}' << endl;

    return sstream.str();
}

#endif // !__LINUX_UV_ENTITY_CAN_FRAME_H__