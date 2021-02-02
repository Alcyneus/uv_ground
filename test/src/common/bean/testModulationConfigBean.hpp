#if !defined(_TEST_MODULATION_CONFIG_BEAN_HPP_)
#define _TEST_MODULATION_CONFIG_BEAN_HPP_


#include "iio.hpp"
#include "bram.hpp"
#include "modulationConfigBean.hpp"

#define PI  (double)(3.1415926)

ModulationConfigDataT        mudulationConfig;         // 发送调制配置

void *sweepFreqHandler(void *);
static void mudulationConfigSelect();

void SWEEP_FREQUENCY_INIT()
{
    GPIO *gpio = (GPIO *)GPIO::getInstance();
    gpio->init();

    BRAM *bram = (BRAM *)BRAM::getInstance();
    bram->init(gpio);

    Baseband::getInstance()->init();

    IIO *iioTx = (IIO *)IIO::getInstanceTX();
    iioTx->init(149.51, 0);

}

int SWEEP_FREQUENCY()
{
    SWEEP_FREQUENCY_INIT();

    pthread_t sweepFreq;

    pthread_create(&sweepFreq, NULL, sweepFreqHandler, NULL);

    ModulationConfigBean *mudulation = (ModulationConfigBean *)ModulationConfigBean::getInstance();
    BRAM *bram = (BRAM *)BRAM::getInstance();


    mudulationConfig.infoRate = 100000;

    uint32_t phyAddr = BRAM_MAP_ADDR_TX;
    int i = 1;
    while (1)
    {
        
        i = 1;

        mudulationConfigSelect();

        mudulation->set(&mudulationConfig);
        mudulation->get(&mudulationConfig);

        bram->writeBegin();

        bram->write(phyAddr + 0x0004*1,2);    //4
        bram->write(phyAddr + 0x0004*2,mudulation->getCarrierIF());//8
        bram->write(phyAddr + 0x0004*3,mudulation->getBitRate());//c
        bram->write(phyAddr + 0x0004*4,mudulationConfig.infoDoppler);//10
        bram->write(phyAddr + 0x0004*5,mudulationConfig.dopplerFactorL1);    //14
        bram->write(phyAddr + 0x0004*6,mudulationConfig.dopplerFactorH1);    //18
        bram->write(phyAddr + 0x0004*7,mudulationConfig.dopplerFactorL2);    //1C
        bram->write(phyAddr + 0x0004*8,mudulationConfig.dopplerFactorH2);    //20
        bram->write(phyAddr + 0x0004*9,mudulationConfig.dopplerFactorL3);    //24
        bram->write(phyAddr + 0x0004*10,mudulationConfig.dopplerFactorH3);    //28
        bram->write(phyAddr + 0x0004*11,mudulationConfig.sweepFrequencyMode);    //2C
        bram->write(phyAddr + 0x0004*12,512);    //30
        bram->writeFinish();

        BRAM_DEBUG("SWEEP_FREQUENCY: config successful!\n");
    }

    pthread_join(sweepFreq, NULL);
    return 0;
}


void *sweepFreqHandler(void *)
{
    IIO *iioTX = (IIO *)IIO::getInstanceTX();

    uint8_t *sendData = (uint8_t *)calloc(2048, sizeof(uint8_t));  

    memset(sendData, 0x00, 2048);

    while (1)
    {
        iioTX->send(sendData);
    }
}

//3.2过航捷曲线
static void overFlightCurve(int *dopplerGHJT0, uint64_t *dopplerPinc1, uint64_t *dopplerPinc2, uint64_t *dopplerPinc3)
{
    switch (abs(*dopplerGHJT0))
    {
    case 500:
       * dopplerPinc1 = 0xFFC108F40000;
       * dopplerPinc2 = 0x2C4;
       * dopplerPinc3 = *dopplerGHJT0 >= 0 ? 0X0 : 0xFFFF00000000;
        break;
    case 100:
       * dopplerPinc1 = 0xffc234bd0000;
       * dopplerPinc2 = 0x146de;
       * dopplerPinc3 = *dopplerGHJT0 >= 0 ? 0X000066600000 : 0xFFFF66600000;
        break;
    case 30:
       * dopplerPinc1 = 0xffcb8d630000;
       * dopplerPinc2 = 0x1d0eba;
       * dopplerPinc3 = *dopplerGHJT0 >= 0 ? 0X000078500000: 0xFFFF78500000;
        break;
    case 10:
       * dopplerPinc1 = 0xffe3cbf10000;
       * dopplerPinc2 = 0x79da7b;
       * dopplerPinc3 = *dopplerGHJT0 >= 0 ? 0X00007d700000: 0xFFFF7d700000;
        break;
    case 5:
       * dopplerPinc1 = 0xfff05a240000;
       * dopplerPinc2 = 0x9a6c5d;
       * dopplerPinc3 = *dopplerGHJT0 >= 0 ? 0X00007eb00000: 0xFFFF7eb00000;
        break;
    case 0:
       * dopplerPinc1 = 0xffff3b170000;
       * dopplerPinc2 = 0xa9249e;
       * dopplerPinc3 = *dopplerGHJT0 >= 0 ? 0X00007ff00000: 0xFFFF7ff00000;
        break;
    default:
        DEBUG("\nError dopplerGHJT0\n");
        break;
    }
}

//3.1发送调制配置=>频率工作模式=>多普勒参数计算
static void dopplerParameterCalculation(uint32_t sweepFrequencyMode)
{
    static uint32_t frequencyOffset = 0;            /*预置频偏*/
    static uint32_t sweepRange = 10000;                 /*扫描范围*/
    static uint32_t sweepVelocity = 10000;              /*扫描速度*/
    static uint32_t sampleFrequency = (uint32_t)1e7;/*样本频率*/
    static int dopplerGHJT0 = 0;

    double dopplerT = 0;
    uint64_t dopplerPinc1 = 0;
    uint64_t dopplerPinc2 = 0;
    uint64_t dopplerPinc3 = 0;

    char reset;
    int resetParameter;
    int loop = 1;

    /*是否修改预设参数？*/
    while (1 == loop)
    {
        DEBUG("是否需要重新设置[预置频偏]/[扫描范围]/[扫描速度]?\nYes(Y), No(N), View(V)\n>>");
        scanf("%c", &reset);

        switch (reset)
        {
        case 'y':
            DEBUG("选择要设置的参数：\n0.查看参数\n1.预置频偏\n2.扫描范围\n3.扫描速度\n>> ");
            scanf("%d", &resetParameter);
            switch (resetParameter)
            {
            case 0:
                DEBUG("预置频偏 = %d\n", frequencyOffset);
                DEBUG("扫描范围 = %d\n", sweepRange);
                DEBUG("扫描速度 = %d\n", sweepVelocity);
                break;
            case 1:
                DEBUG("设置：预置频偏\n>> ");
                scanf("%d", &frequencyOffset);
                DEBUG("设置[预置频偏]为： %d\n", frequencyOffset);
                break;
            case 2:
                DEBUG("设置：扫描范围\n>> ");
                scanf("%d", &sweepRange);
                DEBUG("设置[扫描范围]为： %d\n", sweepRange);
                break;
            case 3:
                DEBUG("设置：扫描速度\n>> ");
                scanf("%d", &sweepVelocity);
                DEBUG("设置[扫描速度]为： %d\n", sweepVelocity);
                break;
            default:
                break;
            }
            break;
        case 'n':
            loop = 0;
            break;
        case 'v':
            DEBUG("预置频偏 = %d\n", frequencyOffset);
            DEBUG("扫描范围 = %d\n", sweepRange);
            DEBUG("扫描速度 = %d\n", sweepVelocity);
            break;
        
        default:
            //DEBUG("输入错误，重新输入：\n>>");
            break;
        }
    }

    
    

    switch (sweepFrequencyMode)
    {
    case 0x0:   /*归零*/
        DEBUG("工作模式：归零\n");

        dopplerPinc1 = 0;
        dopplerPinc2 = 0;
        dopplerPinc3 = 0;

        mudulationConfig.dopplerFactorH1 = 0;
        mudulationConfig.dopplerFactorL1 = 0;
        mudulationConfig.dopplerFactorH2 = 0;
        mudulationConfig.dopplerFactorL2 = 0;
        mudulationConfig.dopplerFactorH3 = 0;
        mudulationConfig.dopplerFactorL3 = 0;
        break;
    case 0x1:   /*固定频率*/
        DEBUG("工作模式：固定频率\n");

        dopplerPinc1 = round((double)frequencyOffset * pow(2, 48) / sampleFrequency);
        
        dopplerPinc2 = 0;

        dopplerPinc3 = 0;
        break;
    case 0x2:   /*正弦波*/
        DEBUG("工作模式：正弦波\n");
        dopplerT = (double)4 * sweepRange / sweepVelocity * PI / 2;
        dopplerPinc1 = round((double)sweepRange*2*PI / dopplerT*pow(2, 32) / 
                            sampleFrequency / sampleFrequency * pow(2, 48));   /*除两次sampleFrequency？*/
        
        dopplerPinc2 = round((double)sweepRange * pow(2*PI/dopplerT, 2) * pow(2, 32) / 
                            sampleFrequency * pow(2,32) / sampleFrequency / sampleFrequency * pow(2, 48));
       
        dopplerPinc3 = round((double)1 / dopplerT * pow(2, 61) * 2 * PI / sampleFrequency);

/*
PI = 3.1415926
SR = 10**5
SV = 10**5
SF = 10**7
T = 4 * SR / SV * PI / 2
T
SINDOP1 = SR * 2 * PI / T * 2**32 / SF / SF * 2**48
SINDOP1
SINDOP2 = SR * (2*PI/T)**2 * 2**32 / SF * 2**32 / SF / SF * 2**48
SINDOP2
SINDOP3 = 1 / T * 2**61 * 2 * PI / SF
SINDOP3

*/       
        break;
    case 0x3:   /*三角波*/
        DEBUG("工作模式：:三角波\n");
        dopplerT = (double)4 * sweepRange / sweepVelocity;

        dopplerPinc1 = round((double)sweepVelocity * pow(2, 32) / sampleFrequency * pow(2, 48) / sampleFrequency);
        
        dopplerPinc2 = round((double)2 * sweepVelocity / dopplerT * pow(2, 32) / 
                            sampleFrequency * pow(2, 32) / sampleFrequency / sampleFrequency * pow(2, 40));


        dopplerPinc3 = round((double)0.5 / dopplerT * pow(2,64) / sampleFrequency);

/*
PI = 3.1415926
SR = 10**4
SV = 10**4
SF = 10**7
T = 4 * SR / SV
T
SANDOP1 = SV * 2**32 / SF * 2**48 / SF
SANDOP1
SANDOP2 = 2 * SV / T * 2**32 / SF * 2**32 / SF / SF *2**40
SANDOP2
SANDOP3 = 1 / 2 / T * 2**64 / SF
SANDOP3

*/
        break;
    case 0x4:   /*过航捷曲线*/
        DEBUG("工作模式：过航捷曲线\n");

        DEBUG("输入：过航捷曲线系数\n>>");

        scanf("%d", &dopplerGHJT0);

        overFlightCurve(&dopplerGHJT0, &dopplerPinc1, &dopplerPinc2, &dopplerPinc3);

        break;
    
    default:    /*错误工作模式*/
        DEBUG("错误工作模式\n");
        break;
    }

    mudulationConfig.dopplerFactorH1 = dopplerPinc1>>32;
    mudulationConfig.dopplerFactorL1 = dopplerPinc1&0x00000000ffffffff;

    mudulationConfig.dopplerFactorH2 = dopplerPinc2>>32;
    mudulationConfig.dopplerFactorL2 = dopplerPinc2&0x00000000ffffffff;

    mudulationConfig.dopplerFactorH3 = dopplerPinc3>>32;
    mudulationConfig.dopplerFactorL3 = dopplerPinc3&0x00000000ffffffff;

    DEBUG("T = %f\n", dopplerT);
    DEBUG("多普勒参数1 = 0x%llx : (%lld)\n", dopplerPinc1, dopplerPinc1);
    DEBUG("多普勒参数2 = 0x%llx : (%lld)\n", dopplerPinc2, dopplerPinc2);
    DEBUG("多普勒参数3 = 0x%llx : (%lld)\n\n", dopplerPinc3, dopplerPinc3);
}


static void mudulationConfigSelect()
{
    DEBUG("\n选择[发送遥控配置 BPSK]想要修改的属性：\n");
    DEBUG("查看当前控制信号属性：0\n发送：100\n取消发送：101\n");
    DEBUG("帧头:1\n帧类型:2\n帧长度:3\n帧计数:4\n");
    DEBUG("中心频率NOC :5\n频率工作模式:6\n多普勒系数1:7\n多普勒系数2:8\n");
    DEBUG("多普勒系数3:9\n信息速率:10\n信息多普勒系数:11\n码型：12\n校验和：13\n>> ");
    int configSendRemoteCtrlSelect = 0;
    int configSendRemoteCtrlLoop = 1;

    uint64_t dopplerPinc1 = 0;
    uint64_t dopplerPinc2 = 0;
    uint64_t dopplerPinc3 = 0;
    while (1 == configSendRemoteCtrlLoop)
    {
        scanf("%d", &configSendRemoteCtrlSelect);
        switch (configSendRemoteCtrlSelect)
        {
        case 1:
            DEBUG("1.帧头：\n>> ");
            scanf("%x", &mudulationConfig.frameHeader);
            break;
        case 2:
            DEBUG("2.帧类型：\n>> ");
            scanf("%x", &mudulationConfig.frameType);
            break;
        case 3:
            DEBUG("3.帧长度：\n>> ");
            scanf("%x", &mudulationConfig.frameLength);
            break;
        case 4:
            DEBUG("4.帧计数：\n>> ");
            scanf("%x", &mudulationConfig.frameCount);
            break;
        case 5:
            DEBUG("5.中心频率NOC：\n>> ");
            scanf("%x", &mudulationConfig.centreFreqNCO);
            break;
        case 6:
            DEBUG("6.频率工作模式：\n0.归零\n1.固定频率\n2.正弦波\n3.三角波\n4.过航捷曲线\n>> ");
            scanf("%d", &mudulationConfig.sweepFrequencyMode);
            dopplerParameterCalculation(mudulationConfig.sweepFrequencyMode);
            break;
        case 7:
            DEBUG("7.多普勒系数1：\n>> ");
           
            scanf("%llx", &dopplerPinc1);
            mudulationConfig.dopplerFactorH1 = dopplerPinc1>>32;
            mudulationConfig.dopplerFactorL1 = dopplerPinc1&0x00000000ffffffff;
            break;
        case 8:
            DEBUG("8.多普勒系数2：\n>> ");
            
            scanf("%llx", &dopplerPinc2);
            mudulationConfig.dopplerFactorH2 = dopplerPinc2>>32;
            mudulationConfig.dopplerFactorL2 = dopplerPinc2&0x00000000ffffffff;
            break;
        case 9:
            DEBUG("9.多普勒系数3：\n>> ");
            
            scanf("%llx", &dopplerPinc3);
            mudulationConfig.dopplerFactorH3 = dopplerPinc3>>32;
            mudulationConfig.dopplerFactorL3 = dopplerPinc3&0x00000000ffffffff;
            break;
        case 10:
            DEBUG("10.信息速率：\n>> ");
            scanf("%d", &mudulationConfig.infoRate);
            break;
        case 11:
            DEBUG("11.信息多普勒系数：\n>> ");
            scanf("%d", &mudulationConfig.infoDoppler);
            break;
        case 12:
            DEBUG("12.码型：\n>> ");
            scanf("%d", &mudulationConfig.nrzCode);
            break;
        case 13:
            DEBUG("13.校验和：\n>> ");
            scanf("%d", &mudulationConfig.checkSum);
            break;
        case 101:
            DEBUG("取消发送！\n");
            configSendRemoteCtrlLoop = 0;
            break;
        case 100:
            DEBUG("发送！\n");
            configSendRemoteCtrlLoop = 0;
            DEBUG("发送数据成功！\n");
        case 0:
            DEBUG("当前[发送遥控配置 BPSK]属性：\n");
            DEBUG("1.帧头.frameHeader 0x%x\n", mudulationConfig.frameHeader);
            DEBUG("2.帧类型.frameType 0x%x\n", mudulationConfig.frameType);
            DEBUG("3.帧长度.frameLength 0x%x\n", mudulationConfig.frameLength);
            DEBUG("4.帧计数.frameCount 0x%x\n", mudulationConfig.frameCount);
            DEBUG("5.中心频率NOC.centreFreqNCO 0x%x\n", mudulationConfig.centreFreqNCO);
            DEBUG("6.频率工作模式.sweepFrequencyMode 0x%x\n", mudulationConfig.sweepFrequencyMode);
            DEBUG("7.多普勒系数1H.dopplerFactorH1 0x%x\n", mudulationConfig.dopplerFactorH1);
            DEBUG("7.多普勒系数1L.dopplerFactorL1 0x%x\n", mudulationConfig.dopplerFactorL1);
            DEBUG("8.多普勒系数2H.dopplerFactorH2 0x%x\n", mudulationConfig.dopplerFactorH2);
            DEBUG("8.多普勒系数2L.dopplerFactorL2 0x%x\n", mudulationConfig.dopplerFactorL2);
            DEBUG("9.多普勒系数3H.dopplerFactorH3 0x%x\n", mudulationConfig.dopplerFactorH3);
            DEBUG("9.多普勒系数3L.dopplerFactorL3 0x%x\n", mudulationConfig.dopplerFactorL3);
            DEBUG("10.信息速率.infoRate 0x%x\n", mudulationConfig.infoRate);
            DEBUG("11.信息多普勒系数.infoDoppler 0x%x\n", mudulationConfig.infoDoppler);
            DEBUG("12.码型.nrzCode 0x%x\n", mudulationConfig.nrzCode);
            DEBUG("13.校验和.checkSum 0x%x\n", mudulationConfig.checkSum);
            break;
        default:
            DEBUG("查看当前控制信号属性：0\n发送：100\n");
            DEBUG("帧头:1\n帧类型:2\n帧长度:3\n帧计数:4\n");
            DEBUG("中心频率NOC :5\n频率工作模式:6\n多普勒系数1:7\n多普勒系数2:8\n");
            DEBUG("多普勒系数3:9\n信息速率:10\n信息多普勒系数:11\n码型：12\n校验和：13\n");
            DEBUG("不在范围之内，重新输入：\n");
            break;
        }

         DEBUG("选择其他需要修改的[发送遥控配置 BPSK]属性:\n>> ");
    }
    
}


#endif // _TEST_MODULATION_CONFIG_BEAN_HPP_
