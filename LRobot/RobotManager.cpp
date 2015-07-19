#include "RobotManager.h"
#include "Logger.h"



RobotManager::RobotManager() {
	
	robot = new Robot(this);
	//hbTimer.attach(this, &RobotManager::timeoutStop,HEARTBEAT_TIMER);
	hrobot_fail_counter = 0;
	mc_fail_counter = 0;
	
}

RobotManager::~RobotManager() {
	if(robot != NULL)
		delete robot;
}


RobotPacket RobotManager::resetRobot() {
	RobotPacket rp;
	rp.command = RP_CMD::EMPTY;
	Logger::logInfo("HRobot raised RESET");
	mbed_interface_reset();
	return rp; //shall not be called

}

RobotPacket RobotManager::stopRobot() {
	bool success = robot->stop();

	RobotPacket rp;

	rp.command = RP_CMD::STOP;

	
	if(success) {
		Logger::logInfo("HRobot raised STOP");
		robot->setStatus(Robot::STOP);
		rp.command |= RP_CMD::ACK;
	}
	else {
		Logger::logWarning("HRobot raised STOP but failed");
		rp.command |= RP_CMD::NACK;
	}

	return rp;

}

RobotPacket RobotManager::setOwnerMode(int owner) {

	heartBeat();

	Logger::logWarning("Not implemented: %s",__PRETTY_FUNCTION__);
	RobotPacket rp;
	rp.command = RP_CMD::EMPTY;
	return rp;
}

RobotPacket RobotManager::moveRobot(RobotPacket rp) {

	heartBeat();

	if( (robot->getStatus() & Robot::STOP) == Robot::STOP ||
		!robot->isCalibrated()) {
		rp.command |= RP_CMD::NACK;
		Logger::logWarning("LRobot is stopped or not calibrated, can't move! (robot->isCalibrated = %d)",robot->isCalibrated());
	}
	else {
		rp.command |= RP_CMD::ACK;

		for(int i = 0; i < NUM_AXES; i++) {
			if(rp.contentb[i]) {
				robot->moveTo(i,rp.contenti[i]);
			}
			else {
				robot->hold(i);
			}
		}

		robot->setStatus(Robot::MOVING_TO);
	}



	//robot->setMoveProcedureParams(RobotPacket rp)
	//move_ticker.attach(robot->moveProcedure(),50); //Raises moveProcedure every 50ms until reach the desired position

	return rp;
}

RobotPacket RobotManager::setRobotAxisPID(RobotPacket rp) {

	heartBeat();

	Axis::PID pid;
	pid.p = rp.contenti[1];
	pid.i = rp.contenti[2];
	pid.d = rp.contenti[3];
	pid.e = 0;

	robot->setAxisPID(rp.contenti[0],pid);
	rp.command |= RP_CMD::ACK;
	return rp;
}


RobotPacket RobotManager::setRobotVel(RobotPacket rp) {
	
	if((robot->getStatus() & Robot::STOP) == Robot::STOP) {
		rp = {0};
		return rp;
	}

	heartBeat();

	bool isMoving = false;

	for (int i = 0; i < NUM_AXES; i++) {
		if(rp.contentb[i]) { //If is a desired value to change
			robot->setAxisVel(i,rp.contenti[i]);
			isMoving = true;
		}
		else { //Else hold axis
			robot->hold(i);
		}
	}
	rp.command |= RP_CMD::ACK;

	if(isMoving)
		robot->setStatus(Robot::MOVING);
	else
		robot->setStatus(Robot::IDLE);
			
		
	return rp;

}

RobotPacket RobotManager::getRobotVel() {
	heartBeat();
	RobotPacket rp;
	
	rp.command = RP_CMD::GET_PWM;
	
	for (int i = 0; i < NUM_AXES; i++) {
		rp.contenti[i] = robot->getAxisVel(i);
		if(abs(rp.contenti[i]) > MAX_PWR) {
			rp.contentb[i] = false;
		}
		else {
			rp.contentb[i] = true;
		}
	}


	rp.command |= RP_CMD::ACK;

	return rp;
	
}

RobotPacket RobotManager::getRobotPos() {
	heartBeat();
	
	RobotPacket rp;
	rp.command = RP_CMD::GET_COUNTS | RP_CMD::ACK;
	
	for (int i = 0; i < NUM_AXES; i++) {
		rp.contentb[i] = true;
		rp.contenti[i] = robot->getAxisPos(i);
	}


	return rp;
	
}

RobotPacket RobotManager::setRobotPos(RobotPacket rp) {

	/*if(robot->getStatus() != Robot::IDLE) {
		rp = {0};
		return rp;
	}*/

	heartBeat();

	rp.command = RP_CMD::SET_COUNTS | RP_CMD::ACK;

	for (int i = 0; i < NUM_AXES; i++) {
		if (rp.contentb[i]) {
			robot->setAxisPos(i,rp.contenti[i]);
			Logger::logDebug("Axis %d calibrated, position = %d.",i,rp.contenti[i]);
		}
	}

	return rp;

}

RobotPacket RobotManager::getLRobotLog(RobotPacket rp) {
	unsigned int pos = rp.contenti[0] * PAY_LENGTH; //slice number
	int size; //log slice size
	memset(((void *)&rp)+HEAD_LENGTH,0,PAY_LENGTH);
	if((pos + PAY_LENGTH) > (Logger::pos)) { //Reached log end
		size = Logger::pos - pos; //only get existing log
		Logger::logDebug("Pos final: %d, Pos requerida hasta: %d",Logger::pos,pos);
		if(size > 0)
			memcpy(((void *)&rp)+HEAD_LENGTH,Logger::getLog()+pos,size); //Fill RobotPacket with log slice

		rp.command |= RP_CMD::ACK; // mark packet as log end

		Logger::clearLog();

	}
	else {
		size = PAY_LENGTH; //normal case
		memcpy(((void *)&rp)+HEAD_LENGTH,Logger::getLog()+pos,size); //Fill RobotPacket with log slice
	}




	return rp;
}

RobotPacket RobotManager::flashLROBOT(RobotPacket rp) {

	Logger::logWarning("Not implemented: %s",__PRETTY_FUNCTION__);
	rp.command = RP_CMD::EMPTY;

	return rp;
}

RobotPacket RobotManager::setRobotAccelRates(RobotPacket rp) {

	heartBeat();
/*

	bool ack = true;

	for (int i = 0; i < NUM_AXES; i++) {
		if(rp.contentb[i]) {
			v1 = rp.contenti[i];
			v2 = robot->setAxisAccel(i,v1);

			if(v1 != v2) {
				rp.contentb[i] = false;
				rp.contenti[i] = v2;
				ack = false;
			}
		}
	}

	if(ack)
		rp.command |= RP_CMD::ACK;
	else
		rp.command |= RP_CMD::NACK;*/
	Logger::logWarning("Not implemented: %s",__PRETTY_FUNCTION__);
	return rp;

}

RobotPacket RobotManager::getRobotAccelRates() {
	heartBeat();

	RobotPacket rp = {0};
/*	bool ack = true;

	rp.command = RP_CMD::GET_PWM;

	for (int i = 0; i < NUM_AXES; i++) {
		rp.contenti[i] = robot->getAxisAccel(i);
		if(rp.contenti[i] > 0xFF || rp.contenti[i] < 0) {
			rp.contentb[i] = false;
			ack = false;
		}
	}

	if(ack)
		rp.command |= RP_CMD::ACK;
	else
		rp.command |= RP_CMD::NACK;
*/
	Logger::logWarning("Not implemented: %s",__PRETTY_FUNCTION__);
	return rp;

}

RobotPacket RobotManager::getRobotStatus(RobotPacket rp) {

	rp.command |= RP_CMD::ACK;
	rp.contenti[0] = getRobotStatus();

	return rp;
}



int RobotManager::getRobotStatus() {

	return robot->getStatus();
}



void RobotManager::heartBeat() {
	hbTimer.detach(); //Disable timeoutStop callback
	if(robot->getStatus() > 0x30) {
		return;
	}
	if(mc_fail_counter > MAX_FAILURES && robot->getStatus() != Robot::MC_FAIL) { //Check if MC_TX failures reached threshold

		robot->stop();
		Logger::logError("MotorControll max transmission failures reached");
		robot->setStatus(Robot::MC_FAIL);
	}
	else {
		mc_fail_counter = 0;
	}

	
	if(hrobot_fail_counter > MAX_FAILURES && robot->getStatus() != Robot::HR_FAIL) { //Check if HRobotTX failures reached threshold

		robot->stop();
		Logger::logError("HRobot max transmission failures reached");
		robot->setStatus(Robot::HR_FAIL);
	}
	else {
		hrobot_fail_counter = 0;
	}

	//Logger::logDebug("LRobot Beat (Uptime in ms:%d)",LPC_TIM3->TC/1000);


	//LPC_TIM3->TC = 0;	//Restart counter
	hbTimer.attach(this, &RobotManager::timeoutStop,HEARTBEAT_TIMER); //Restarted countdown for timeoutStop
}

void RobotManager::timeoutStop() {
	if ((robot->getStatus() & Robot::STOP) != Robot::STOP) { //If is not in a stop status
		Logger::logFatal("LRobot HeartBeat Timeout (Timer:%d)",LPC_TIM3->TC);
		robot->stop();
		robot->setStatus(Robot::HR_DOWN);
	}
	for(int i = 0; i < NUM_AXES; i++) {
	Logger::logInfo("Motor %d:%d",i,robot->getAxisVel(i));
	Logger::logInfo("Pos %d\r\n",robot->getAxisPos(i));
	}

}

void RobotManager::hRobotSocketError() {
	hrobot_fail_counter++;
	Logger::logError("HRobot socket error. (%d/%d)",hrobot_fail_counter,MAX_FAILURES);
}

void RobotManager::mcSocketError() {
	mc_fail_counter++;
	Logger::logError("MotorController socket error. (%d/%d)",mc_fail_counter,MAX_FAILURES);
}

RobotPacket RobotManager::processPacket(RobotPacket rp) {


	if(robot->getStatus() == Robot::INIT) {
		robot->setStatus(Robot::IDLE);
		hbTimer.attach(this, &RobotManager::timeoutStop,HEARTBEAT_TIMER ); //Restarted countdown for timeoutStop
		Logger::logInit("HRobot socket established.");
	}


	if(!checkLRC(rp)) { //Check RobotPacket integrity
		Logger::logWarning("Integrity error");
		hRobotSocketError();
		rp.command = RP_CMD::EMPTY;
	}
	else {

		switch(rp.command) {//Select command

		case RP_CMD::FLASH_LROBOT:
			rp = flashLROBOT(rp);
			break;
		case RP_CMD::GET_ACCEL_RATES:
			rp = getRobotAccelRates();
			break;
		case RP_CMD::GET_COUNTS:
			rp = getRobotPos();
			break;
		case RP_CMD::GET_LROBOT_LOG:
			rp = getLRobotLog(rp);
			break;
		case RP_CMD::GET_PWM:
			rp = getRobotVel();
			break;
		case RP_CMD::MOVE:
			rp = moveRobot(rp);
			break;
		case RP_CMD::RESET:
			rp = resetRobot();
			break;
		case RP_CMD::SET_ACCEL_RATES:
			rp = setRobotAccelRates(rp);
			break;
		case RP_CMD::SET_COUNTS:
			rp = setRobotPos(rp);
			break;
//		case RP_CMD::SET_MODE_HROBOT:
//			rp = setOwnerMode(HROBOT); //Not implemented (HRobot is owner)
//			break;
//		case RP_CMD::SET_MODE_LROBOT:
//			rp = setOwnerMode(LROBOT); //Not implemented (HRobot is owner)
//			break;
		case RP_CMD::SET_PID:
			rp = setRobotAxisPID(rp);
			break;
		case RP_CMD::SET_PWM:
			rp = setRobotVel(rp);
			break;
		case RP_CMD::STATUS:
			rp = getRobotStatus(rp);
			break;
		case RP_CMD::STOP:
			rp = stopRobot();
			break;
		default:
			//Unknown command
			rp.command = RP_CMD::EMPTY;
		}
	}


	if((rp.command & RP_CMD::NACK) == RP_CMD::NACK) { //Notify MC errors and gen LRC
		this->mcSocketError();
	}

	if(rp.command != RP_CMD::EMPTY) {
		rp.lrc = genLRC(rp);
		//Logger::logDebug("cmdack: %x",rp.command);
		//Logger::logDebug("cmdlrc: %x",rp.lrc);


	}

	return rp;
}

void RobotManager::performRobotTick() {
	robot->performTick();
}

void RobotManager::reportStatus(){
	int a[NUM_AXES];

	for (int i = 0; i < NUM_AXES; i++) {
		a[i] = robot->getAxisVel(i);
	}

	Logger::logInfo("RobotVel: %03d %03d %03d %03d %03d %03d",
				a[0],a[1],a[2],a[3],
				a[4],a[5]);

	for (int i = 0; i < NUM_AXES; i++) {
		a[i] = robot->getAxisPos(i);
	}

	Logger::logInfo("RobotPos: %08d %08d %08d %08d %08d %08d",
				a[0],a[1],a[2],a[3],
				a[4],a[5]);

	Logger::logInfo("RobotStatus: %02x",robot->getStatus());

}
