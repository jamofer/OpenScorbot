/*
 * Robot.cpp
 *
 *  Created on: 8 de oct. de 2015
 *      Author: Javier
 */

#include "Robot.h"

Robot::Robot(RobotController* rc) {
	// TODO Auto-generated constructor stub
	log_counter = 0;
	lrobot_log_buffer = "";
	this->rc = rc;
	rm = NULL;
	rbs = new RobotSocketI2C(this);
	Logger::logInit("HRobot I2C socket established Addr:0x%02x",LR_ADDR);
	memset(robot_idle_counter,0,(size_t)sizeof(robot_idle_counter));
	status = INIT;
	LRobotStatus = INIT;
	owner = LROBOT;


	Axis::Params ap = {0};

	ap.count_home = BASE_COUNTS_HOME;
	ap.count_limit = BASE_COUNTS_MAX;
	ap.pid.p = BASE_PID_P;
	ap.pid.i = BASE_PID_I;
	ap.pid.d = BASE_PID_D;
	axes[BASE] = new Axis(ap);

	ap.count_home = BODY_COUNTS_HOME;
	ap.count_limit = BODY_COUNTS_MAX;
	ap.pid.p = BODY_PID_P;
	ap.pid.i = BODY_PID_I;
	ap.pid.d = BODY_PID_D;
	axes[BODY] = new Axis(ap);

	ap.count_home = UPPER_ARM_COUNTS_HOME;
	ap.count_limit = UPPER_ARM_COUNTS_MAX;
	ap.pid.p = UPPER_ARM_PID_P;
	ap.pid.i = UPPER_ARM_PID_I;
	ap.pid.d = UPPER_ARM_PID_D;
	axes[UPPER_ARM] = new Axis(ap);

	ap.count_home = FOREARM_PITCH_COUNTS_HOME;
	ap.count_limit = FOREARM_PITCH_COUNTS_MAX;
	ap.pid.p = FOREARM_PITCH_PID_P;
	ap.pid.i = FOREARM_PITCH_PID_I;
	ap.pid.d = FOREARM_PITCH_PID_D;
	axes[FOREARM_PITCH] = new Axis(ap);

	ap.count_home = FOREARM_ROLL_COUNTS_HOME;
	ap.count_limit = FOREARM_ROLL_COUNTS_MAX;
	ap.pid.p = FOREARM_ROLL_PID_P;
	ap.pid.i = FOREARM_ROLL_PID_I;
	ap.pid.d = FOREARM_ROLL_PID_D;
	axes[FOREARM_ROLL] = new Axis(ap);

	ap.count_home = CLIP_COUNTS_HOME;
	ap.count_limit = CLIP_COUNTS_MAX;
	ap.pid.p = CLIP_PID_P;
	ap.pid.i = CLIP_PID_I;
	ap.pid.d = CLIP_PID_D;
	axes[CLIP] = new Axis(ap);


	for(int i = 0; i < NUM_AXES; i++) {
		desired_axes[i] = new Axis(axes[i]);
		past_axes[i] = new Axis(axes[i]);
	}

}


Robot::~Robot() {

	rm->stop();
	delete rm;
	delete rbs;
}

bool Robot::setLRobotVel(RobotPacket rp) {
	RobotPacket rpr;
	bool status;

	rp.command = RP_CMD::SET_PWM;
	rbs->sendPacket(rp); //Send command
	rpr = rbs->receivePacket(); //Get response
	status = checkRobotACK(rpr);

	if(checkRobotCMD(rpr,RP_CMD::SET_PWM)) { //Update local info
		for(int i = 0; i < NUM_AXES; i++) {
			past_axes[i]->setVel(axes[i]->getVel());
			axes[i]->setVel(rp.contenti[i]);
		}

	}
	else {
		status = false;
	}

	return status;

}

bool Robot::updateRobotVel() {
	bool status;
	RobotPacket rp = {0};

	rp.command = RP_CMD::GET_PWM;

	rbs->sendPacket(rp);
	rp = rbs->receivePacket();

	if(checkRobotCMD(rp,RP_CMD::GET_PWM)) { //Update local info
		for(int i = 0; i < NUM_AXES; i++) {
			past_axes[i]->setVel(axes[i]->getVel());
			axes[i]->setVel(rp.contenti[i]);
			//Logger::logDebug("Motor %d: %d",i,axes[i]->getVel());
		}
		status = checkRobotACK(rp);
	}
	else {
		status = false;
	}
	//Logger::logDebug("Updated status %d",status);

	return status;
}

RobotPacket Robot::getRobotVel() {

	RobotPacket rp = {0};

	for(int i = 0; i < NUM_AXES; i++) {
		rp.contenti[i] = axes[i]->getVel();
	}
	return rp;

}

bool Robot::setLRobotPos(RobotPacket rp) {

	bool success;
	RobotPacket rpr;

	rp.command = RP_CMD::SET_COUNTS;
	rbs->sendPacket(rp);
	rpr = rbs->receivePacket();

	if(checkRobotCMD(rpr,RP_CMD::SET_COUNTS)) {
		success = checkRobotACK(rpr);
	}
	else {
		success = false;
	}

	if(success) {
		for(int i = 0; i < 6; i++) {//Update local info
			if(rp.contentb[i]) {
				axes[i]->setPos(rp.contenti[i]);
				axes[i]->setCalibrated(true);
			}
		}
	}

	return success;

}

bool Robot::updateRobotPos() {

	bool status;
	RobotPacket rp = {0};
	rp.command = RP_CMD::GET_COUNTS;
	rbs->sendPacket(rp);
	rp = rbs->receivePacket();

	if(checkRobotCMD(rp,RP_CMD::GET_COUNTS)) { //Update local info
		for(int i = 0; i < NUM_AXES; i++) {
			past_axes[i]->setPos(axes[i]->getPos());
		    past_axes[i]->setHsw(axes[i]->getHsw());
			axes[i]->setHsw(rp.contentb[i]);
			axes[i]->setPos(rp.contenti[i]);
		}
		status = checkRobotACK(rp);
	}
	else {
		status = false;
	}

	return status;
}

RobotPacket Robot::getHRobotPos() {
	RobotPacket rp = {0};

	for(int i = 0; i < NUM_AXES; i++) {
		rp.contenti[i] = axes[i]->getPos();
		rp.contentb[i] = axes[i]->isCalibrated();
	}

	return rp;
}

bool Robot::setLRobotAccel(RobotPacket rp) {

/*	bool status;

	rbs->sendPacket(rp);
	rp = rbs->receivePacket();

	if((rp.command & RP_CMD::CMD_MASK) == RP_CMD::SET_ACCEL_RATES) { //Update local info
		status = (rp.command & RP_CMD::ACK_MASK) == RP_CMD::ACK;
	}
	else {
		status = false;
	}

	return status;*/

	Logger::logWarning("NOT IMPLEMENTED: %s", __PRETTY_FUNCTION__);
	return false;

}

bool Robot::updateRobotAccel() {

	/*bool status;
	RobotPacket rp = {0};
	memset(rp.contentb,true,(size_t)sizeof(rp.contentb));
	rp.command = RP_CMD::GET_ACCEL_RATES;
	rbs->sendPacket(rp);
	rp = rbs->receivePacket();

	if((rp.command & RP_CMD::CMD_MASK) == RP_CMD::GET_ACCEL_RATES) { //Update local info
		for (int i = 0; i < NUM_AXES; i++) {
			axes[i]->setAccel(rp.contenti[i]);
		}
		status = (rp.command & RP_CMD::ACK_MASK) == RP_CMD::ACK;
	}
	else {
		status = false;
	}

	return status;*/

	Logger::logWarning("NOT IMPLEMENTED: %s", __PRETTY_FUNCTION__);
	return false;

}

RobotPacket Robot::getHRobotAccel() {
	RobotPacket rp = {0};

	for (int i = 0; i < NUM_AXES; i++) {
		rp.contenti[i] = axes[i]->getAccel();
	}

	return rp;
}

void Robot::calibrateRobot(RobotPacket rp) {
	//memset(robot_idle_counter,0,(int)sizeof(robot_idle_counter));
	for (int i = 0; i < NUM_AXES; i++) {
		if(rp.contentb[i]){
			robot_idle_counter[i] = 0;
			axes[i]->setCalibrated(false);
			if(status != CALIBRATING && (status & STOPPED) != STOPPED) {
				status = CALIBRATING;
			}
			if(rp.contenti[i] == 2 && desired_axes[i]->getCalibDir() == 1) {
				Logger::logGUI("Confirming calibration axis %d...",i);
				desired_axes[i]->setCalibDir(2); //Manual calibration confirm
			}
			else if(rp.contenti[i] == 2) {
				Logger::logGUI("Axis %d shall request manual calibration before confirm it.",i);
				Logger::logError("Axis %d shall request manual calibration before confirm it.",i);
			}
			else if(rp.contenti[i] == 1) {
				Logger::logGUI("Calibrating axis %d... (MANUAL)",i);
				desired_axes[i]->setCalibDir(1); //Manual calibration request
			}
			else {
				Logger::logGUI("Calibrating axis %d... (AUTO)",i);
				desired_axes[i]->setCalibDir(0);
			}
			desired_axes[i]->setCalibrated(true);

		}
		else {
			desired_axes[i]->setCalibrated(false);
			desired_axes[i]->setCalibDir(0);
			hold(i);
		}
	}

}

bool Robot::doCalibrate(int axis) {

	int vel;
	int lastpos;
	bool calibrated = false;
	bool can_calibrate = false;
	std::string axis_name;

	if(axes[axis]->isCalibrated()) {
		return true;
	}

	switch(axis) {
	case BASE:
		vel = 40;
		axis_name = "BASE";
		can_calibrate = true;
		break;
	case BODY:
		vel = 120;
		axis_name = "BODY";
		can_calibrate = true;
		break;
	case UPPER_ARM:
		vel = 60;
		axis_name = "UPPER_ARM";
		if(axes[BODY]->isCalibrated())
			can_calibrate = true;
		break;
	case FOREARM_PITCH:
		vel = -100;
		axis_name = "FOREARM_PITCH";
		if(axes[BASE]->isCalibrated())
			can_calibrate = true;
		break;
	case FOREARM_ROLL:
		vel = 0;
		axis_name = "FOREARM_ROLL";
		if(axes[CLIP]->isCalibrated())
			can_calibrate = true;
		break;
	case CLIP:
		vel = -MAX_PWR;
		axis_name = "CLIP";
		if(axes[FOREARM_PITCH]->isCalibrated())
			can_calibrate = true;
		break;

	default:
		break;
	}

	if(can_calibrate == false) {
		Logger::logDebug("[Calibrate] Holding axis %d",axis);
		hold(axis);
		return false;
	}

	int manual_calib = desired_axes[axis]->getCalibDir();

	//t1 = std::chrono::steady_clock::now();
	lastpos = past_axes[axis]->getPos();
	if( desired_axes[axis]->getVel() != vel || axes[axis]->getStatus() != Axis::FREE) {
		desired_axes[axis]->setVel(vel);
		axes[axis]->setStatus(Axis::FREE);
		Logger::logDebug("Axis %d vel changed for calib",axis);
		robot_idle_counter[axis] = 0;
	}
	else if(robot_idle_counter[axis] >= IDLE_MAX_COUNT && (manual_calib == 0 || manual_calib == 2) ) {

		int dif = lastpos - axes[axis]->getPos();
		axes[axis]->setPos(axes[axis]->getCountHome());
		axes[axis]->setCalibrated(true);
		past_axes[axis]->setPos(axes[axis]->getCountHome()+dif);
		hold(axis);
		calibrated = true;
		Logger::logInfo("HRobot Axis %s Calibrated",axis_name.c_str());
		Logger::logGUI("HRobot Axis %s Calibrated",axis_name.c_str());
		desired_axes[axis]->setCalibDir(0);

	}
	return calibrated;

}

bool Robot::updateLRobotStatus() {
	RobotPacket rp;
	rp.command = RP_CMD::STATUS;
	rbs->sendPacket(rp);
	rp = rbs->receivePacket();

	if(checkRobotCMD(rp,RP_CMD::STATUS)) {
		this->LRobotStatus = rp.contenti[0];
		return true;
	}

	return false;
}

int Robot::getLRobotStatus() {
	return LRobotStatus;
}

void Robot::setStatus(int status) {
	//lock_status.lock();
	this->status = status;
	//lock_status.unlock();
}

int Robot::getStatus() {
	return status;
}

bool Robot::stopLRobot() {
	bool success;
	RobotPacket rp = {0};
	rp.command = RP_CMD::STOP;

	rbs->sendPacket(rp);
	rp = rbs->receivePacket();

	if(checkRobotCMD(rp,RP_CMD::STOP) && checkRobotACK(rp)) {
		success = true;
		status = STOPPED;
	}
	else
		success = false;

	return success;

}
/*
void Robot::stopAxisAtLimits() {
	int pos;
	int limit;
	int vel;
	bool shall_send = false;
	RobotPacket rp = {0};

	for(int i = 0; i < NUM_AXES; i++) {
		if(axes[i]->isCalibrated()) {
			pos = axes[i]->getPos();
			limit = axes[i]->getCountLimit();
			vel = axes[i]->getVel();

			if(vel > 0 && pos >= limit) { //Block Forward if upper limit reached
				rp.contentb[i] = true;
				shall_send = true;
			}
			else if (vel < 0 && pos <= 0) { //Block Backward if down limit reached
				rp.contentb[i] = true;
				shall_send = true;
			}
		}
	}
	if(shall_send) {
		setLRobotVel(rp);
	}
}*/

void Robot::hold(int axis) {
	//Sets the desired axis position the actual one
	//If axis is out of range, it will set to all the axes.
	if(axis >= 0 && axis < NUM_AXES) {
		axes[axis]->setStatus(Axis::MOVE_TO);
		desired_axes[axis]->setPos(axes[axis]->getPos());
	}
	else {
		for(int i = 0; i < NUM_AXES; i++) {
			hold(i);
		}
		status = IDLE;
	}
}

bool Robot::resetLRobot() {
	RobotPacket rp;
	rp.command = RP_CMD::RESET;

	return rbs->sendPacket(rp);
}

bool Robot::isCalibrated() {
	bool calibrated = true;

	for (int i = 0; i < NUM_AXES; i++) {
		calibrated = axes[i]->isCalibrated();
		if(calibrated == false)
			break;
	}
	return calibrated;
}

void Robot::setRobotVel(RobotPacket rp) {

	memset(robot_idle_counter,0,(int)sizeof(robot_idle_counter));
	for(int i = 0; i < NUM_AXES; i++) {
		if(rp.contentb[i]) {
			desired_axes[i]->setVel(rp.contenti[i]);
			axes[i]->setStatus(Axis::FREE);
		}
		else {
			hold(i);
		}
	}

	// Forward the command to LRobot if it is the owner
	if (owner == LROBOT) {
		setLRobotVel(rp);
	}
	status = MOVING;

}

void Robot::setRobotPos(RobotPacket rp) {
	for(int i = 0; i < NUM_AXES; i++) {
		if(rp.contentb[i]) {
			int dif = axes[i]->getPos() - rp.contenti[i];
			//Update actual axis
			axes[i]->setPos(rp.contenti[i]);
			axes[i]->setCalibrated(true);
			//Desired axis shall be equal
			desired_axes[i]->setPos(rp.contenti[i]);
			desired_axes[i]->setCalibrated(true);
			//Past axis position shall be with the same difference
			past_axes[i]->setPos(past_axes[i]->getPos()-dif);
		}
	}
	//Pass info to LRobot
	setLRobotPos(rp);
}

void Robot::setRobotAccel(RobotPacket rp) {
	for(int i = 0; i < NUM_AXES; i++) {
		if(rp.contentb[i]) {
			desired_axes[i]->setAccel(rp.contenti[i]);
		}
	}
	Logger::logWarning("NOT IMPLEMENTED: %s", __PRETTY_FUNCTION__);
}

bool Robot::moveTo(RobotPacket rp) {
	bool success = true;

	if (isCalibrated() && (status & STOPPED) != STOPPED) {
		memset(robot_idle_counter,0,(int)sizeof(robot_idle_counter));
		for(int i = 0; i < NUM_AXES; i++) {
			if(rp.contentb[i]) {
				desired_axes[i]->setPos(rp.contenti[i]);
				axes[i]->setStatus(Axis::MOVE_TO);
			}
			else {
				hold(i);
			}
		}
		success = true;
		status = MOVING_TO;
	}
	else {
		success = false;
	}

	//If it can move and the owner is LRobot, forward command
	if(owner == LROBOT && success) {
		rp.command = RP_CMD::MOVE;
		rbs->sendPacket(rp);
		rp = rbs->receivePacket();
		success = checkRobotACK(rp);
	}

	return success;
}

void Robot::robotTick() {
	std::lock_guard<std::mutex> lock(robot_mutex);
	updateInfo();//4-6ms
	bool done;

	int dif;

	//Update Axes IDLE Counters
	for (int i = 0; i < NUM_AXES; i++) {
		dif = past_axes[i]->getPos() - axes[i]->getPos();

		if(abs(dif) < MAX_DIFF_POS) {
			if(robot_idle_counter[i] < IDLE_MAX_COUNT) {
				if(robot_idle_counter[i] == (IDLE_MAX_COUNT-1)) {
					Logger::logDebug("axis %d is IDLE",i);
				}
				robot_idle_counter[i]++;
			}
		}
		else {
			robot_idle_counter[i] = 0;
		}
	}

	if (status == CALIBRATING) {
		done = true;
		for (int i = 0; i < NUM_AXES; i++) {
			if(!axes[i]->isCalibrated() && desired_axes[i]->isCalibrated()) {
				done &= doCalibrate(i);
			}
		}
		if(done) {
			Logger::logInfo("Robot is calibrated");
			Logger::logGUI("Robot is calibrated");
		}

	}

	if(owner == HROBOT && status == MOVING_TO) {
		done = true;
		for (int i = 0; i < NUM_AXES; i++) {
			if(robot_idle_counter[i] >= IDLE_MAX_COUNT && abs(desired_axes[i]->getPos()-axes[i]->getPos()) < 500) {
				done &= true;
			}
			else {
				done = false;
			}
		}
		//If yes, then is because is in IDLE state without movement.
		if (done) {
			hold();
		}
	}

	if(owner == HROBOT || status == CALIBRATING) {
		for (int i = 0; i < NUM_AXES; i++) {
			if(axes[i]->getStatus() == Axis::FREE) {
				past_axes[i]->setVel(axes[i]->getVel());
				axes[i]->setVel(desired_axes[i]->getVel());
			}
			else {
				past_axes[i]->setVel(axes[i]->getVel());
				axes[i]->setVel(desired_axes[i]->getPos(),past_axes[i]->getPos());
			}
		}
	}

	submitLRobotChanges();//6ms
}

void Robot::updateInfo() {
	//if(status == INIT || owner == LROBOT) {
		//updateRobotAccel();
		updateRobotVel();

	//}
	updateRobotPos();
	updateLRobotStatus();
	/*if(owner == LROBOT && status != CALIBRATING) {
		status = LRobotStatus;
	}*/
	//Get up to 120 logbytes per tick
	for (int i = 0; i < 3; i++) {
		if(getLog())
			break;
	}
}

void Robot::submitLRobotChanges() {
	RobotPacket rp = {0};
	bool shall_send = false;

	if(status == CALIBRATING) {
		for (int i = 0; i < NUM_AXES; i++) {
			if(axes[i]->isCalibrated() && !past_axes[i]->isCalibrated()) {
				past_axes[i]->setCalibrated(true);
				shall_send = true;
				rp.contentb[i] = true;
				rp.contenti[i] = axes[i]->getPos();
			}
		}
		if(shall_send) {
			setLRobotPos(rp);
		}

		if(isCalibrated()) {
			hold();
		}

	}

	shall_send = false;

	if(status == CALIBRATING || owner == HROBOT) {

		for (int i = 0; i < NUM_AXES; i++) {
			if(axes[i]->getVel() != past_axes[i]->getVel()) {
				shall_send = true;
				//past_axes[i]->setVel(rp.contenti[i]);
			}
			rp.contentb[i] = true;
			rp.contenti[i] = axes[i]->getVel();
		}
		if(shall_send)
			setLRobotVel(rp);

	}

/*	shall_send = false;

	for (int i = 0; i < NUM_AXES; i++) {
		if(axes[i]->getAccel() != past_axes[i]->getAccel()) {
			shall_send = true;
			rp.contentb[i] = true;
			rp.contenti[i] = axes[i]->getAccel();
		}
		else {
			rp.contentb[i] = false;
		}
	}

	if(shall_send)
		setLRobotAccel(rp);*/


}

void Robot::start() {
	updateInfo();
	hold();
	submitLRobotChanges();
	status = IDLE;
	rm = new RobotManager();
	rm->start(this);
}

bool Robot::getLog() {
	RobotPacket rp = {0};
	rp.command = RP_CMD::GET_LROBOT_LOG;
	char buff[PAY_LENGTH +1] = {0}; //Extra char for \x00
	bool status;

	rp.contenti[0] = log_counter;
	rbs->sendPacket(rp);
	rp = rbs->receivePacket();
	memcpy(buff,(void *)((unsigned char *)&rp + HEAD_LENGTH),PAY_LENGTH); //copy loginfo to buff
	lrobot_log_buffer = std::string(buff);
	rc->logLRobot(lrobot_log_buffer);
	log_counter++;


	status = checkRobotACK(rp);
	if(status) {
		if(lrobot_log_buffer.length() > 0) {
			Logger::logDebug("\"%d\" bytes transmitted",lrobot_log_buffer.length());
		}
		log_counter = 0;
	}
	return status;

}
void Robot::setOwnerMode(int owner) {
	this->owner = owner;
}

RobotPacket Robot::getDesiredVel() {
	RobotPacket rp = {0};
	for(int i = 0; i < NUM_AXES; i++) {
		if(axes[i]->getStatus() == Axis::FREE)
			rp.contenti[i] = desired_axes[i]->getVel();
		else {
			rp.contenti[i] = 0x7FFFFFFF;
		}
	}
	return rp;
}

RobotPacket Robot::getDesiredPos() {
	RobotPacket rp = {0};
	for(int i = 0; i < NUM_AXES; i++) {
		if(axes[i]->getStatus() == Axis::MOVE_TO) {
			rp.contenti[i] = desired_axes[i]->getPos();
			rp.contentb[i] = true;
		}
		else {
			rp.contenti[i] = 0x7FFFFFFF;
			rp.contentb[i] = false;
		}
	}
	return rp;
}

RobotPacket Robot::getDesiredCalibration() {
	RobotPacket rp = {0};
	for(int i = 0; i < NUM_AXES; i++) {
		if(desired_axes[i]->isCalibrated()) {
			rp.contenti[i] = 1;
		}
		if (axes[i]->isCalibrated()) {
			rp.contenti[i] += 10;
		}
	}
	return rp;
}
