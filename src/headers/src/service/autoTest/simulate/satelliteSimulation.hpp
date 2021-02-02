#if !defined(_SIMULATE_SATELLITE_SIMULATION_HPP_)
#define _SIMULATE_SATELLITE_SIMULATION_HPP_


#include "commands.hpp"
#include "telemetryPoll.hpp"
#include "remotePoll.hpp"
#include <map>


#ifndef _SIMULATION_DEBUG_
#define SIMULATION_MSG(...) printf(__VA_ARGS__)
#define SIMULATION_DEBUG(...) printf(__VA_ARGS__)
#else
#define SIMULATION_MSG(...)
#define SIMULATION_DEBUG(...)  
#endif




class SatelliteSimulation
{
private:
    std::map<std::string, Commands *> commands;


    static volatile SatelliteSimulation *instance;
    static pthread_mutex_t mtx;

public:
    SatelliteSimulation(/* args */);
    ~SatelliteSimulation();

    static volatile SatelliteSimulation *getInstance();

    bool addCommand(std::string name, Commands *command); 

    uint8_t *send(std::string commandnName);

    std::string toString();
};


volatile SatelliteSimulation * SatelliteSimulation::instance = NULL;
pthread_mutex_t SatelliteSimulation::mtx;

SatelliteSimulation::SatelliteSimulation(/* args */)
{
}

SatelliteSimulation::~SatelliteSimulation()
{
}

volatile SatelliteSimulation *SatelliteSimulation::getInstance()
{
    if (NULL == instance)
    {
        pthread_mutex_lock(&mtx);
        if (NULL == instance)  
        {
            instance = new SatelliteSimulation();
        }
        pthread_mutex_unlock(&mtx);
    }
    SIMULATION_DEBUG("SIMULATION_DEBUG: Get the %s SatelliteSimulation instance: %d%s\n",GREEN, instance,NONE);

    return instance;
}

bool SatelliteSimulation::addCommand(std::string name, Commands *command)
{
    this->commands.insert(std::map<std::string, Commands*>::value_type(name, command));
    SIMULATION_DEBUG("SIMULATION_DEBUG: %sAdd Command [%s:%s]%s\n",LUE,name.c_str(), command->toString().c_str(),NONE);
}
uint8_t *SatelliteSimulation::send(std::string commandnName)
{
    if(0 == this->commands.count(commandnName))
    {
        SIMULATION_DEBUG("SIMULATION_DEBUG: %s no Command [%s]%s\n",RED, commandnName.c_str(),NONE);
        return NULL;
    }

    return this->commands[commandnName]->send();
}

std::string SatelliteSimulation::toString()
{
    std::stringstream ssream;

    ssream << "SatelliteSimulation commands map:\n";
    for(auto &it : commands){
        ssream << "[" << it.first << ":" << it.second->toString() << "]\n";
    }   

    return ssream.str();

}

#endif // _SIMULATE_SATELLITE_SIMULATION_HPP_
