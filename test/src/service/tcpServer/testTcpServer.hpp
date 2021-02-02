#if !defined(_TEST_TCP_SERVER_HPP_)
#define _TEST_TCP_SERVER_HPP_

#include "tcpServer.hpp"

int TEST_TCP_SERVER()
{

    TcpServier *tcpServier = new TcpServier();

    tcpServier->init(1024);

    tcpServier->threadJoin();
    return 0;
}

#endif // _TEST_TCP_SERVER_HPP_
