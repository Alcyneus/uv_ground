// #include "catch.h"
#include "testTcp.hpp"
#include "testTcpServer.hpp"
#include "testThreadManage.hpp"
#include "testCan.hpp"
#include "testComands.hpp"

#include "testSatelliteSimulation.hpp"
#include "testGPIO.hpp"
#include "testBRAM.hpp"
#include "testBaseband.hpp"
#include "testIIO.hpp"
#include "testControlInformationBean.hpp"
#include "testCaptureTest.hpp"
#include "testRemoteErrorRateTest.hpp"

#include "testKMP.hpp"
#include "testBit_SYNC.hpp"
#include "testSORT.hpp"

#include "testModulationConfigBean.hpp"

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

int main(int argc, char const *argv[])
{  

    // TEST_TCP_SERVER();
    // TEST_THREAD_NOTIFY();

    // TEST_CAN();

    // TEST_COMMANDS();
    // TEST_DOWN_LINK_REMOTE_DATA();
    // TEST_SATELLITE_SIMULATION();

    // TEST_COMMON_GPIO();

    // TEST_COMMON_BRAM();

    // TEST_IIO_BASEBAND();
    // TEST_COMMON_IIO();
    //  TEST_COMMON_IIO_GAIN();
    // TEST_IIO_RECV();
    // TEST_IIO_BIT_SYNC();
    // TEST_SORT();
    // TEST_KMP2();
    // TEST_BIT_SYNC();

    // TEST_CONTROL_INFORMATION();

    // TEST_CAPTURE_TEST_issuedORder();
    // TEST_CAPTURE_judgeSignalLock();

    // TEST_CAPTURE();

    // REMOTE_ERROR_RATE_TEST();


    SWEEP_FREQUENCY();

    return 0;
}

// TEST_CASE( "Factorials are computed", "[factorial]" ) {
//     // REQUIRE( Factorial(1) == 1 );

//     // REQUIRE(outputConsole() == 0 );
//     // REQUIRE(outputConsoleAndWritteToFile() == 0 );
//     // REQUIRE(outputConsoleAndsendToServer() == 0 );
//     // REQUIRE( TEST_COMMON_SOCKET_TCP() == 0 );
//     REQUIRE( TEST_TCP_SERVER() == 0 );

// }