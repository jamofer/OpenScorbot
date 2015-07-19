/*
 * RobotSocketI2C.cpp
 *
 *  Created on: 8 de oct. de 2015
 *      Author: Javier
 */

#include "RobotSocketI2C.h"
//xd
RobotSocketI2C::RobotSocketI2C(Robot* rb) {
	// TODO Auto-generated constructor stub
	//Init socket
	status = UP;
	this->rb = rb;
	socket = open(I2C_DEVICE,O_RDWR);
	if(socket < 0) {
		Logger::logFatal("HRobot can't open I2C socket.");
		rb->setStatus(Robot::FATAL);
		throw 1;
	}

	if (ioctl(socket,I2C_SLAVE,LR_ADDR) < 0) {
		Logger::logFatal("HRobot can't connect to %x I2C slave device.",LR_ADDR);
		rb->setStatus(Robot::FATAL);
		throw 1;
	}

}

RobotSocketI2C::~RobotSocketI2C() {
	// TODO Auto-generated destructor stub
}

bool RobotSocketI2C::sendPacket(RobotPacket rp) {
	bool success = true;
	char buff[32];
	memcpy(buff,&rp,PACKET_SIZE);

	if(status == DOWN)
		return false;

	genLRC(rp);
	int nrec = write(socket,buff,PACKET_SIZE);
	success = nrec == PACKET_SIZE;
	if(!success) {
		Logger::logError("HRobot failed sending i2c packet(size:(%d)(cmd: %s) errno: %d",nrec,&rp,errno);
		send_failure++;
	}
	else {
		send_failure = 0;
	}

	if(send_failure > MAX_FAILURES && status == UP) {
		status = DOWN;
		rb->setStatus(Robot::Status::LR_DOWN);
		Logger::logFatal("HRobot is sending wrong packets or i2c socket seems down.");
		rb->setStatus(Robot::FATAL);
	}

	return success;

}

RobotPacket RobotSocketI2C::receivePacket() {
	bool success = true;

	RobotPacket rp = {0};

	if(status == DOWN)
		return rp;

	char buff[32];


	int nrec = read(socket,buff,PACKET_SIZE);
	memcpy(&rp,buff,PACKET_SIZE);

	success = (nrec == PACKET_SIZE);

	if(!success) {
		Logger::logError("HRobot failed receiving i2c packet(size:(%d)(cmd: %s) errno: %d",nrec,&rp,errno);
		recv_failure++;
	}
	else if(checkLRC(rp)) {
		recv_failure = 0;
		//Logger::logInfo("Hrobot recvpacket success (%d)",nrec);
		//Logger::logDebug("P(%02x:%02x [%06d,%06d,%06d,%06d,%06d,%06d])",rp.lrc,rp.command,rp.contenti[0],rp.contenti[1],rp.contenti[2],rp.contenti[3],rp.contenti[4],rp.contenti[5]);
	}
	else {
		Logger::logError("HRobot failed receiving i2c packet, wrong checksum");
		recv_failure++;
	}

	if(recv_failure > MAX_FAILURES && status == UP) {
		status = DOWN;
		rb->setStatus(Robot::Status::LR_DOWN);
		Logger::logFatal("LRobot is receiving wrong packets or i2c socket seems down.");
		rb->setStatus(Robot::FATAL);
	}

	return rp;

}
