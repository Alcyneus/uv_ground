#if !defined(_IIO_SYNC_SUNC_HPP_)
#define _IIO_SYNC_SUNC_HPP_


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "kmp.hpp" 
#include "sort.hpp"
#include "iio.hpp"
#include "debug.hpp"


#define CRITICAL_REGION     0

class SYNC
{
private:
    int *buffer0 = NULL;
    int *buffer1 = NULL;
    int bufferSize = 0;
    int dataCounter = 0;

    int *headBuffer = NULL;
    int headSize = 0;

    int *indexList = NULL;
    int indexCounter = 0;

    uint8_t *head = NULL;
    int headError[16][16];

    uint8_t *syncBuffer = NULL;
    int syncState = 0;

    IIO *iioRx = NULL;



    void frees();
    void dataShift(uint32_t *data, int *dataBuffer);
    void headShift(uint8_t *head, int *headBuffer);
    int getData(int *buffer);
    int findHead(int *head, int *buffer, int index);
    int findHeads(int *buffer);
    void indexListReset();
    int getLength(int index, int *buffer);
    int framing(int index, int *buffer);
    int criricalFraming(int *lastBuffer, int index, int *nextBuffer);
public:
    SYNC(/* args */);
    ~SYNC();

    
    void init(int dataSize, int headSize);
    int run(uint8_t *data);
    int setHead(uint8_t *head);
};

string criricadataToString(uint8_t *data, int dataLength)
{
    std::stringstream sstream;

    int columns = 16;
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

SYNC::SYNC(/* args */)
{
}

SYNC::~SYNC()
{
    frees();
}
void SYNC::frees()
{
    if(NULL==buffer0){free(buffer0);buffer0=NULL;}
    if(NULL==buffer1){free(buffer1);buffer1=NULL;}
    if(NULL==head){free(head);head=NULL;}
    if(NULL==indexList){free(indexList);indexList=NULL;}
    if(NULL==syncBuffer){free(syncBuffer);syncBuffer=NULL;}
}
void SYNC::init(int dataSize, int headSize)
{

    frees();

    this->bufferSize = dataSize * 8;
    this->buffer0 = (int *)calloc(this->bufferSize+CRITICAL_REGION, sizeof(int));
    this->buffer1 = (int *)calloc(this->bufferSize+CRITICAL_REGION, sizeof(int));
    this->syncBuffer = (uint8_t *)calloc(dataSize, sizeof(uint8_t));

    this->headSize = headSize * 8;
    this->headBuffer = (int *)calloc(this->bufferSize, sizeof(int));

    this->head = (uint8_t *)calloc(this->headSize/8, sizeof(uint8_t));

    this->indexList = (int *)calloc(this->bufferSize/8, sizeof(int));


    iioRx = (IIO *)IIO::getInstanceRX();
}

int SYNC::setHead(uint8_t * head)
{
    memcpy(this->head, head, this->headSize/8);
    headShift(head, this->headBuffer);

    for (int i = 0; i < 16; i++)
    {
        /*填充正确帧头*/
        for (int headBit = 0; headBit < this->headSize; headBit++)
        {
            this->headError[i][headBit] = this->headBuffer[headBit];
        }
        
        /*设置容错帧头(1bit)*/
        this->headError[i][i] = (0 == this->headError[i][i])? 1 : 0;
  
    } 

    // SYNC_DEBUG("SYNC_DEBUG:%shead:0x%x%x\n", YELLOW, this->head[0], head[1]);
    // for (size_t i = 0; i < 16; i++)
    // {
    //     SYNC_DEBUG("%d\t", i);
    //     for (int j = 0; j < 16; j++)
    //     {
    //         SYNC_DEBUG("%d, ", this->headError[i][j]);
    //     }
    //     SYNC_DEBUG("%s\n", NONE);
    // }
    

}

int SYNC::getLength(int index, int *buffer)
{
    static int bits = 8;
    static int headLength = 7*8;

    static uint8_t data[8];
    int counter = 0;

    /*获取帧头2byte、帧类型2byte、帧长度2byte、帧计数1byte； total:7byte*/
    for (int byte = index; byte < index+headLength; byte+=8)
    {
        for (int bit = 0; bit < bits; bit++)
        {
            data[counter] = data[counter] << 1; 
            data[counter] = data[counter] + buffer[byte+bit];     
        }

        BIT_SYNC_DEBUG("0x%x, ",data[counter]);
        counter++;    
    }
    BIT_SYNC_DEBUG("\n");

    uint16_t dataLength;

    dataLength = data[counter-3];
    dataLength = (dataLength << 8 | data[counter-2]);
    
    /*计算数据长度（正常帧头，取反帧头）*/
    dataLength = (dataLength > this->bufferSize/8) ? (65536-dataLength) : (dataLength+1);

    /*未知数据长度异常（>最大数据长度[2048]则归零）*/
    dataLength = (dataLength > this->bufferSize/8) ? 0 : dataLength;
    
    // dataLength = (index+dataLength*8) > this->bufferSize ? (this->bufferSize-index)/8 : dataLength;
    BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: %sdata length: %d%s;\n",YELLOW, dataLength, NONE);
    return dataLength;
}

int SYNC::run(uint8_t *data)
{
    // this->dataCounter = 0;

    int stop = 1;
    while (stop)
    {
        switch (this->syncState)
        {
        case 0:
            BIT_SYNC_DEBUG("%sCASE 0:\n%s", RED, NONE);

            getData(buffer0);
            this->syncState = (-1 == findHeads(buffer0)) ? 2 : this->syncState;

            for (int i = 0; i < indexCounter; i++)
            {

                this->syncState = (-1 == framing(indexList[i], buffer0)) ? 1 : -1;

                // BIT_SYNC_DEBUG("%sframing data:\n", YELLOW);
                // BIT_SYNC_DEBUG(criricadataToString(&(syncBuffer[this->dataCounter]), 320).c_str());
                // BIT_SYNC_DEBUG("%s\n", NONE);
            }

            stop = (-1==this->syncState) ? 0 : 1;
            this->syncState = 2;
            BIT_SYNC_DEBUG("%sCASE 0 break state:%d\n%s", RED, this->syncState, NONE);
            break;
        case 1:
            BIT_SYNC_DEBUG("%sCASE 1:\n%s", RED, NONE);
            getData(buffer1);
            criricalFraming(buffer0, indexList[indexCounter-1], buffer1);

            this->syncState = (-1 == findHeads(buffer1))? 0 : 4;

            stop = 0;
           
            // exit(0);
            BIT_SYNC_DEBUG("%sCASE 1 break state:%d\n%s", RED, this->syncState, NONE);
            break;
        case 2:
            BIT_SYNC_DEBUG("%sCASE 2:\n%s", RED, NONE);
            getData(this->buffer1);
            // findHeads(buffer1);
            this->syncState = (-1 == findHeads(buffer1)) ? 0 : this->syncState;
            for (int i = 0; i < indexCounter; i++)
            {

                this->syncState = (-1 == framing(indexList[i], buffer1)) ? 3 : -1;
                // BIT_SYNC_DEBUG("%sframing data:\n", YELLOW);
                // BIT_SYNC_DEBUG(criricadataToString(&(syncBuffer[dataCounter]), 320).c_str());
                // BIT_SYNC_DEBUG("%s\n", NONE);
            }

            stop = (-1==this->syncState) ? 0 : 1;
            this->syncState = 0;
            BIT_SYNC_DEBUG("%sCASE 2 break state:%d\n%s", RED, this->syncState, NONE);
            break;

        case 3:
            BIT_SYNC_DEBUG("%sCASE 3:\n%s", RED, NONE);
            getData(buffer0);
            criricalFraming(buffer1, indexList[indexCounter-1], buffer0);

            this->syncState = (-1 == findHeads(buffer0))? 2 : 5;

            stop = 0;
            BIT_SYNC_DEBUG("%sCASE 3 break state:%d\n%s", RED, this->syncState, NONE);
            break;
        case 4:
            BIT_SYNC_DEBUG("%sCASE 4:\n%s", RED, NONE);
            for (int i = 0; i < indexCounter; i++)
            {
                // if(-1 == framing(indexList[i], buffer1)){this->syncState = 3;}

                this->syncState = (-1 == framing(indexList[i], buffer1)) ? 3 : 0;

                // BIT_SYNC_DEBUG("%sframing data:\n", YELLOW);
                // BIT_SYNC_DEBUG(criricadataToString(&(syncBuffer[this->dataCounter]), 320).c_str());
                // BIT_SYNC_DEBUG("%s\n", NONE);
            }

            stop = (3 == this->syncState) ? 1 : 0;
            
            BIT_SYNC_DEBUG("%sCASE 4 break state:%d\n%s", RED, this->syncState, NONE);
            break;
        case 5:
            BIT_SYNC_DEBUG("%sCASE 5:\n%s", RED, NONE);
            for (int i = 0; i < indexCounter; i++)
            {
                this->syncState = (-1 == framing(indexList[i], buffer0)) ? 1 : 2;
                // BIT_SYNC_DEBUG("%sframing data:\n", YELLOW);
                // BIT_SYNC_DEBUG(criricadataToString(&(syncBuffer[this->dataCounter]), 320).c_str());
                // BIT_SYNC_DEBUG("%s\n", NONE);
            }

            stop = (1 == this->syncState) ? 1 : 0;
            
            BIT_SYNC_DEBUG("%sCASE 5 break state:%d\n%s", RED, this->syncState, NONE);
            break;
        default:
            break;
        }
    }

    memcpy(data, syncBuffer, bufferSize/8);
    memset(syncBuffer, 0x0, bufferSize/8);
    BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: this->syncState = %d\n", this->syncState);
    
}


int SYNC::criricalFraming(int *lastBuffer, int index, int *nextBuffer)
{
    int bits = 8;
    int headLength = 7;

    int dataLength = getLength(index, lastBuffer);
    
    int beyond = index + dataLength*bits + headLength*bits - (this->bufferSize+CRITICAL_REGION);
    BIT_SYNC_DEBUG("%scrirical beyond :%d%s\n", YELLOW, beyond, NONE);

    int offset = (beyond % bits);
    BIT_SYNC_DEBUG("%scrirical offset :%d%s\n", YELLOW, offset, NONE);

    int nextLength = (beyond-offset)/bits;
    int lastLength = (dataLength + headLength) - nextLength - 1;
    BIT_SYNC_DEBUG("%slastLength :%d, nextLength :%d%s\n", YELLOW, lastLength, nextLength, NONE);

    int COUNTER = this->dataCounter;

    int dataHead = index;
    int dataTail = index+lastLength*bits;
    for (int byte = dataHead; byte < dataTail; byte+=8)
    {
        for (int bit = 0; bit < bits; bit++)
        {
            this->syncBuffer[this->dataCounter] = this->syncBuffer[this->dataCounter] << 1; 
            this->syncBuffer[this->dataCounter] = this->syncBuffer[this->dataCounter] + lastBuffer[byte+bit];     
        }
        this->dataCounter++; 
    }

    for (int bit = 0; bit < bits-offset; bit++)
    {
        this->syncBuffer[this->dataCounter] = this->syncBuffer[this->dataCounter] << 1; 
        this->syncBuffer[this->dataCounter] = this->syncBuffer[this->dataCounter] + lastBuffer[dataTail+bit];     
    }
    for (int bit = 0; bit < offset; bit++)
    {
        this->syncBuffer[this->dataCounter] = this->syncBuffer[this->dataCounter] << 1; 
        this->syncBuffer[this->dataCounter] = this->syncBuffer[this->dataCounter] + nextBuffer[CRITICAL_REGION+bit]; 
    }
    this->dataCounter++; 

    BIT_SYNC_DEBUG("%scrirical: 0x%x%s\n", YELLOW,syncBuffer[this->dataCounter-1], NONE);

    dataHead = CRITICAL_REGION+offset;
    dataTail = CRITICAL_REGION+offset+nextLength*bits;
    
    for (int byte = dataHead; byte < dataTail; byte+=8)
    {
        for (int bit = 0; bit < bits; bit++)
        {
            this->syncBuffer[this->dataCounter] = this->syncBuffer[this->dataCounter] << 1; 
            this->syncBuffer[this->dataCounter] = this->syncBuffer[this->dataCounter] + nextBuffer[byte+bit];     
        }
        this->dataCounter++; 
    }
    // BIT_SYNC_DEBUG("%sdata:\n", YELLOW);
    // BIT_SYNC_DEBUG(criricadataToString(&(syncBuffer[COUNTER]), 320).c_str());
    // BIT_SYNC_DEBUG("%s\n", NONE);
}

int SYNC::framing(int index, int *buffer)
{

    // if(index > this->bufferSize+CRITICAL_REGION){RE}

    int bits = 8;
    int headLength = 7;
 
    int dataLength = getLength(index, buffer);

    int dataHead = index;
    int dataTail = index+dataLength*8+headLength*8;
    if(dataTail < this->bufferSize+CRITICAL_REGION)
    {
        
        BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: dataTail: %d;\n",dataTail);
        /*获取帧数据+校验和*/
        for (int byte = dataHead; byte < dataTail; byte+=8)
        {
            for (int bit = 0; bit < bits; bit++)
            {
                this->syncBuffer[this->dataCounter] = this->syncBuffer[this->dataCounter] << 1; 
                this->syncBuffer[this->dataCounter] = this->syncBuffer[this->dataCounter] + buffer[byte+bit];     
            }
            this->dataCounter++; 
        }    

        // this->dataCounter = 0;
    }else
    {
        BIT_SYNC_DEBUG("%sBIT_SYNC_DEBUG: dataTail: %d%s;\n", RED, dataTail, NONE);
        BIT_SYNC_DEBUG("%sBIT_SYNC_DEBUG: dataTail beyond: %d%s;\n", 
                        RED,dataTail-this->bufferSize-CRITICAL_REGION, NONE);
        return -1;
    }

    return 0;  
}

int SYNC::findHeads(int *buffer)
{

    SYNC_DEBUG("SYNC_DEBUG: %sfind heads: buffer%d%s\n",GREEN, buffer==buffer0? 0 : 1, NONE );
    
    int index = 0;
    
    indexListReset();


    index = findHead(this->headBuffer, buffer, 0);
    
    if(0 == this->indexCounter)
    {
        // SYNC_DEBUG("SYNC_DEBUG: find head 0x%x%x : 0\n", this->head[0],this->head[1]);
        for (int i = 0; i < this->headSize/8; i++)
        {
            this->head[i] = ~head[i];
        }
        setHead(head);
        index = findHead(this->headBuffer, buffer, 0);

        if(0 == this->indexCounter)
        {
            SYNC_DEBUG("SYNC_DEBUG: find head 0x%x%x : 0\n", this->head[0],this->head[1]);
            return -1;
        }
       
    }

    /*找误BIT:1, 帧头下标*/
    for (int headBit = 0; headBit < this->headSize; headBit++)
    {
        findHead(this->headError[headBit], buffer, index);
    }

    SYNC_DEBUG("SYNC_DEBUG: find head 0x%x%x : %d\n", this->head[0],this->head[1], this->indexCounter);

    /**/
    int counter = this->indexCounter;
    for (int i = 0; i < counter; i++)
    {
        if(this->indexList[i] >= this->bufferSize-CRITICAL_REGION)
        {
            this->indexCounter--;
            this->indexList[i] = this->bufferSize;
        }
    }
    

    bubbleSort(this->indexList, this->bufferSize/8);

    SYNC_DEBUG("SYNC_DEBUG: %sfind head 0x%x%x: %d\n", YELLOW, this->head[0], this->head[1], this->indexCounter);
    for (int i = 0; i < this->indexCounter; i++)
    {
        SYNC_DEBUG("%d, ", this->indexList[i]);
    }
    SYNC_DEBUG("%s\n", NONE);

}
void SYNC::indexListReset()
{
    this->dataCounter = 0;
    this->indexCounter = 0;
    for (int i = 0; i < this->bufferSize/8; i++)
    {
        this->indexList[i] = this->bufferSize+CRITICAL_REGION+1;
    }
}

int SYNC::findHead(int *head, int *buffer, int index)
{
    int newIndex = 0;
    
    while ((index < this->bufferSize))
    {
        /*用KMP算法查找帧头*/
        newIndex = KMP(&(buffer[index]), this->bufferSize+CRITICAL_REGION,
                    head, this->headSize);

        // BIT_SYNC_DEBUG("new index : %d\n", newIndex);
        /*后续无帧头，则退出*/
        if (newIndex == -1){break;}

        index += newIndex+1;
        /*存储帧头下标*/
        this->indexList[this->indexCounter++] = index-1;  
    }

    return index;
}


int SYNC::getData(int *buffer)
{   

    if(buffer == this->buffer0)
    {
        SYNC_DEBUG("buffer == this->buffer0\n");
        dataShift((uint32_t *)this->iioRx->recv(), buffer0);

        
        for (int i = 0; i < CRITICAL_REGION; i++)
        {
            this->buffer1[i] = this->buffer0[this->bufferSize-CRITICAL_REGION+i];
        }

    }else
    {
        SYNC_DEBUG("buffer == this->buffer1\n");

        dataShift((uint32_t *)this->iioRx->recv(), buffer1);

        for (int i = 0; i < CRITICAL_REGION; i++)
        {
            this->buffer0[i] = this->buffer1[this->bufferSize-CRITICAL_REGION+i];
        }
        

    }
    
    
}

void SYNC::dataShift(uint32_t *data, int *dataBuffer)
{   
    int bits = 32;
    int bitCounter = CRITICAL_REGION;


	static uint32_t shift[32] = {
	0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
	0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
	0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000};

	for (int bytes = 0; bytes < (this->bufferSize/bits); bytes++)
	{
		for (int bit = bits-1; bit >= 0 ; bit--)
		{
			dataBuffer[bitCounter++] = (data[bytes]&shift[bit]) >> bit;
		}		
	}	
    BIT_SYNC_DEBUG("%s bitCounter:%d\n%s", YELLOW, bitCounter, NONE);
}

void SYNC::headShift(uint8_t *head, int *headBuffer)
{   
    static int bits = 8;

    int bitCounter = 0;

	static uint32_t shift[32] = {
	0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
	0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
	0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000};

	for (int byte = 0; byte < (this->headSize/bits) ; byte++)
	{
		for (int bit = bits-1; bit >= 0; bit--)
		{
			headBuffer[bitCounter++] = (head[byte]&shift[bit]) >> bit;
		}	
	}
}

#endif // _IIO_SYNC_SUNC_HPP_
