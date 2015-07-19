/*
 * main.cpp
 *
 *  Created on: 7 de oct. de 2015
 *      Author: Javier
 */

#include <thread>
#include <chrono>
#include <csignal>

#include "Robot.h"
#include "ConsoleGUI.h"

RobotController* rc;
ConsoleGUI* gui;

const int GUI_DELAY = 10;
void init();
void closeProgram( int r_num );

int main() {
	init();
	int a = 0;
	//while((rc->getHRobotStatus() & Robot::STOPPED) != Robot::STOPPED) {
	unsigned int counter = 0;
	while(a != 10) {
		if(GUI_DELAY * counter % 1000 == 0 || a != 0) {//update every second
			gui->updateInfoView();
			//gui->updateLogView();
			gui->updateStatusView();
			//Logger::logDebug("Counter = %d",GUI_DELAY *counter/1000);
		}
		a=gui->checkPrompt();
		counter++;
		std::this_thread::sleep_for(std::chrono::milliseconds(GUI_DELAY)); //25fps
	}
	closeProgram(rc->getHRobotStatus());


}

void init() {
	try {
		rc = new RobotController();
		Logger::logInit("HRobot init...");
		gui = new ConsoleGUI(rc);
		rc->start();
		Logger::logInit("HRobot init complete");
	}
	catch (int e) {
		closeProgram(-1);
	}

}

void closeProgram( int r_num )
{
	int return_num;

    switch(r_num) {
    case Robot::INIT:
    case Robot::MOVING:
    case Robot::MOVING_TO:
    case Robot::CALIBRATING:
    case Robot::IDLE:
    case Robot::STOPPED:
    	Logger::logInfo("HRobot terminate raised");
    	return_num = 0;
    	break;
    case Robot::FATAL:
    	Logger::logFatal("Aborting execution...");
    	return_num = 1;
    	break;
    case -1:
    	Logger::logFatal("Failed to init HRobot, terminating...");
    	return_num = 1;
    	break;
    default:
    	Logger::logFatal("Unexpected exit");
    	return_num = 1;

    }

    if (rc != NULL)
    	delete rc;
    if (gui != NULL)
    	delete gui;

    // cleanup and close up stuff here
    // terminate program

   exit(return_num);

}



