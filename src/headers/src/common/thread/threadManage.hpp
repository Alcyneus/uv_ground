#if !defined(_SERVICE_THREAD_ID_HPP_)
#define _SERVICE_THREAD_ID_HPP_

#include <map>
#include <string>
#include <pthread.h>
#include <iostream>
#include <sstream>

#include "debug.hpp"


#if _THREAD_MANAGE_DEBUG_
#define THREAD_MANAGE_MSG(...) printf(__VA_ARGS__)
#define THREAD_MANAGE_DEBUG(...) printf(__VA_ARGS__)
#else
#define THREAD_MANAGE_MSG(...)
#define THREAD_MANAGE_DEBUG(...)  
#endif

class ThreadManage
{
private:
    std::map<std::string, pthread_t*> threadids;

    static volatile ThreadManage *instance;
    static pthread_mutex_t mtx;
public:
    ThreadManage(/* args */);
    ~ThreadManage();

    bool add(std::string, pthread_t*);
    bool del(std::string);
    pthread_t get(std::string);
    std::string toString();


    static volatile ThreadManage *getInstance();
};

volatile ThreadManage * ThreadManage::instance = NULL;
pthread_mutex_t ThreadManage::mtx;

ThreadManage::ThreadManage(/* args */)
{
}

ThreadManage::~ThreadManage()
{
}
volatile ThreadManage *ThreadManage::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new ThreadManage();
        }
        pthread_mutex_unlock(&mtx);
    }

    THREAD_MANAGE_DEBUG("THREAD_MANAGE_DEBUG: Get the %sThreadManage object: %d%s\n",GREEN, instance,NONE);
    
    return instance;
}

bool ThreadManage::add(std::string name, pthread_t* id)
{
    this->threadids.insert(std::map<std::string, pthread_t*>::value_type(name, id));
    THREAD_MANAGE_DEBUG("THREAD_MANAGE_DEBUG: %sAdd thread id [%s:%d]%s\n",LUE,name.c_str(), *id,NONE);
}
bool ThreadManage::del(std::string name)
{
    if(0 != this->threadids.count(name)) 
    {
        THREAD_MANAGE_DEBUG("THREAD_MANAGE_DEBUG: %sDeleta thread id [%s:%d]%s\n",RED,name.c_str(),*threadids[name],NONE);
        this->threadids.erase(name);   
        return true;
    }

    return false;
}
pthread_t ThreadManage::get(std::string name)
{
    if(0 != this->threadids.count(name))
    { 
        THREAD_MANAGE_DEBUG("THREAD_MANAGE_DEBUG: Get thread id [%s:%d]\n",name.c_str(),*threadids[name]);
        return *(this->threadids[name]); 
    }  
    return 0;
}

std::string ThreadManage::toString()
{
    std::stringstream ssream;

    ssream << "Thread ID map:\n";
    for(auto &it : threadids){
        ssream << "[" << it.first << ":" << *(it.second) << "]\n";
    }   

    return ssream.str();
}
#endif // _SERVICE_THREAD_ID_HPP_
