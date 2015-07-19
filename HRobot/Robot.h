/*
 * Robot.h
 *
 *  Created on: 8 de oct. de 2015
 *      Author: Javier
 */

#ifndef ROBOT_H_
#define ROBOT_H_

#include "RobotParams.h"
#include "RobotPacket.h"
#include "RobotManager.h"
#include "RobotController.h"
#include "RobotSocketI2C.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

class RobotManager;
class RobotController;
class RobotSocketI2C;

class Robot {
public:
	Robot();
	Robot(RobotController* rm);
	~Robot();


	enum Status {
		INIT = 0x00,
		IDLE = 0x01,

		MOVING 		= 0x10,
		MOVING_TO 	= 0x11,

		CALIBRATING = 0x20,

		STOPPED 	= 0x30,
		LR_DOWN 	= 0x31,
		FATAL		= 0x35

	};

    /* Funcs */
    void setRobotVel(RobotPacket rp);

	bool updateRobotVel();
	RobotPacket getRobotVel();

	void setRobotPos(RobotPacket rp);

	bool updateRobotPos();
	RobotPacket getHRobotPos();

	void setRobotAccel(RobotPacket rp);

	bool updateRobotAccel();
	RobotPacket getHRobotAccel();

	void setAxisPID(int axis, Axis::PID pid);
	Axis::PID getAxisPID(int axis);

	void calibrateRobot(RobotPacket rp);

	bool moveTo(RobotPacket rp);


	int getStatus();
	void setStatus(int status);

	bool updateLRobotStatus();
	int getLRobotStatus();

	bool stopLRobot();
	bool resetLRobot();
	void hold(int axis = -1);

	RobotPacket getDesiredVel();
	RobotPacket getDesiredPos();
	RobotPacket getDesiredCalibration();


	bool isCalibrated();
	//void stopAxisAtLimits();
	void robotTick();

	void start();
	void setOwnerMode(int owner);

	std::mutex robot_mutex;


private:
	Axis* axes[NUM_AXES];
	Axis* desired_axes[NUM_AXES];
	Axis* past_axes[NUM_AXES];
	int status;
	int LRobotStatus;
	int owner;
	int log_counter;
	int robot_idle_counter[NUM_AXES];
	const int IDLE_MAX_COUNT = 50;
	const int MAX_DIFF_POS = 20;


	std::string lrobot_log_buffer;

	void updateInfo();
	bool doCalibrate(int axis);
	void submitLRobotChanges();
	bool getLog();


	bool setLRobotAccel(RobotPacket rp);
	bool setLRobotVel(RobotPacket rp);
	bool setLRobotPos(RobotPacket rp);

	RobotSocketI2C* rbs;



	RobotController* rc;
	RobotManager* rm;


};

#endif /* ROBOT_H_ */
