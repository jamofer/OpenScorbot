/*
 * RobotManager.h
 *
 *  Created on: 7 de oct. de 2015
 *      Author: Javier
 */

#ifndef ROBOTMANAGER_H_
#define ROBOTMANAGER_H_

#include "Robot.h"
#include "RobotParams.h"

#include <cstdlib>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

class Robot;
class RobotManager {
public:
	RobotManager();
	~RobotManager();

	void start(Robot* rb);
	void stop();

private:
	Robot* rb;
	std::thread robot_worker;
	bool robotworker_end;
	void performRobotWork();
};

#endif /* ROBOTMANAGER_H_ */
