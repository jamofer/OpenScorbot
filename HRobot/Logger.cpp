/*
 * Logger.cpp
 *
 *  Created on: 23 de sept. de 2015
 *      Author: Javier
 */

#include "Logger.h"

RobotController* Logger::rc = NULL;

bool Logger::logWarning(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::WARNING, args);
	va_end(args);

	return status;
}

bool Logger::logError(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::ERROR, args);
	va_end(args);

	return status;
}

bool Logger::logFatal(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::FATAL, args);
	va_end(args);

	return status;
}

bool Logger::logDebug(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::DEBUG, args);
	va_end(args);

	return status;
}

bool Logger::logInfo(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::INFO, args);
	va_end(args);

	return status;
}

bool Logger::logInit(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::INIT, args);
	va_end(args);

	return status;
}

bool Logger::logLine(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,-1, args);
	va_end(args);

	return status;
}

bool Logger::logGUI(const char * mesg, ...) {
	bool status;
	va_list args;
	char buffer[256];

	va_start(args,mesg);
	vsnprintf(buffer, 256, mesg, args);

	if(rc != NULL) {
		rc->logHRobot(std::string(buffer),true);
		return true;
	}
	else {
		return false;
	}
	va_end(args);

	return status;
}


bool Logger::logMessage(const char * mesg,int msgtype, va_list args) {

	char buffer[256];
	char line[256];



	if(strlen(mesg) > 236)
		return false; //Large lines are forbidden

	vsnprintf(buffer, 256, mesg, args);

	switch(msgtype) {

	case TAG::INFO:
		sprintf(line,"%s[INFO]%s %s\n",BOL,NRM,buffer);
		break;
	case TAG::WARNING:
		sprintf(line,"%s[WARNING]%s %s\n",YEL,NRM,buffer);
		break;
	case TAG::DEBUG:
		sprintf(line,"%s[DEBUG]%s %s\n",GRN,NRM,buffer);
		break;
	case TAG::ERROR:
		sprintf(line,"%s%s[ERROR]%s %s\n",BOL,RED,NRM,buffer);
		break;
	case TAG::FATAL:
		sprintf(line,"%s[FATAL]%s %s\n",RED,NRM,buffer);
		break;
	case TAG::INIT:
		sprintf(line,"%s[INIT]%s %s\n",CYN,NRM,buffer);
		break;
	default:
		sprintf(line,"%s\n",buffer);
	}

	if(rc != NULL) {
		rc->logHRobot(std::string(line));
		return true;
	}
	else {
		return false;
	}
}

void Logger::setRobotController(RobotController* rc) {
	Logger::rc = rc;
}
