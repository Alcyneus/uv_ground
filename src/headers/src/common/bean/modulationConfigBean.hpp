#if !defined(_COMMON_BEAN_MODULATION_CONFIG_HPP_)
#define _COMMON_BEAN_MODULATION_CONFIG_HPP_

#include "bean.hpp"
#include "debug.hpp"


#define SAMPLING_FREQUENCY (double)10e6

typedef struct ModulationConfigData
{
    uint32_t frameHeader;           //01 帧头       0xEB90      2
    uint32_t frameType;             //02 帧类型     0x1002       2
    uint32_t frameLength;           //03 帧长度     0x0023      2
    uint32_t frameCount;            //04 帧计数     0-255       1
    uint32_t centreFreqNCO;         //05 中心频率NOC            4
    uint32_t sweepFrequencyMode;     //06 频率工作模式               1

    uint32_t dopplerFactorH1;          //07 多普勒系数1 H(高八位)     2
    uint32_t dopplerFactorL1;          //07 多普勒系数1 L(低八位)     4

    uint32_t dopplerFactorH2;          //08 多普勒系数2 H(高八位)     4
    uint32_t dopplerFactorL2;          //08 多普勒系数2 L(低八位)     4

    uint32_t dopplerFactorH3;          //09 多普勒系数3 H(高八位)     2
    uint32_t dopplerFactorL3;          //09 多普勒系数3 L(低八位)     4

    uint32_t infoRate;              //10 信息速率                  4
    uint32_t infoDoppler;           //11 信息多普勒系数             4
    uint32_t nrzCode;               //12 码型                       1
    uint32_t checkSum;              //13 校验和                     1

}ModulationConfigDataT;

class ModulationConfigBean:public Bean
{
private:
    ModulationConfigData *data = NULL;

    uint32_t carrierIF = 0;
    uint32_t bitRate = 0;

    static volatile ModulationConfigBean *instance;
    static pthread_mutex_t mtx;

public:
    ModulationConfigBean(/* args */);
    ~ModulationConfigBean();

    static volatile ModulationConfigBean *getInstance();

    int pack(uint8_t *buffer);
    int unpack(uint8_t *buffer);
    string toString();


    int set(ModulationConfigData *data);
    int get(ModulationConfigData *data);
    uint32_t getCarrierIF();
    uint32_t getBitRate();

};

ModulationConfigBean::ModulationConfigBean(/* args */)
{
    data = new ModulationConfigData();
}

ModulationConfigBean::~ModulationConfigBean()
{
    if (NULL == data){delete data;}   
}

volatile ModulationConfigBean * ModulationConfigBean::instance = NULL;
pthread_mutex_t ModulationConfigBean::mtx;

volatile ModulationConfigBean *ModulationConfigBean::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new ModulationConfigBean();
        }
        pthread_mutex_unlock(&mtx);
        BEAN_DEBUG("BEAN_DEBUG: Get the %sModulationConfigBean instance: %d%s\n",GREEN, instance,NONE);
    }
    
    return instance;
}

int ModulationConfigBean::pack(uint8_t *buffer)
{
    memcpy(buffer, data, sizeof(ModulationConfigData));
}
int ModulationConfigBean::unpack(uint8_t *buffer)
{
    memcpy(data, buffer, sizeof(ModulationConfigData));
    carrierIF = (data->centreFreqNCO / SAMPLING_FREQUENCY) * pow(2,32);
    bitRate = (data->infoRate / SAMPLING_FREQUENCY) * pow(2,32);
}
int ModulationConfigBean::set(ModulationConfigData *data)
{
    memcpy(this->data, data, sizeof(ModulationConfigData));

    carrierIF = (data->centreFreqNCO / SAMPLING_FREQUENCY) * pow(2,32);
    bitRate = (data->infoRate / SAMPLING_FREQUENCY) * pow(2,32);
}

int ModulationConfigBean::get(ModulationConfigData *data)
{
    memcpy(data, this->data, sizeof(ModulationConfigData));
}

string ModulationConfigBean::toString()
{
    stringstream sstream;

    sstream << "\nModulationConfigBean{\n" 
            << "frameHeader=0x" << hex << (long int)data->frameHeader
            << ", frameType=0x" << hex << (long int)data->frameType
            << ", frameLength=0x" << hex << (long int)data->frameLength
            << ", frameCount=0x" << hex << (long int)data->frameCount
            << "\n"
            << "centreFreqNCO=0x" << hex << (long int)data->centreFreqNCO
            << ", sweepFrequencyMode=0x" << hex << (long int)data->sweepFrequencyMode
            << ", dopplerFactorH1=0x" << hex << (long int)data->dopplerFactorH1
            << ", dopplerFactorL1=0x" << hex << (long int)data->dopplerFactorL1
            << "\n"
            << "dopplerFactorH2=0x" << hex << (long int)data->dopplerFactorH2
            << ", dopplerFactorL2=0x" << hex << (long int)data->dopplerFactorL2
            << ", dopplerFactorH3=0x" << hex << (long int)data->dopplerFactorH3
            << "\n"
            << "dopplerFactorL3=0x" << hex << (long int)data->dopplerFactorL3
            << ", infoRate=0x" << hex << (long int)data->infoRate
            << ", infoDoppler=0x" << hex << (long int)data->infoDoppler
            << "\n"
            << "nrzCode=0x" << hex << (long int)data->nrzCode
            << ", checkSum=0x" << hex << (long int)data->checkSum
            << "\n}";

    return sstream.str();
}


uint32_t ModulationConfigBean::getCarrierIF()
{
    return carrierIF;
}
uint32_t ModulationConfigBean::getBitRate()
{
    return bitRate;
}

#endif // _COMMON_BEAN_MODULATION_CONFIG_HPP_
