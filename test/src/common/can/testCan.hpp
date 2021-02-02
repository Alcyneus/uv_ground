#if !defined(_TEST_CAN_HPP_)
#define _TEST_CAN_HPP_

#include "can.hpp"

// #define COMMAND "/sbin/ip link set can0 type can bitrate 500000"//将CAN0波特率设置为500000 bps
// #define up "/sbin/ip link set can0 up"//打开CAN0
// #define down "/sbin/ip link set can0 down"//关闭CAN0 

int TEST_CAN()
{
    CAN *can = (CAN *)CAN::getInstance();

    

    // can->command("/sbin/ip link set can0 down");
    // can->command("/sbin/ip link set can0 type can bitrate 100000");
    // can->command("/sbin/ip link set can0 down");
    // can->command("ip link set can0 up");

    // system(down);
    // cout << "关闭CAN0\n"<< endl;
    // // system(COMMAND);
    // // cout << "将CAN0波特率设置为500000 bps\n"<< endl;
    // system(up);
    // cout << "打开CAN0\n"<< endl;

    // can->down();
    // can->setBitRate(1);
    // can->up();

    can->init("can0", 200000);

    CanFrame *frame = new CanFrame();
    can_frame canframd;

    canframd.can_id = 1001;
    canframd.can_dlc = 8;
    canframd.data[0] = 1;
    frame->setCanFrame(canframd);

    CAN_DEBUG("CAN_DEBUG: %s\n", frame->toString().c_str());

    

    can->sendCanFrame(frame);

    // can->down();
    // can->command("/sbin/ip -details link show can0");

    return 0;
}


#endif // _TEST_CAN_HPP_
