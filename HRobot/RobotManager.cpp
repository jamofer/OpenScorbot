/*
 * RobotManager.cpp
 *
 *  Created on: 7 de oct. de 2015
 *      Author: Javier
 */

#include "RobotManager.h"

RobotManager::RobotManager() : robot_worker() {
	// TODO Auto-generated constructor stub
	rb = NULL;
	robotworker_end = false;
	if(robot_worker.joinable()) {
		robot_worker.join();
	}
}

RobotManager::~RobotManager() {
	// TODO Auto-generated destructor stub
}

void RobotManager::stop() {

	Logger::logInfo("HRobotManager ending...");
	robotworker_end = true;
	if(robot_worker.joinable()) {
		robot_worker.join();
		Logger::logInfo("HRobot robot_worker end");
	}

}

void RobotManager::start(Robot* rb) {
	//TODO
	//return rb->init();
	this->rb = rb;
	robot_worker = std::thread{ [this](){ this->performRobotWork(); } };
	Logger::logInit("HRobot robot_worker started");

}


void RobotManager::performRobotWork() {
	const std::chrono::milliseconds IDLE_DELAY(20); //Sleep 20ms - Delta time
	const std::chrono::milliseconds MOVE_DELAY(10); //Sleep 10ms - Delta time
	std::chrono::steady_clock::time_point t1; //Delta time, begin
	std::chrono::steady_clock::time_point t2; //Delta time, end
	std::chrono::milliseconds duration; //Delta time duration

	while (rb != NULL && rb->getStatus() != Robot::FATAL && !robotworker_end) {
		//TODO

		//t1 = std::chrono::steady_clock::now();
		rb->robotTick();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		//t2 = std::chrono::steady_clock::now();
		//duration = MOVE_DELAY - std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		//std::this_thread::sleep_for(duration);

	}

}
