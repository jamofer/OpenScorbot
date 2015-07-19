#ifndef RobotManager_H
#define RobotManager_H

#include "mbed.h"
#include "Robot.h"
#include "RobotPacket.h"


#define HEARTBEAT_TIMER 5.0 //5seconds

class Robot;
class RobotManager {

public:

    RobotManager();
    ~RobotManager();
    RobotPacket processPacket(RobotPacket rp);
    void performRobotTick();
    void hRobotSocketError();
    void mcSocketError();
    int getRobotStatus();
    void reportStatus();


private:

	Robot* robot;
	Ticker hbTimer;
	int hrobot_fail_counter;
	int mc_fail_counter;
	const int MAX_FAILURES = 10;

    RobotPacket getRobotStatus(RobotPacket rp);
	RobotPacket resetRobot();
    RobotPacket stopRobot();
    RobotPacket setOwnerMode(int owner);
    RobotPacket moveRobot(RobotPacket rp);
    RobotPacket setRobotAxisPID(RobotPacket rp);
    RobotPacket getRobotVel();
    RobotPacket setRobotVel(RobotPacket rp);
    RobotPacket setRobotAccelRates(RobotPacket rp);
    RobotPacket getRobotAccelRates();
    RobotPacket setRobotPos(RobotPacket rp);
    RobotPacket getRobotPos();

    RobotPacket getLRobotLog(RobotPacket rp); //HROBOT and LROBOT shall use the same endianess
    RobotPacket flashLROBOT(RobotPacket rp); //HROBOT and LROBOT shall use the same endianess


    void timeoutStop();
    void heartBeat();
	
};

#endif
    
