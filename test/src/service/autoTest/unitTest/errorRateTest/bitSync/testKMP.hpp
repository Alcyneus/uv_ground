// #ifndef _TEST_KMP_H_
// #define _TEST_KMP_H_

// #include <string.h>
// #include <iostream>
// #include <sstream>


// #include "kmp.hpp"


// using namespace std;

// #define MASTER_SIZE 2048*8
// #define SUB_SIZE    16


// int TEST_KMP2()
// {
//     uint8_t master[MASTER_SIZE];
//     uint8_t sub[SUB_SIZE];

//     for (int i = 0; i < MASTER_SIZE; i++)
//     {
//         master[i] = 0x1;
//     }

//     for (int i = 0; i < SUB_SIZE; i++)
//     {
//         sub[i] = 0x0;
//     }

    

//     int offset = 100;


//     cout << "input off set:\n>>";
//     cin >> offset; 

//     master[0] = 1;
//     master[1] = 1;
//     master[2] = 0;
//     master[3] = 1;
//     master[4] = 1;
//     master[5] = 1;
//     master[6] = 0;
//     master[7] = 1;

//     master[0+offset+10] = 1;
//     master[1+offset+10] = 1;
//     master[2+offset+10] = 0;
//     master[3+offset+10] = 1;

//     master[4+offset+10] = 1;
//     master[5+offset+10] = 1;
//     master[6+offset+10] = 0;
//     master[7+offset+10] = 1;

//     master[8+offset+10] = 0;
//     master[9+offset+10] = 1;
//     master[10+offset+10] = 0;
//     master[11+offset+10] = 1;

//     master[12+offset+10] = 1;
//     master[13+offset+10] = 1;
//     master[14+offset+10] = 0;
//     master[15+offset+10] = 0;

//     sub[0] = 1;
//     sub[1] = 1;
//     sub[2] = 0;
//     sub[3] = 1;

//     sub[4] = 1;
//     sub[5] = 1;
//     sub[6] = 0;
//     sub[7] = 1;

//     sub[8] = 0;
//     sub[9] = 1;
//     sub[10] = 0;
//     sub[11] = 1;

//     sub[12] = 1;
//     sub[13] = 1;
//     sub[14] = 0;
//     sub[15] = 0;

    
//     cout << "master=[";
//     for (int i = 0; i < offset+25; i++)
//     {
//         cout << (int)master[i] << ", ";
//     }
//     cout << (int)(master[offset+25]) << "]" <<  endl;

//     cout << "sub=[";
//     for (int i = 0; i < SUB_SIZE-1; i++)
//     {
//         cout << (int)sub[i] << ", ";
//     }
//     cout << (int)sub[SUB_SIZE-1] << "]" << endl; 

//      cout << "address = " << (Index_KMP(master, MASTER_SIZE , sub, SUB_SIZE, 0)) << endl;
// }

// // int _TEST_KMP()
// // {
// //     int master[MASTER_SIZE];
// //     int sub[SUB_SIZE] = {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// //     // for (size_t i = 0; i < MASTER_SIZE; i++)
// //     // {
// //     //     master[i] = 1;
// //     // }
// //     // memset(master, 0x00, MASTER_SIZE);

// //     for (int i = 0; i < MASTER_SIZE; i++)
// //     {
// //         master[i] = 0;
// //     }
    

// //     master[10] = 1;
// //     master[12] = 1;

// //     cout << "master=[";
// //     for (int i = 0; i < MASTER_SIZE-1; i++)
// //     {
// //         cout << (int)master[i] << ", ";
// //     }
// //     cout << (int)master[MASTER_SIZE-1] << "]" <<  endl;
    


// //     cout << "sub=[";
// //     for (int i = 0; i < SUB_SIZE-1; i++)
// //     {
// //         cout << (int)sub[i] << ", ";
// //     }
// //     cout << (int)sub[SUB_SIZE-1] << "]" << endl;   


// //     cout << "address = " << (KMP(master, MASTER_SIZE, sub, SUB_SIZE)) << endl;

// //     return 0;
// // }

// #endif // !_TEST_KMP_H_