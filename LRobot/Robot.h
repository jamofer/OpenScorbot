#ifndef Robot_H
#define Robot_H


#include "mbed.h"
#include "Axis.h"
#include "RobotParams.h"
#include "RobotManager.h"
#include "Logger.h"

class RobotManager;
class Robot {




public:
	
    enum Status {
        INIT = 0x00,
        IDLE = 0x01,
        
        MOVING 		= 0x10,
        MOVING_TO 	= 0x11,
        
        STOP 	= 0x30,
        MC_DOWN = 0x31,
        MC_FAIL = 0x32,
        HR_DOWN = 0x33,
        HR_FAIL = 0x34,
		FATAL 	= 0x35
        
    };


    Robot();
	Robot(RobotManager* rm);
	Robot(PinName rsw, RobotManager* rm);
	~Robot();
	
	
	void setAxisVel(int axis,int vel);
	int getAxisVel(int axis);
	
	void setAxisPos(int axis,int pos);
	int getAxisPos(int axis);
	
	void setAxisAccel(int axis, int accel);
	int getAxisAccel(int axis);

	int getStatus();
	void setStatus(int status);

	bool getAxisHsw(int axis);
	
	void setAxisPID(int axis, Axis::PID pid);
	Axis::PID getAxisPID(int axis);

	bool moveTo(int axis, int pos, bool force=false);
	void hold(int axis);

	bool stop();
	bool isCalibrated();
	
	void performTick();

	


private:
	RobotManager* rm;
	Axis* axes[NUM_AXES];
	Axis::Data infoAxes_desired[NUM_AXES];
	Axis::Data infoAxes_past[NUM_AXES];
	DigitalOut robotSwitch;
	int status;
	int owner;
	int robot_idle_counter[NUM_AXES];
	const int IDLE_MAX_COUNT = 50;
	const int MAX_DIFF_POS = 20;
	const int MC_MAX_RETRY = 5;


	bool start();

	
};
	

#endif
