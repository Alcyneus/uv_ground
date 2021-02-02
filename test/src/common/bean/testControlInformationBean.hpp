#if !defined(_TEST_CONTROL_INFORMATION_BEAN_HPP_)
#define _TEST_CONTROL_INFORMATION_BEAN_HPP_

#include "controlInformationBean.hpp"

int TEST_CONTROL_INFORMATION()
{
    ControlInformationBean *bean = (ControlInformationBean *)ControlInformationBean::getInstance();

    ControlInformationData *data = bean->getData();

    data->frameHeader = 0x1;      //01 帧头       0xEB90      2
    data->frameType = 0x2;        //02 帧类型     0x1001      2
    data->frameLength = 0x3;      //03 帧长度     0x000E      2
    data->frameCount = 0x4;       //04 帧计数     0-255       1

    data->controlSwitch = 0x5;    //0x 控制开关   bit32 详情见3.4.1控制信号->控制开关
    data->chanaelSwitch = 0x6;    //05 通道开关               1
    data->remoteSwitch = 0x7;     //06 遥控开关               1
    data->telemetrySwitch = 0x8;  //07 遥测开关               1
    data->testFunction = 0x9;     //08 测试功能控制               1

    data->captureTestH = 0xa;     //09 捕获时间测试 H(高32位，关闭信号时间[byte1~2], 捕获等待时间[byte3~4])    4
    data->captureTestL = 0xb;     //09 捕获时间测试 L(低32位，含测试开关[byte1~2[15]]，锁定稳定时间[byte1~2[14:0])    2

    data->workStatus = 0xc;       //10 工作状态                   1
    data->statusOut = 0xd;        //11 状态输出控制               2
    data->checkSum = 0xe;         //12 校验和  

    BEAN_DEBUG("TEST_BEAN: %s%s%s\n", YELLOW, bean->toString().c_str(), NONE);   

    return 0;
}

#endif // _TEST_CONTROL_INFORMATION_BEAN_HPP_
