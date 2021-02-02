#if !defined(_TEST_THREAD_NOTIFY_HPP_)
#define _TEST_THREAD_NOTIFY_HPP_

#include "threadManage.hpp"

int TEST_THREAD_NOTIFY()
{
    ThreadManage *threadManage = (ThreadManage *)ThreadManage::getInstance();

    pthread_t id[3] = {1, 2, 3};

    threadManage->add("a", &id[0]);
    threadManage->add("b", &id[1]);
    threadManage->add("c", &id[2]);


    std::cout << threadManage->toString() << std::endl;

    return 0;
}




#endif // _TEST_THREAD_NOTIFY_HPP_




