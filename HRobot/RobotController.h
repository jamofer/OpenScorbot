/*
 * RobotController.h
 *
 *  Created on: 8 de oct. de 2015
 *      Author: Javier
 */

#ifndef ROBOTCONTROLLER_H_
#define ROBOTCONTROLLER_H_

#include "Axis.h"
#include "Robot.h"
#include "RobotPacket.h"
#include <thread>
#include <mutex>
#include <iostream>
#include <sstream>
#include <fstream>
#include "RobotParams.h"
#include "GUI.h"

class GUI;
class Robot;
class RobotController {
public:
	RobotController();
	~RobotController();

    // TODO: void or return a boolean value?
    void calibrateRobot(RobotPacket rp);
    bool moveRobot(RobotPacket rp);


    int getHRobotStatus();
    int getLRobotStatus();

	bool resetRobot();
    bool stopRobot();
    void setOwnerMode(int owner);



    void setRobotAxisPID(int axis, Axis::PID pid);
    Axis::PID getRobotAxisPID(int axis);

    RobotPacket getRobotVel();
    void setRobotVel(RobotPacket rp);

    void setRobotAccelRates(RobotPacket rp);
    RobotPacket getRobotAccelRates();

    void setRobotPos(RobotPacket rp);
    RobotPacket getRobotPos();

    RobotPacket getRobotDesiredVel();
    RobotPacket getRobotDesiredPos();
    RobotPacket getRobotDesiredCalibration();

    void logHRobot(std::string s, bool gui = false);
    void logLRobot(std::string s);

    void setGUI(GUI* gui);

    void start();

    //RobotPacket getLRobotLog(RobotPacket rp); //HROBOT and LROBOT shall use the same endianess
    //RobotPacket flashLRobot(RobotPacket rp); //HROBOT and LROBOT shall use the same endianess

private:

    Robot* rb;
    GUI* gui;
    std::ofstream hrobot_log;
    std::ofstream lrobot_log;

    std::string hlog_buffer;
};

#endif /* ROBOTCONTROLLER_H_ */
