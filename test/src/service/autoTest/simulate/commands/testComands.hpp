#if !defined(_SIMULATE_COMMANDS_TEST_COMMANDS_HPP_)
#define _SIMULATE_COMMANDS_TEST_COMMANDS_HPP_

#include <cmath>
#include <sstream>

#include "can.hpp"

#include "telemetryPoll.hpp"
#include "remotePoll.hpp"

#include "downlinkRemoteData.hpp"

string toString(uint8_t *data, int dataLength);

int TEST_DOWN_LINK_REMOTE_DATA()
{
    CAN *can0 = (CAN *)CAN::getInstance();

    can0->init("can0", 500000);

    uint8_t * data = (uint8_t *)calloc(300, sizeof(uint8_t));

    DownlinkRemoteData *downlink = (DownlinkRemoteData *)DownlinkRemoteData::getInstance();

    downlink->init(can0);

    for (int i = 0; i < 200; i++)
    {
        data[i] = i;
    }
    for (int i = 0; i < 100; i++)
    {
        data[i+200] = i;
    }

    data[0] = 0x5a;
    data[1] = 0x47;
    data[2] = 0x01;
    data[3] = 0x0f;
    data[4] = 0x0d;

    data[34*8+2] = 0x4c;
    data[34*8+3] = 0x00;
    data[34*8+4] = 0x00;
    data[34*8+5] = 0x00;
    data[34*8+6] = 0x00;
    data[34*8+7] = 0x00;

    downlink->setData(data, 300);

    downlink->send();


    


}



int TEST_COMMANDS()
{
    CAN *can0 = (CAN *)CAN::getInstance();

    can0->init("can0", 500000);

    uint8_t * data = (uint8_t *)calloc(300, sizeof(uint8_t));

    // TelemetryPoll *command = new TelemetryPoll();
    RemotePoll *command = new RemotePoll();

    command->init(can0);

    data = command->send();
    
    CAN_DEBUG("data:\n%s%s%s", YELLOW, toString(data, 100).c_str(), NONE);
}


string toString(uint8_t *data, int dataLength)
{
    std::stringstream sstream;

    int columns = 8;
    int rows = floor(dataLength / columns); 

    sstream << "data:\n";
    for (int row = 0; row < rows; row++)
    {
        sstream << row <<":\t";
        for (int column = 0; column< columns; column++)
        {
            sstream << "0x" << hex << (int)data[column+row*columns] << ", ";
        }
        sstream << "\n";
    }
    return sstream.str();
    
}


#endif // _SIMULATE_COMMANDS_TEST_COMMANDS_HPP_
