#if !defined(_BIT_SYNC_HPP_)
#define _BIT_SYNC_HPP_



#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "kmp.hpp" 
#include "sort.hpp"

#include "debug.hpp"



class BitSync
{
private:
    int *dataBuffer = NULL;
    int bufferSize = 0;

    
    int *headBuffer = NULL;
    int headSize = 0;

    uint8_t *frameHead = NULL;
    uint8_t *frameType = NULL;

    int dataCounter = 0;
    
    int headFaultTolerance [16][16];

    int *indexList = NULL;
    int indexCounter = 0;

    static volatile BitSync *instance;
    static pthread_mutex_t mtx;
public:
    BitSync(/* args */);
    ~BitSync();

    static volatile BitSync *getInstance(); 

    void init(int bufferSize, int subSize);
    void setData(uint8_t *data);
    void setFrameHead(uint8_t *head);
    void setFrameType(uint8_t *type);
    void setFaultToleranceHead(uint8_t *head);


    int findHead(int index, int *head);
    int findHeadFaultTolerance();

    void indexListReset();

    int run(uint8_t *data);

    int framing(int index, uint8_t *data);

    void dataShift(uint32_t *data, int *dataBuffer);
    void headShift(uint8_t *data, int *dataBuffer);
};

volatile BitSync * BitSync::instance = NULL;
pthread_mutex_t BitSync::mtx;

BitSync::BitSync(/* args */)
{
}

BitSync::~BitSync()
{
    /*delete*/
}

volatile BitSync *BitSync::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new BitSync();
        }
        pthread_mutex_unlock(&mtx);
    }
    BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: Get the %sBitSync instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}

void BitSync::init(int bufferSize, int subSize)
{
    if (NULL != this->dataBuffer){free(this->dataBuffer); this->dataBuffer=NULL;}
    if (NULL != this->headBuffer){free(this->dataBuffer); this->headBuffer=NULL;}
    if (NULL != this->frameHead){free(this->frameHead); this->frameHead=NULL;}
    if (NULL != this->frameType){free(this->frameType); this->frameType=NULL;}
    if (NULL != this->indexList){free(this->indexList); this->indexList=NULL;}
    
    this->frameHead = (uint8_t *)calloc(subSize, sizeof(uint8_t));
    this->frameType = (uint8_t *)calloc(subSize, sizeof(uint8_t));

    

    this->bufferSize = bufferSize * 8;
    this->headSize = subSize * 8;

    /*error head + head*/
    this->indexList = (int *)calloc(bufferSize/8, sizeof(int));

    BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: this->bufferSize: %d\n", this->bufferSize);
    BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: this->headSize: %d\n", this->headSize);

    this->dataBuffer = (int *)calloc(this->bufferSize, sizeof(int));
    this->headBuffer = (int *)calloc(this->headSize, sizeof(int));
}

int BitSync::run(uint8_t *data)
{

    memset(data, 0x00, this->bufferSize/8);

    /*第一次若未找到正确帧头，返回无数据*/
    if(-1 == findHeadFaultTolerance()){return -1;}

    for (int i = 0; i < this->bufferSize/8; i++)
    {
        if(this->bufferSize <= this->indexList[i]){break;}
        framing(this->indexList[i], data);
    }
    
    /*数据计数器清零*/
    this->dataCounter = 0;
    return 0;
}

int BitSync::findHead(int index, int *head)
{
   
   int newIndex = 0;
    
    while ((index < this->bufferSize))
    {
        /*用KMP算法查找帧头*/
        newIndex = KMP(&(this->dataBuffer[index]), this->bufferSize-index,
                    head, this->headSize);

        /*后续无帧头，则退出*/
        if (newIndex == -1){break;}

        index += newIndex+1;
        /*存储帧头下标*/
        this->indexList[this->indexCounter++] = index-1;  
    }

    return index;
}

/**
 * @brief 将每一个帧头下表初始化为缓冲区最后一位，方便找到帧头后从小到达的排序
 * 
 */
void BitSync::indexListReset()
{
    this->indexCounter = 0;
    for (int i = 0; i < this->bufferSize/8; i++)
    {
        this->indexList[i] = this->bufferSize;
    }
}

int BitSync::findHeadFaultTolerance()
{   
    int index = 0;

    /*下标列表复位*/
    indexListReset();
    
    /*找正确帧头下标*/
    index = findHead(0, this->headBuffer);
    
    /*第一次若未找到正确帧头，返回无数据*/
    if(this->bufferSize <= this->indexList[0])
    {
        BIT_SYNC_DEBUG("%sfind head 0x%x%x: 0%s\n", YELLOW, this->frameHead[0],this->frameHead[1], NONE);
        return -1;
    };

    /*找误BIT:1, 帧头下标*/
    for (int headBit = 0; headBit < this->headSize; headBit++)
    {
        index = findHead(0, this->headFaultTolerance[headBit]);
    }

    /*帧头下标按从小到大排序*/
    bubbleSort(this->indexList, this->bufferSize/8);

    BIT_SYNC_DEBUG("%sfind head 0x%x%x: %d %s\n", RED, 
                    this->frameHead[0],this->frameHead[1], this->indexCounter-1, NONE);

    return 0;
    
}

int BitSync::framing(int index, uint8_t *data)
{
    static int bits = 8;
    static int headLength = 7*8;


    /*获取帧头2byte、帧类型2byte、帧长度2byte、帧计数1byte； total:7byte*/
    for (int byte = index; byte < index+headLength; byte+=8)
    {
        for (int bit = 0; bit < bits; bit++)
        {
            data[this->dataCounter] = data[this->dataCounter] << 1; 
            data[this->dataCounter] = data[this->dataCounter] + this->dataBuffer[byte+bit];     
        }
        this->dataCounter++; 

        BIT_SYNC_DEBUG("0x%x, ",data[this->dataCounter-1]);
    }
    BIT_SYNC_DEBUG("\n");

    // BIT_SYNC_DEBUG("fram head:\n");
    // for (int i = this->dataCounter-7; i < this->dataCounter; i++)
    // {
    //     BIT_SYNC_DEBUG("%d, ",data[i]);
    // }
    // BIT_SYNC_DEBUG("\n");

    /**/
    // if ((data[2] != this->frameType[0]) || (data[3] != this->frameType[1]))
    // {
    //     /*数据计数还原*/
    //     this->dataCounter -= headLength/8;
    //     /*数据区域还原*/
    //     memset(&(data[this->dataCounter]), 0x00, headLength/8);
    //     /*跳过误帧头*/
    //     return index + 1;
    // }
    

    /*获取帧帧数据+校验和的长度(N+1 byte)*/
    uint16_t dataLength;

    dataLength = data[this->dataCounter-3];
    dataLength = (dataLength << 8 | data[this->dataCounter-2]);
    

    /*计算数据长度（正常帧头，取反帧头）*/
    dataLength = (dataLength > this->bufferSize/8) ? (65536-dataLength) : (dataLength+1);

    /*未知数据长度异常（>最大数据长度[2048]则归零）*/
    dataLength = (dataLength > this->bufferSize/8) ? 0 : dataLength;
    

    /*判断数据长度加上下标是否超过缓冲区最大长度[2048*8]*/
    dataLength = (index+dataLength*8) > this->bufferSize ? (this->bufferSize-index)/8 : dataLength;
    BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: data length: %d;\n", dataLength);

    int dataHead = index+headLength;
    int dataTail = index+dataLength*8;
    BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: dataTail: %d;\n", dataTail);
    /*获取帧数据+校验和*/
    for (int byte = dataHead; byte < dataTail; byte+=8)
    {
        for (int bit = 0; bit < bits; bit++)
        {
            data[this->dataCounter] = data[this->dataCounter] << 1; 
            data[this->dataCounter] = data[this->dataCounter] + this->dataBuffer[byte+bit];     
        }
        this->dataCounter++; 
    }

    return dataTail;
}

void BitSync::setData(uint8_t *data)
{
    dataShift((uint32_t *)data, this->dataBuffer);
}
void BitSync::setFrameHead(uint8_t *head)
{
    memcpy(this->frameHead, head, this->headSize/8);

    headShift(head, this->headBuffer);

    setFaultToleranceHead(head);
}

void BitSync::setFrameType(uint8_t *type)
{
    memcpy(this->frameType, type, this->headSize/8);
}

void BitSync::setFaultToleranceHead(uint8_t *head)
{ 
    for (int i = 0; i < 16; i++)
    {
        /*填充正确帧头*/
        for (int headBit = 0; headBit < this->headSize; headBit++)
        {
            this->headFaultTolerance[i][headBit] = this->headBuffer[headBit];
        }
        
        /*设置容错帧头(1bit)*/
        this->headFaultTolerance[i][i] = (0 == this->headFaultTolerance[i][i])? 1 : 0;
  
    } 
}

void BitSync::dataShift(uint32_t *data, int *dataBuffer)
{   
    int bitCounter = 0;

	static uint32_t shift[32] = {
	0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
	0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
	0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000};

	int masterCounter = 0;
	for (int byteNum = 0; byteNum < (this->bufferSize/32); byteNum++)
	{
		for (int bitNum = 31; bitNum >= 0 ; bitNum--)
		{
			dataBuffer[masterCounter++] = (data[byteNum]&shift[bitNum]) >> bitNum;
		}		
	}	

}

void BitSync::headShift(uint8_t *data, int *dataBuffer)
{   
    static int bits = 8;

    int bitCounter = 0;
	static uint32_t shift[32] = {
	0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
	0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
	0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000};

	for (int byte = 0; byte < (this->headSize/8) ; byte++)
	{
		for (int bit = bits-1; bit >= 0; bit--)
		{
			dataBuffer[bitCounter++] = (data[byte]&shift[bit]) >> bit;
		}	
	}
}
#endif // _BIT_SYNC_HPP_
