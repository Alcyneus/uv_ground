#if !defined(_TEST_SIMULATE_SATELLITE_SIMULATION_HPP_)
#define _TEST_SIMULATE_SATELLITE_SIMULATION_HPP_

#include "satelliteSimulation.hpp"


int TEST_SATELLITE_SIMULATION()
{   
    SatelliteSimulation *satellite = (SatelliteSimulation *)SatelliteSimulation::getInstance();


    CAN *can = (CAN *)CAN::getInstance();

    can->init("can0", 500000);

    Commands *cmd1 = new TelemetryPoll();
    Commands *cmd2 = new TelemetryPoll();
    Commands *cmd3 = new TelemetryPoll();
    cmd1->init(can);
    cmd2->init(can);
    cmd3->init(can);

    satellite->addCommand("cmd1",cmd1);
    satellite->addCommand("cmd2",cmd2);
    satellite->addCommand("cmd3",cmd3);


    SIMULATION_DEBUG("SIMULATION_DEBUG: %s\n", satellite->toString().c_str());


    satellite->send("cmd1");

    return 0;
}

#endif // _TEST_SIMULATE_SATELLITE_SIMULATION_HPP_
