/*
 * RobotSocketI2C.h
 *
 *  Created on: 8 de oct. de 2015
 *      Author: Javier
 */

#ifndef ROBOTSOCKETI2C_H_
#define ROBOTSOCKETI2C_H_

#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include "RobotPacket.h"
#include "RobotParams.h"
#include "Logger.h"

class Robot;
class RobotSocketI2C {
public:

	enum Status {
		UP = 0,
		DOWN = 1
	};

	RobotSocketI2C(Robot* rb);
	~RobotSocketI2C();

	bool sendPacket(RobotPacket rp);
	RobotPacket receivePacket();

private:
	Robot* rb;
	int socket;
	int status;
	int recv_failure = 0;
	int send_failure = 0;
	const int MAX_FAILURES = 10;
};

#endif /* ROBOTSOCKETI2C_H_ */
