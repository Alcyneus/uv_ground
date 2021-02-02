// #ifndef _SYNCHRONOUS_H_
// #define _SYNCHRONOUS_H_

// #include <stdio.h>
// #include <stdint.h>

// #include "kmp.hpp"

// void getNext(int *next, int *T, int len);
// int KMP(int *s, int len, int *p, int plen);//利用KMP算法匹配
// uint8_t bitSynchronous(uint32_t *master, int *sub);


// //未改进的KMP算法代码实现
// void getNext(int *next, int *T, int len)
// {
// 	next[0] = -1;//-1代表没有重复子串
// 	int k = -1;
// 	for (int q = 1; q <= len; q++)
// 	{
// 		while (k > -1 && T[k+1] != T[q])//下一个元素不相等，把k向前回溯
// 		{
// 			k = next[k];
// 		}
// 		if (T[k+1] == T[q])//下一个元素相等，所以最长重复子串+1
// 		{
// 			k = k+1;
// 		}
// 		next[q] = k;//给next数组赋值
// 	}
// }

// /** @brief 
//  * 	@param s			主串首地址
//  * 	@param len			主串长度
// * 	@param p			子串首地址
// * 	@param plen			子串长度
// *  @return	    		成功：主串中子串位置； 失败：-1
// **/
// int KMP(int *s, int len, int *p, int plen)//利用KMP算法匹配
// {
// 	int *next = new int(plen);
// 	getNext(next, p, plen);
// 	int k = -1;
	
// 	for (int i = 0; i < len; i++)
// 	{
//  		while (k > -1 && p[k+1]!=s[i])//两串下一个字符不相等，向前回溯（效率高就是在这里，每次匹配失败，k不用直接变为0，从第一个字符开始重新匹配，而是变为最长重复子串的下一个字符，从中间开始匹配即可）。
// 		{
// 			k = next[k];
// 		}
// 		if(p[k+1] == s[i])//两个串的字符相等，k+1来匹配子串的一个字符
// 		{
// 			k++;
// 		}
// 		if (k == plen-1)//匹配成功，返回短串在长串的位置。
// 		{
// 			delete(next);
// 			return i-plen+1;
// 		}
// 	}
// 	delete(next);
// 	return -1;
// }

// uint8_t bitSynchronous(uint32_t *master, int *sub)
// {

//     static uint8_t testData[72];

// 	static int masterArr[8192*2];
// 	static uint32_t shift[32] = {
// 	0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
// 	0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
// 	0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
// 	0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000};
// 	//printf("master[2] = 0x%x\n", master[2]);
// 	int masterArrCounter = 0;
// 	for (int byteNum = 0; byteNum < 256*2; byteNum++)
// 	{
// 		for (int bitNum = 31; bitNum >= 0 ; bitNum--)
// 		{
// 			masterArr[masterArrCounter++] = (master[byteNum]&shift[bitNum]) >> bitNum;
// 			//printf("masterArr[%d] = %d\n", masterArrCounter-1, masterArr[masterArrCounter-1]);
// 		}		
// 	}	

// 	int index = KMP(masterArr, 8192*2, sub, 16);
// 	// int index =  Index_KMP(masterArr,  8192*2, (uint8_t *)sub, 15, 0);
// 	uint8_t bitSyncTestCount = 0;
// 	if(0 <= index)
// 	{
	
        

// 		// for (int i = index; i < index+64; i++)
// 		// {
// 		// 	printf("下masterArr[%d] = %d\n", i, masterArr[i]);
// 		// }
		
// 		printf("\ncount = %d", masterArr[index+16]);
// 		printf("%d", masterArr[index+17]);
// 		printf("%d", masterArr[index+18]);
// 		printf("%d", masterArr[index+19]);
// 		printf("%d", masterArr[index+20]);
// 		printf("%d", masterArr[index+21]);
// 		printf("%d", masterArr[index+22]);
// 		printf("%d\n", masterArr[index+23]);

// 		bitSyncTestCount = (bitSyncTestCount + masterArr[index+16]) << 1;		
// 		bitSyncTestCount = (bitSyncTestCount + masterArr[index+17]) << 1;
// 		bitSyncTestCount = (bitSyncTestCount + masterArr[index+18]) << 1;
// 		bitSyncTestCount = (bitSyncTestCount + masterArr[index+19]) << 1;
// 		bitSyncTestCount = (bitSyncTestCount + masterArr[index+20]) << 1;
// 		bitSyncTestCount = (bitSyncTestCount + masterArr[index+21]) << 1;
// 		bitSyncTestCount = (bitSyncTestCount + masterArr[index+22]) << 1;
// 		bitSyncTestCount = (bitSyncTestCount + masterArr[index+23]);

// 		printf("bitSyncTestCount = 0x%x\n", bitSyncTestCount);

//         bitSyncTestCount = 0;

//         //  index += 1;
//         for (size_t i = 0; i < 72; i++)
//         {
//             bitSyncTestCount = (bitSyncTestCount + masterArr[index+8*i+0]) << 1;		
//             bitSyncTestCount = (bitSyncTestCount + masterArr[index+8*i+1]) << 1;
//             bitSyncTestCount = (bitSyncTestCount + masterArr[index+8*i+2]) << 1;
//             bitSyncTestCount = (bitSyncTestCount + masterArr[index+8*i+3]) << 1;
//             bitSyncTestCount = (bitSyncTestCount + masterArr[index+8*i+4]) << 1;
//             bitSyncTestCount = (bitSyncTestCount + masterArr[index+8*i+5]) << 1;
//             bitSyncTestCount = (bitSyncTestCount + masterArr[index+8*i+6]) << 1;
//             bitSyncTestCount = (bitSyncTestCount + masterArr[index+8*i+7]);

//             testData[i] =  bitSyncTestCount;   
//             bitSyncTestCount = 0x00;
//         }

//         printf("----------------------------------test data: ---------------------------------\n");
//         for (size_t i = 0; i < 7; i++)
//         {
//             for (size_t j = 0; j < 10; j++)
//             {
//                 printf("data[%d] = 0x%x, ", j+10*i, testData[j+10*i]);
//             }
//             printf("\n");
//         }
//         printf("------------------------------------------------------------------------------\n");
        
        

// 	}

	

// 	printf("KMP(masterArr, 8192, (int*)sub, 16) =  %d\n",index);

// 	return bitSyncTestCount;
	
// }


// #endif // !_SYNCHRONOUS_H_synchronous_h_