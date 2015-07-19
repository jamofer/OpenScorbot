/*
 * RobotController.cpp
 *
 *  Created on: 8 de oct. de 2015
 *      Author: Javier
 */

#include "RobotController.h"

RobotController::RobotController() {
	// TODO Auto-generated constructor stub
	hlog_buffer = "";
	gui = NULL;
	hrobot_log.open(HROBOT_LOG);
	lrobot_log.open(LROBOT_LOG);
	Logger::setRobotController(this);
	this->rb = new Robot(this);

}

RobotController::~RobotController() {
	// TODO Auto-generated destructor stub
	Logger::logInfo("Shutdown Robot...");
	delete rb;
	rb = NULL;
	if(hrobot_log.is_open()) {
		hrobot_log.close();
	}
	if(lrobot_log.is_open()) {
		lrobot_log.close();
	}
	Logger::setRobotController(NULL);
}

// TODO: review prototype. Should it return something??
void RobotController::calibrateRobot(RobotPacket rp) {
	std::lock_guard<std::mutex> lock(rb->robot_mutex);
	rb->calibrateRobot(rp);

}


bool RobotController::moveRobot(RobotPacket rp) {
	//TODO
	std::lock_guard<std::mutex> lock(rb->robot_mutex);
	return rb->moveTo(rp);
}


int RobotController::getHRobotStatus() {
	return rb->getStatus();
}

int RobotController::getLRobotStatus() {
	return rb->getLRobotStatus();
}

bool RobotController::resetRobot() {
	//TODO
	return rb->resetLRobot();
}

bool RobotController::stopRobot() {
	return rb->stopLRobot();
}

void RobotController::setOwnerMode(int owner) {
	rb->setOwnerMode(owner);
}

void RobotController::setRobotAxisPID(int axis, Axis::PID pid) {
	//std::lock_guard<std::mutex> lock(rb->robot_mutex);
	Logger::logWarning("'%s' not implemented.",__PRETTY_FUNCTION__);
//	rb->setAxisPID()
	//TODO
	//return NULL;
	//return rb->setAxisPID(axis,pid);
}

	//TODO
Axis::PID RobotController::getRobotAxisPID(int axis) {
	Axis::PID pid = {0};
	Logger::logWarning("'%s' not implemented.",__PRETTY_FUNCTION__);
	return pid;
	//return rb->getAxisPID(axis);
	//return rb->getAxisPID(axis);
}

RobotPacket RobotController::getRobotVel() {
	return rb->getRobotVel();
}

void RobotController::setRobotVel(RobotPacket rp) {
	std::lock_guard<std::mutex> lock(rb->robot_mutex);
	rb->setRobotVel(rp);
}

void RobotController::setRobotAccelRates(RobotPacket rp) {
	//std::lock_guard<std::mutex> lock(rb->robot_mutex);
	Logger::logWarning("'%s' not implemented.",__PRETTY_FUNCTION__);
	//rb->setLRobotAccel(rp);
}

RobotPacket RobotController::getRobotAccelRates() {

	return rb->getHRobotAccel();
}

void RobotController::setRobotPos(RobotPacket rp) {

	std::lock_guard<std::mutex> lock(rb->robot_mutex);
	rb->setRobotPos(rp);
}

RobotPacket RobotController::getRobotPos() {
	return rb->getHRobotPos();
}

void RobotController::logHRobot(std::string s, bool gui) {
	if(gui) {
		this->gui->appLogLine(s);
	}
	else if(hrobot_log.is_open()) {
		hrobot_log << s;
		hrobot_log.flush();
	}

}

void RobotController::logLRobot(std::string s) {
	if(lrobot_log.is_open()) {
		lrobot_log << s;
		lrobot_log.flush();
	}
}

void RobotController::setGUI(GUI* gui) {
	this->gui = gui;
}

void RobotController::start() {
	rb->start();
}

RobotPacket RobotController::getRobotDesiredVel() {
	return rb->getDesiredVel();
}

RobotPacket RobotController::getRobotDesiredPos() {
	return rb->getDesiredPos();
}

RobotPacket RobotController::getRobotDesiredCalibration() {
	return rb->getDesiredCalibration();
}
