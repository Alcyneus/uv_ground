// #if !defined(_BIT_SYNC_HPP_)
// #define _BIT_SYNC_HPP_


// #include <stdint.h>
// #include <stdlib.h>
// #include <string.h>
// #include <pthread.h>

// #include "kmp.hpp" 

// #include "debug.hpp"

// class BitSync
// {
// private:
//     uint8_t *masterBuffer = NULL;
//     uint8_t *substrBuffer = NULL;

//     static volatile BitSync *instance;
//     static pthread_mutex_t mtx;
// public:
//     BitSync(/* args */);
//     ~BitSync();

//     static volatile BitSync *getInstance(); 

//     void init(int masterSize, int subSize);
//     void run(uint8_t *masterStr, uint8_t *head);
//     void dataShift(uint8_t *data, int bytes, uint8_t *dataBuffer);
// };

// volatile BitSync * BitSync::instance = NULL;
// pthread_mutex_t BitSync::mtx;

// BitSync::BitSync(/* args */)
// {
// }

// BitSync::~BitSync()
// {
// }

// volatile BitSync *BitSync::getInstance()
// {
//     if (NULL == instance)
//     {
//         pthread_mutex_lock(&mtx);
//         if (NULL == instance)  
//         {
//             instance = new BitSync();
//         }
//         pthread_mutex_unlock(&mtx);
//     }
//     BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: Get the %sBitSync instance: %d%s\n",GREEN, instance,NONE);

//     return instance;
// }

// void BitSync::init(int masterSize, int subSize)
// {
//     this->masterBuffer = (uint8_t *)calloc(2048*8, sizeof(uint8_t));
//     this->substrBuffer = (uint8_t *)calloc(16, sizeof(uint8_t));
// }

// void BitSync::run(uint8_t *masterStr, uint8_t *head)
// {


//     uint8_t *master = (uint8_t *)calloc(2048*8, sizeof(uint8_t));
//     uint8_t *sub = (uint8_t *)calloc(16, sizeof(uint8_t));

//     memset(master, 0x00, 2048*8);
//     memset(sub, 0x00, 16);

//     dataShift(masterStr, 2048, master);
//     dataShift(head, 2, sub);

//     for (int i = 0; i < 64; i++)
//     {
//         BIT_SYNC_DEBUG("%d", master[i]); 
//     }
//     BIT_SYNC_DEBUG("\n");
//     for (int i = 0; i < 16; i++)
//     {
//         BIT_SYNC_DEBUG("%d", sub[i]); 
//     }
//     BIT_SYNC_DEBUG("\n");

//     int index = Index_KMP(master, 2048, sub, 15, 0);

//     BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: head index: %d\n", index);

//     // uint8_t *master = (uint8_t *)calloc(2048, sizeof(uint8_t));
//     // uint8_t *sub = (uint8_t *)calloc(16, sizeof(uint8_t));

//     // memset(master, 0x00, 2048);
//     // memset(sub, 0x00, 16);



//     // int offset = 100;


//     // cout << "input off set:\n>>";
//     // cin >> offset; 

//     // master[0] = 1;
//     // master[1] = 1;
//     // master[2] = 0;
//     // master[3] = 1;
//     // master[4] = 1;
//     // master[5] = 1;
//     // master[6] = 0;
//     // master[7] = 1;

//     // master[0+offset+10] = 1;
//     // master[1+offset+10] = 1;
//     // master[2+offset+10] = 0;
//     // master[3+offset+10] = 1;

//     // master[4+offset+10] = 1;
//     // master[5+offset+10] = 1;
//     // master[6+offset+10] = 0;
//     // master[7+offset+10] = 1;

//     // master[8+offset+10] = 0;
//     // master[9+offset+10] = 1;
//     // master[10+offset+10] = 0;
//     // master[11+offset+10] = 1;

//     // master[12+offset+10] = 1;
//     // master[13+offset+10] = 1;
//     // master[14+offset+10] = 0;
//     // master[15+offset+10] = 0;

//     // sub[0] = 1;
//     // sub[1] = 1;
//     // sub[2] = 0;
//     // sub[3] = 1;

//     // sub[4] = 1;
//     // sub[5] = 1;
//     // sub[6] = 0;
//     // sub[7] = 1;

//     // sub[8] = 0;
//     // sub[9] = 1;
//     // sub[10] = 0;
//     // sub[11] = 1;

//     // sub[12] = 1;
//     // sub[13] = 1;
//     // sub[14] = 0;
//     // sub[15] = 0;

//     // for (int i = 0; i < 64; i++)
//     // {
//     //     BIT_SYNC_DEBUG("%d", master[i]); 
//     // }
//     // BIT_SYNC_DEBUG("\n");
//     // for (int i = 0; i < 16; i++)
//     // {
//     //     BIT_SYNC_DEBUG("%d", sub[i]); 
//     // }
//     // BIT_SYNC_DEBUG("\n");

//     // int index = Index_KMP(master, 2048, sub, 16, 0);


//     // // int index = Index_KMP(this->masterBuffer, 2048*8, this->substrBuffer, 16, 0);

//     // BIT_SYNC_DEBUG("BIT_SYNC_DEBUG: head index: %d\n", index);
// }

// void BitSync::dataShift(uint8_t *data, int bytes, uint8_t *dataBuffer)
// {   
//     static int bits = 8;

//     int bitCounter = 0;
// 	static uint8_t shift[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

// 	for (int byte = 0; byte < bytes; byte++)
// 	{
// 		for (int bit = bits-1; bit >= 0; bit--)
// 		{
// 			dataBuffer[bitCounter++] = (data[byte]&shift[bit]) >> bit;
// 		}	
// 	}


//     if(bytes > 2)
//     {
//         BIT_SYNC_DEBUG("\nmaster:\n");
//         for (int i = 0; i < 64; i++)
//         {
//             BIT_SYNC_DEBUG("%d",dataBuffer[i]);
//         }
        
//     }else
//     {
//         BIT_SYNC_DEBUG("\n");
//         for (int i = 0; i < 16; i++)
//         {
//             BIT_SYNC_DEBUG("%d",dataBuffer[i]);
//         }
//         BIT_SYNC_DEBUG("\n");
//     }
    
// }
// #endif // _BIT_SYNC_HPP_
